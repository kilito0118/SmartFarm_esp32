# 🌱 ESP32 WROOM 기반 스마트팜 관측기  
**Smart Farm Monitoring Device using ESP32 WROOM Board + Flask Web Server**

## 📌 프로젝트 소개 | Project Overview
이 프로젝트는 **ESP32 WROOM 보드**와 **Flask 웹 서버**를 이용해 스마트팜 환경 데이터를 실시간으로 관측하고 웹 페이지에서 확인할 수 있는 시스템입니다.  
ESP32는 온도·습도, 토양 습도, 조도 데이터를 측정하여 LCD 패널에 표시하며, 동시에 Flask 서버로 데이터를 전송해 PC나 모바일 브라우저에서 모니터링할 수 있습니다.

This project uses an **ESP32 WROOM board** and **Flask web server** to monitor smart farm environmental data in real time and view it via a web page.  
ESP32 measures temperature & humidity, soil moisture, and illumination, displaying them on an LCD panel while also sending data to the Flask server for monitoring on PC or mobile browsers.

---

## 🔧 주요 기능 | Key Features
- **실시간 센서 데이터 측정** (온도·습도, 토양 습도, 조도)  
- **LCD 패널 출력** 및 **웹 페이지 출력**  
- PC, 스마트폰에서 브라우저로 데이터 확인 가능  
- 스마트팜 자동화 시스템 확장 가능  

- **Real-time sensor data measurement** (temperature & humidity, soil moisture, illumination)  
- **Display on LCD panel** and **on web page**  
- View data on PC or mobile browser  
- Expandable for smart farm automation  

---

## 📡 핀 연결 | Pin Connections
| 센서 / 장치 (Device)                | 핀 번호 (Pin No.) | 설명 (Description)                          |
|-------------------------------------|-------------------|----------------------------------------------|
| LCD Panel (SDA, SCL)                | 21 (SDA), 22 (SCL)| I²C 통신 (I²C Communication)                |
| DHT11 Temperature & Humidity Sensor | 23                | 온도 및 습도 측정 (Temperature & Humidity)  |
| Soil Humidity Sensor                | 36                | 토양 습도 측정 (Soil Moisture)              |
| Illumination Sensor                 | 39                | 조도 측정 (Illumination)                    |

---

## ⚙️ 설치 방법 | Installation

### 1. ESP32 펌웨어 업로드  
1. Arduino IDE에서 ESP32 보드 매니저 설치  
2. 필요한 라이브러리 설치:  
   - `LiquidCrystal_I2C`  
   - `DHT`  
   - 기타 센서 라이브러리  
3. 위 표에 맞게 센서 연결 후 ESP32 코드 업로드  

### 2. Flask 웹 서버 설치 및 실행  
```bash
# Flask 설치
pip install flask

# Flask 서버 실행 (예: server.py)
python server.py
```

Flask 서버 실행 후, 브라우저에서 아래 주소로 접속:  
```
http://localhost:5000
```
ESP32가 주기적으로 센서 데이터를 HTTP 요청으로 전송하면, 웹 페이지에서 최신 데이터가 표시됩니다.

---

## ⚠️ 주의사항 | Notes
- 기존 보드에 설치된 라이브러리와 충돌이 발생할 경우, **Arduino IDE의 에러 메시지를 확인**하고 사용하지 않는 라이브러리를 제거하세요.  
- Flask 서버 실행 시, ESP32 코드에서 서버 IP 주소를 정확히 입력해야 합니다. (예: `192.168.x.x`)  
- If there is a conflict with the library installed on the existing board, **refer to the Arduino IDE error message** and remove unused libraries.  
- When running the Flask server, ensure that the **server IP in ESP32 code** matches your network environment.

---

## 📄 라이선스 | License
이 프로젝트는 [MIT License](LICENSE)를 따릅니다.  
This project is licensed under the [MIT License](LICENSE).
