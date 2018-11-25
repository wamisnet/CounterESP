#include <EEPROM.h>

struct SETTING_COUNTER {
  uint8_t sendTimer;//送信間隔
  uint8_t count;//カウント数
  char sta_ssid[32];
  char sta_pass[64];
  char module_id[32];
  uint32_t lastTime;
  uint32_t timeArray[220];
} sc = {
  1,
  0,
  "",
  "",
  "IoT Counter",
  0
};

void memory_begin() {
  EEPROM.begin(1024);

  EEPROM.get<SETTING_COUNTER>(0, sc);
  Serial.print("Setting_Counter.count : ");
  Serial.println(sc.count);
  
  Serial.print("Setting_Counter.sendTimer : ");
  Serial.println(sc.sendTimer);
}

void memory_save() {
  EEPROM.put<SETTING_COUNTER>(0, sc);
  EEPROM.commit();
}

uint32_t memory_getLastTime(){
  return sc.lastTime;
}

void memory_setLastTime(uint32_t Time){
  sc.lastTime = Time;
  return;
}

uint32_t* memory_getTimeArray(){
  return sc.timeArray;
}

void memory_setTimeArray(uint32_t Time){
  sc.timeArray[sc.count++] = Time;
  return;
}

char* memory_getModule(){
  return sc.module_id;
}

void memory_setModule(const char module[32]){
  strcpy(sc.module_id, module);
  return;
}

char* memory_getPass(){
  return sc.sta_pass;
}

void memory_setPass(const char PASS[64]){
  strcpy(sc.sta_pass, PASS);
  return;
}

char* memory_getSSID(){
  return sc.sta_ssid;
}

void memory_setSSID(const char SSID[32]){
  strcpy(sc.sta_ssid, SSID);
  return;
}

uint8_t memory_getCount(){
  return sc.count;
}

void memory_setCount(uint8_t Count){
  sc.count = Count;
  return;
}

uint8_t memory_getTimer(){
  return sc.sendTimer;
}

void memory_setTimer(uint8_t Timer){
  sc.sendTimer = Timer;
  return;
}
