#ifndef server__h
#define server__h
#include "hardware.h"
#include <ESP8266HTTPClient.h>
#include <aJSON.h>
#include <ESP8266httpUpdate.h>
String sendDataPacket;

void dataFormat() {
  sendDataPacket = "{\"version\":1,\"count\":[";
  int i;
  if (memory_getCount() != 0) {
    for (i = 0; i < memory_getCount() - 1; i++) {
      sendDataPacket += memory_getTimeArray()[i];
      sendDataPacket += ",";
    }
    sendDataPacket += memory_getTimeArray()[i];
  }
  sendDataPacket += "],\"address\":\"";
  sendDataPacket += WiFi.macAddress();
  sendDataPacket += "\",\"battery\":";
  sendDataPacket += getBattery();
  sendDataPacket += "}";
  Serial.println(sendDataPacket);
}

//Serverデータ送信
bool sendData() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("sendData dataFormat\n");
    dataFormat();

    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    http.begin("http://cloud.nefry.studio:1880/nefrysetting/iotcounter");
    int httpCode = http.POST(sendDataPacket);

    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
        aJsonObject* root = aJson.parse((char *)payload.c_str());

        // パースが成功したか確認。できなきゃ終了
        if (root == NULL) {
          Serial.println("parseObject() failed");
        } else {
          aJsonObject* updateUrl = aJson.getObjectItem(root, "update");
          if (updateUrl != NULL) {
            String updateFirmware = updateUrl->valuestring;
            Serial.println(updateFirmware);
            if (updateFirmware.length() != 0) {
              Serial.println("updateFirmware start");
              t_httpUpdate_return ret = ESPhttpUpdate.update(updateFirmware);
              switch (ret) {
                case HTTP_UPDATE_FAILED:
                  Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                  break;
                case HTTP_UPDATE_NO_UPDATES:
                  Serial.println("HTTP_UPDATE_NO_UPDATES");
                  break;
                case HTTP_UPDATE_OK:
                  Serial.println("HTTP_UPDATE_OK");
                  break;
              }
            }
          }
        }
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      return true;
    }
    return false;
  }
  Serial.println("sendData wifi no connection");
  return false;
}

#endif

