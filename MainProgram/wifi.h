void wait_wifi() {
  int wifiTimeOut = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifiTimeOut++;
    if (wifiTimeOut > 50) {
      break;
    }
  }
}

void wifi_begin() {
  system_deep_sleep_set_option(RF_CAL);
  String ssid = memory_getSSID();
  Serial.println("SSID : " + ssid);
  if (!ssid.equals("") && ssid.length() <= 32) {
    String pass = memory_getPass();
    Serial.println("Pass : " + pass);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(90);
    if (memory_getMode() == 0) {
      int founds = WiFi.scanNetworks();
      if (founds <= 0) {
        Serial.println(F("no networks found"));
      }
      else {
        Serial.print(founds);
        Serial.println(F(" networks found"));
        for (int i = 0; i < founds; ++i) {
          // Print SSID and RSSI for each network found
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.print(WiFi.SSID(i));
          Serial.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial.println(")");
          if (WiFi.SSID(i).equals(ssid)) {
            if (pass.equals("") && pass.length() <= 64) {
              WiFi.begin(ssid.c_str());
            }
            else {
              WiFi.begin(ssid.c_str(), pass.c_str());
            }
            wait_wifi();
          }
        }
      }
    } else {
      if (pass.equals("") && pass.length() <= 64) {
        WiFi.begin(ssid.c_str());
      }
      else {
        WiFi.begin(ssid.c_str(), pass.c_str());
      }
      wait_wifi();
    }
  } else {
    WiFi.begin("NefryWiFi", "NefryWiFi");
    wait_wifi();
  }
  WiFi.mode(WIFI_AP_STA);
  String module = memory_getModule();
  Serial.println("\nmodule : " + module);
  WiFi.softAP(module.c_str(), "iotcounter");

  Serial.println(WiFi.localIP());
}

void wifi_off() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
}
