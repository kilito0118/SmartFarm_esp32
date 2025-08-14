#include <WiFi.h>
#include <WebSocketsClient.h>


#include <LiquidCrystal_I2C.h> // 아두이노 라이브러리 "LiquidCrystal I2C" 다운로드
                              //Download Arduino Library "LiquidCrystal I2C"
#include <DHT.h>
#include <WiFiClientSecure.h>
#include "ReadyMail.h"//아두이노 라이브러리 ReadyMail 다운로드
                      //Download Arduino Library ReadyMail



/*
ESP32 WROOM 보드 이용

21,22번 핀 -> LCD 패널(SDA -> 21, SDL->22)

23번 핀 -> DHT11 온습도 센서

36번 핀 -> 토양 습도 센서

39번 핀 -> 조도 센서


기존 보드에 설치되어 있던 라이브러리와 충돌이 날 경우 메시지를 참고하여
사용하지 않는 라이브러리를 제거해 주시기 바랍니다.

Using ESP32 WROOM Board

Pin 21,22 -> LCD Panel (SDA -> 21, SDL->22)

Pin No. 23 -> DHT11 Temperature and Humidity Sensor

Pin 36 -> Soil Humidity Sensor

Pin 39 -> Illumination Sensor


If there is a conflict with the library installed on the existing board, refer to the message
Please remove unused libraries.
*/


#define ENABLE_SMTP
#define ENABLE_DEBUG

// 핀 설정
// Pin settings
#define LDR_PIN 39
#define SOIL_PIN 36
#define DHTPIN 23
#define DHTTYPE DHT11


// 센서 임계값 (예: 토양 습도 센서 값이 1000 미만일 때 메일 전송)
// Sensor limit setting
const int soilThreshold = 3500;

// 와이파이 정보
// Write your Wifi info
const char* ssid = "MyWiFi";            //와이파이 이름
const char* password = "";                //와이파이 비밀번호(없다면 빈 문자열""로 표현)

// 웹소켓 정보
WebSocketsClient webSocket;
const char* websocket_host = "00.00.00.000"; // 요청받을 웹 서버 주소
const int websocket_port = 5000; //포트번호
const int websocket_port = 5000;
const char* websocket_path = "/ws/esp";

// 메일 서버 및 계정 정보 (Gmail SMTP 465포트 예시)
const char* SMTP_HOST = "smtp.gmail.com";
const uint16_t SMTP_PORT = 465;

const char* AUTHOR_EMAIL    = "@gmail.com";         // 보내는 사람 이메일
const char* AUTHOR_PASSWORD = "";            // 앱 비밀번호
const char* RECIPIENT_EMAIL = "@naver.com";    // 받는 사람

// 시간 관리 변수
unsigned long lastEmailTime = 0;
const unsigned long emailInterval = 1800000; // 30분 (ms)
bool initialEmailSent = false;

// LCD, DHT 센서, ReadyMail SMTP 선언
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

WiFiClientSecure ssl_client;
SMTPClient smtp(ssl_client);

// WebSocket 이벤트 처리 함수 (기존과 동일)
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("[WebSocket] Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("[WebSocket] Connected");
      break;
    case WStype_TEXT:
      Serial.printf("[WebSocket] Message: %s\n", payload);
      break;
  }
}

// ReadyMail 상태 콜백 함수
auto smtpStatusCallback = [](SMTPStatus status) {
  Serial.println(status.text);
  if(status.isComplete) {
    if(status.errorCode < 0) {
      Serial.printf("이메일 전송 실패! 에러코드: %d\n", status.errorCode);
    } else {
      Serial.printf("이메일 전송 성공, 서버 응답 코드: %d\n", status.statusCode);
    }
  }
};

// 메일 발송 함수
void sendSoilEmail(int soilValue) {
  if (!smtp.isConnected()) {
    smtp.connect(SMTP_HOST, SMTP_PORT, smtpStatusCallback);
    delay(1000); // 연결 안정화용 잠시 대기
  }

  if (!smtp.isConnected()) {
    Serial.println("SMTP 서버 연결 실패");
    return;
  }

  if (!smtp.isAuthenticated()) {
    smtp.authenticate(AUTHOR_EMAIL, AUTHOR_PASSWORD, readymail_auth_password);
    delay(500); 
  }

  if (!smtp.isAuthenticated()) {
    Serial.println("SMTP 인증 실패");
    return;
  }

  SMTPMessage &msg = smtp.getMessage();

  msg.headers.clear(); // 기존 헤더 클리어
  msg.text.clear();    // 기존 본문 클리어

  msg.headers.add(rfc822_from, String("ESP32 Sensor <") + AUTHOR_EMAIL + ">");
  msg.headers.add(rfc822_subject, "토양 습도 경고 - ESP32 센서");
  msg.headers.add(rfc822_to, String("관리자 <") + RECIPIENT_EMAIL + ">");

  String body = "경고: 토양 습도 센서 값이 임계치 이하로 감지되었습니다.\r\n";
  body += "현재 토양 습도 값: " + String(soilValue) + "\r\n";
  body += "센서 데이터를 확인하고 조치를 취해주세요.";
  msg.text.body(body);

  // 메일 발송 타임스탬프 (system 시간 동기화되어 있어야 자동 설정)
  msg.timestamp = time(nullptr);

  smtp.send(msg);
}

void setup() {
  Serial.begin(115200);

  // LCD 초기화
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  lcd.clear();
  lcd.print("WiFi Connected");

  dht.begin();

  webSocket.begin(websocket_host, websocket_port, websocket_path);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  ssl_client.setInsecure(); // 인증서 검사 생략(테스트용)

  delay(2000);
  lcd.clear();

  // 처음 실행 시 메일 1회 보내기 -> 일단 센서 수치 읽기
  int initSoilValue = analogRead(SOIL_PIN);
  Serial.printf("초기 토양값: %d\n", initSoilValue);
  sendSoilEmail(initSoilValue);
  initialEmailSent = true;
  lastEmailTime = millis();
}

void loop() {
  webSocket.loop();

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int ldrValue = analogRead(LDR_PIN);
  int soilValue = analogRead(SOIL_PIN);

  if (isnan(h) || isnan(t)) {
    Serial.println("DHT 센서 읽기 실패");
    h = 0; t = 0;
  }

  Serial.printf("Temperature: %.1f C, Humidity: %.1f %%\n", t, h);
  Serial.printf("LDR: %d, Soil: %d\n", ldrValue, soilValue);

  lcd.setCursor(0, 0);
  lcd.printf("t:%.1f h:%.1f", t, h);
  lcd.setCursor(0, 1);
  lcd.printf("c:%4d sl:%4d", ldrValue, soilValue);

  String payload = "{\"temp\":" + String(t, 1) +
                   ",\"hum\":" + String(h, 1) +
                   ",\"ldr\":" + String(ldrValue) +
                   ",\"soil\":" + String(soilValue) + "}";
  webSocket.sendTXT(payload);

  // 토양 습도 임계치 이하이고, 30분 이상 지났으면 메일 발송
  unsigned long currentMillis = millis();
  if (soilValue > soilThreshold) {
    if (!initialEmailSent) {
      sendSoilEmail(soilValue);
      initialEmailSent = true;
      lastEmailTime = currentMillis;
    } else if (currentMillis - lastEmailTime >= emailInterval) {
      sendSoilEmail(soilValue);
      lastEmailTime = currentMillis;
    }
  }

  delay(2000);
}
