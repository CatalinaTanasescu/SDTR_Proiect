#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"

#include "tcpserver.h"
#include "string.h"
#include "main.h"

#include "tcpserver.h"
struct senzor_data sensorData; // aici definim efectiv structura

extern osMutexId_t sensorDataMutexHandle;

static struct netconn *conn, *newconn;
static char smsg[200];

static void tcp_thread(void *arg)
{
    err_t err, accept_err;

    /* Creare conexiune TCP */
    conn = netconn_new(NETCONN_TCP);
    if (conn != NULL)
    {
        /* Bind serverul TCP pe portul 7 */
        err = netconn_bind(conn, IP_ADDR_ANY, 7);

        if (err == ERR_OK)
        {
            /* Ascultă conexiuni pe portul 7 */
            netconn_listen(conn);

            while (1)
            {
                /* Așteaptă conexiuni de la client */
                accept_err = netconn_accept(conn, &newconn);
                if (accept_err == ERR_OK)
                {
                    while (1)
                    {
                    	osMutexAcquire(sensorDataMutexHandle, osWaitForever);

                    	// ✅ Date reale din senzor
                    	int len = sprintf(smsg,
                    	    "🌡 TEMP: %.1f °C\r\n💡 LUX: %d\r\n🌫 DUST: %d ppm\r\n🌱 SOIL: %d %%\r\n"
                    	    "🔧 FAN: %s\r\n💧 PUMP: %s\r\n🧱 PISTON: %s\r\n💡 BEC: %s\r\n",
                    	    sensorData.temp,
                    	    sensorData.lux,
                    	    sensorData.praf,
                    	    sensorData.soil_moisture,
                    	    sensorData.fan ? "ON" : "OFF",
                    	    sensorData.pump ? "ON" : "OFF",
                    	    sensorData.piston ? "ON" : "OFF",
                    	    sensorData.bec ? "ON" : "OFF"
                    	);

                    	 osMutexRelease(sensorDataMutexHandle);



                        /* Trimite mesajul către client */
                        err_t write_err = netconn_write(newconn, smsg, len, NETCONN_COPY);

                        /* Verifică dacă clientul mai este conectat */
                        if (write_err != ERR_OK)
                        {
                            netconn_close(newconn);
                            netconn_delete(newconn);
                            break;  // Iese din while și așteaptă o nouă conexiune
                        }

                        /* Așteaptă 5 secunde înainte de a trimite următorul mesaj */
                        sys_msleep(5000);
                    }
                }
            }
        }
        else
        {
            netconn_delete(conn);
        }
    }
}

void tcpserver_init(void)
{
    sys_thread_new("tcp_thread", tcp_thread, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityNormal);
}
