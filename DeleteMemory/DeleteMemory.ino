#include <ESP.h>
extern "C" {
#include "user_interface.h"
}
#include "RTClib.h"
#include "memory.h"
#define REANSON_DEEP_SLEEP_AWAKE 5
#define IO2_CONTROL 13
#define MODE_SW     14
#define RESET_CONTROL 12
#define LED         0
#define MAGNET_SW   2
volatile int timerCounter = 0;
RTC_DS3231 rtc;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(74880);
  memory_begin();
  memory_setModule("IoT Counter 001");
  memory_setLastTime(0);
  memory_setPass("");
  memory_setSSID("");
  memory_setCount(0);
  memory_setTimer(1);
  memory_save();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc_time_print();
  Serial.println("memory Delete!");
}


void loop() {
  // put your main code here, to run repeatedly:

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
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print("since midnight 1/1/1970 = ");
  Serial.print(now.unixtime());
  Serial.print("s = ");
  Serial.print(now.unixtime() / 86400L);
  Serial.println("d");
}
