#ifndef rtc__h
#define rtc__h
#include <Wire.h>
#include "RTClib.h"
#include <ESP8266WiFi.h>
#include <time.h>
#define JST     3600*9

RTC_DS3231 rtc;

int rtc_setup () {
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    return 1;
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    return 2;
  }
  return 0;
}

void rtc_time_print() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
}

uint32_t rtc_getTime() {
  DateTime now = rtc.now();
  return now.unixtime();
}

#endif
