#ifndef UNIT_TEST
#include <Arduino.h>
#endif
/* 赤外線受信用 */
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
/* WiFi接続用 */
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

/* 赤外線受信用 */
IRrecv irrecv(14);
decode_results results;
/* WiFi接続用 */
ESP8266WiFiMulti WiFiMulti;

void setup() {
  pinMode(12, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  irrecv.enableIRIn();
  WiFiMulti.addAP("toyotas", "kojik0ji");
}

void loop() {
  /* ボタン待機 */
  Serial.println("[Info] push waiting...");
  while(digitalRead(12)) delay(1);
  tone(13, 349, 100); // sound
  delay(100); // sound

  /* 赤外線受信 待機 */
  Serial.println("[Info] ir waiting...");
  irrecv.resume();
  while(irrecv.decode(&results) == false) delay(1);
  tone(13, 392, 100); // sound
  delay(200); // sound
  tone(13, 392, 100); // sound
  delay(100); // sound

  /* 赤外線 信号parse */
  String bits = uint64ToString(results.bits, 16);
  String addr = uint64ToString(results.address, 16);
  String type = uint64ToString(results.decode_type, 16);
  String val = uint64ToString(results.value, 16);

  /* WiFi 接続待機 */
  while(WiFiMulti.run() != WL_CONNECTED) delay(1);

  /* サーバへ送信 */
  HTTPClient http;
  Serial.println("[HTTP] begin...");
  http.begin("http://192.168.2.8/ir?bits=" + bits + "&addr=" + addr + "&type=" + type + "&val=" + val);
  
  Serial.println("[HTTP] GET...");
  int httpCode = http.GET();
  
  if(httpCode > 0) {
    Serial.printf("[HTTP] Response code: %d\n", httpCode);
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] Response... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

  /* 終了音 */
  tone(13, 440, 100); // sound
  delay(200); // sound
  tone(13, 494, 100); // sound
  delay(200); // sound
  tone(13, 523, 500); // sound
  delay(500); // sound
}
