🌱 ESP32 WROOM 기반 스마트팜 관측기

Smart Farm Monitoring Device using ESP32 WROOM Board

📌 프로젝트 소개 | Project Overview

이 프로젝트는 ESP32 WROOM 보드를 이용해 스마트팜 환경 데이터를 실시간으로 관측하는 장치입니다.
온도·습도, 토양 습도, 조도 데이터를 측정하여 LCD 패널에 표시하며, 스마트팜 자동화 및 데이터 로깅 시스템에 쉽게 확장할 수 있습니다.

This project uses an ESP32 WROOM board to monitor smart farm environmental data in real time.
It measures temperature & humidity, soil moisture, and illumination, displaying them on an LCD panel. The system can be easily extended for smart farm automation and data logging.

🔧 주요 기능 | Key Features

실시간 센서 데이터 측정 (온도·습도, 토양 습도, 조도)

LCD 패널 출력으로 직관적인 상태 확인

스마트팜 자동화 시스템 확장 가능

간단한 배선과 코드로 빠른 구축 가능

Real-time sensor data measurement (temperature & humidity, soil moisture, illumination)

LCD panel display for quick visual feedback

Expandable for smart farm automation

Simple wiring and code for quick setup

📡 핀 연결 | Pin Connections
센서 / 장치 (Device)	핀 번호 (Pin No.)	설명 (Description)
LCD Panel (SDA, SCL)	21 (SDA), 22 (SCL)	I²C 통신 (I²C Communication)
DHT11 Temperature & Humidity Sensor	23	온도 및 습도 측정 (Temperature & Humidity)
Soil Humidity Sensor	36	토양 습도 측정 (Soil Moisture)
Illumination Sensor	39	조도 측정 (Illumination)
⚙️ 설치 방법 | Installation

ESP32 보드 매니저 설치

Arduino IDE → Preferences → Additional Board Manager URLs에 ESP32 보드 URL 추가

Tools → Board → Board Manager에서 ESP32 설치

필요한 라이브러리 설치

LiquidCrystal_I2C (LCD용)

DHT (DHT11 센서용)

기타 센서에 맞는 라이브러리 설치

핀 연결 후 코드 업로드

위 표에 따라 센서를 연결

제공된 코드 업로드

⚠️ 주의사항 | Notes

기존 보드에 설치된 라이브러리와 충돌이 발생할 경우, Arduino IDE의 에러 메시지를 확인하고 사용하지 않는 라이브러리를 제거하세요.

If there is a conflict with the library installed on the existing board, refer to the Arduino IDE error message and remove unused libraries.

📄 라이선스 | License

이 프로젝트는 MIT License를 따릅니다.
This project is licensed under the MIT License.
