# 🌐 Achiziția datelor de la sisteme IoT și procesarea lor în Cloud

## 📘 Descriere generală
Acest proiect demonstrează o arhitectură completă **hardware-software** pentru monitorizarea și controlul unei sere inteligente, bazată pe microcontrolere STM32 și o platformă edge (Raspberry Pi 4).  
Datele sunt colectate de la senzori, procesate local și vizualizate printr-o interfață mobilă și dashboard-uri Grafana.

---

## 🎯 Scop și motivație
Sistemul urmărește:
- achiziția și prelucrarea datelor de la senzori IoT în timp real;
- transmiterea acestora prin protocoale **MQTT**, **Modbus RTU** și **TCP/IP**;
- stocarea locală și vizualizarea prin **Grafana**;
- controlul și monitorizarea de la distanță printr-o aplicație mobilă.

---

## 🏗️ Arhitectura conceptuală
Arhitectura proiectului este împărțită în mai multe module:
- **Edge Computing (Raspberry Pi 4)** – coordonarea comunicației și rularea serviciilor containerizate;
- **Microcontroler STM32** – achiziția datelor de la senzori și controlul echipamentelor;
- **Rețea locală Wi-Fi** – comunicație între noduri folosind TCP/IP și MQTT;
- **Aplicație mobilă (Flutter/Dart)** – interfață de monitorizare în timp real.

---

## ⚙️ Tehnologii utilizate

### Embedded & Microcontrolere
- STM32  
- Senzori (BMP180, BH1750, Soil Moisture, Dust Sensor GP2Y1014)  
- UART, RS485, Modbus RTU

### Containerizare și orchestrare
- **Docker** și **Docker Compose**

### Servicii software
- **EMQX** – broker MQTT performant (port 1883, dashboard 18083)  
- **PostgreSQL** – stocare locală a datelor  
- **Grafana** – vizualizare date prin dashboard-uri  
- **MQTT Client** – simulare și distribuție de mesaje IoT

### Aplicație mobilă
- **Flutter / Dart** cu pluginul `mqtt_client`

---

## 🧩 Structura repository-ului

```
/src
  ├── bmp180.c / .h
  ├── BH1750_(light_intensity_sensor).c / .h
  ├── soil_moisture.c / .h
  ├── dust_sensor_gp2y1014.c / .h
  ├── actuators.c / .h
  ├── modbus_slave2.c / .h
```

---

## 🖥️ Platformă hardware
- Raspberry Pi 4  
- STM32F4xx  
- Rețea Wi-Fi locală  
- Echipamente control (ventilație, irigare etc.)

---

## 📊 Vizualizare și stocare
Datele colectate sunt salvate într-o bază de date PostgreSQL și afișate în timp real în Grafana, prin dashboard-uri configurabile.

---

## 📱 Aplicație mobilă
Aplicația dezvoltată în Flutter permite monitorizarea datelor și controlul echipamentelor conectate.  
Utilizează protocolul **MQTT** pentru comunicare cu brokerul EMQX.

---

## ✅ Concluzii
- Arhitectura este modulară, scalabilă și funcționează autonom fără conexiune la internet.  
- Protocolul MQTT oferă o comunicare rapidă și fiabilă între nodurile IoT.  
- Docker simplifică implementarea și rularea sistemului.  
- Platforma poate fi extinsă pentru cloud computing, alerte și automatizări AI.

---

## 🔄 Direcții viitoare
- Adăugarea de senzori suplimentari (CO₂, pH, umiditate aer);  
- Notificări mobile (push alerts);  
- Salvare și vizualizare în cloud remote;  
- Control vocal și integrare cu Home Assistant / Google Home.

---

## 👩‍💻 Autor
**Tănăsescu Ștefania-Cătălina**  
Facultatea de Inginerie Electrică și Știința Calculatoarelor  
Specializarea: Automatică și Informatică Aplicată  
Coordonator științific: **Prof. Univ. Constantin Suciu**

---

## 📚 Bibliografie selectivă
- Muhammad Ali Mazidi et al. (2018) – *ARM Cortex-M based Microcontroller Programming*  
- Kyle Simpson (2019) – *MQTT Essentials*  
- Rădulescu F. (2020) – *Programare în Python pentru sisteme IoT*  
- STMicroelectronics – *STM32 Reference Manuals*  
- https://modbus.org
