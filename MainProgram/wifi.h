

void wifi_begin() {
  system_deep_sleep_set_option(RF_CAL);
  String ssid = memory_getSSID();
  Serial.println("SSID : " + ssid);
  if (!ssid.equals("") && ssid.length() <= 32) {
    String pass = memory_getPass();
    Serial.println("Pass : " + pass);
    WiFi.mode(WIFI_AP_STA);
    if (pass.equals("") && pass.length() <= 64) {
      WiFi.begin(ssid.c_str());
    }
    else {
      WiFi.begin(ssid.c_str(), pass.c_str());
    }
  } else {
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin("NefryWiFi", "NefryWiFi");
  }
  int wifiTimeOut = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifiTimeOut++;
    if (wifiTimeOut > 50) {
      break;
    }
  }

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
