
#include <ESP.h>
extern "C" {
#include "user_interface.h"
}
#include "rtc.h"
#include "memory.h"
#include "wifi.h"
#include "webServer.h"
#include "server.h"
#include "hardware.h"

#define REANSON_DEEP_SLEEP_AWAKE 5
#define IO2_CONTROL 13
#define MODE_SW     14
#define RESET_CONTROL 12
#define LED         0
#define MAGNET_SW   2
uint32_t nowTime = 0;


void setup() {

  //-----------------------
  //初期設定
  //-----------------------
  Serial.begin(74880);
  timerCounter = 0;
  init_pin();
  wifi_off();
  memory_begin();

  //-----------------------
  //設定モード判定
  //-----------------------
  if (digitalRead(MODE_SW) == LOW) {
    Serial.println("--------------\nWeb Mode\n--------------");
    //Button PUSH
    delay(500);
    rtc_setup ();
    wifi_begin();
    webServer_begin();
    readBattery();
    led_off();
    while (1) {
      webServer_loop();
      if (digitalRead(MAGNET_SW) == LOW) {
        digitalWrite(LED, HIGH);
      } else {
        digitalWrite(LED, LOW);
      }
    }
  }

  //-----------------------
  //割り込み有効化
  //-----------------------
  Interrupt();
  
  //-----------------------
  //バッテリ確認
  //-----------------------
  bool sendFlg = false;
  readBattery();
  Serial.print("batteryRawData : "); Serial.println(getBattery());
  Serial.print("battery : "); Serial.print((float)getBattery() / 130); Serial.println(" V");
  if (getBattery() < 450) {//電源IC下限電圧(2.5V付近)電池切れ防止
    //送信タイミング：3.3Vを割ったとき
    sendFlg = true;
    Serial.println("Battery low Server Data Send");
  }

  //-----------------------
  //時間取得機能有効化
  //-----------------------
  switch (rtc_setup ()) {
    case 1:
    case 2:
      //時間データがないとき
      Serial.println("time set fail");
      break;
  }
  rtc_time_print();

  //-----------------------
  //カウントデータ送信確認
  //-----------------------
  nowTime = rtc_getTime();

  if (memory_getCount() >= 200) {//未送信データ数確認
    sendFlg = true;
    Serial.println("Counter Data Over Server Send");
  }

  if (memory_getLastTime() < nowTime - 3600 * memory_getTimer()) {//指定間隔送信判定
    sendFlg = true;
    Serial.println("Counter Time Over Server Send");
  }

  //-----------------------
  //カウントタイマデータ送信
  //-----------------------
  if (sendFlg == true) {
    wifi_begin();
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("wifi connect!");
      Serial.println("server send Start!");
      if (sendData() == true) {
        memory_setLastTime(nowTime);
        memory_setCount(0);
        memory_save();
        Serial.println("server send Finish!");
      } else {
        Serial.println("server send Error!");
      }
    } else {
      Serial.println("wifi no connect");
    }
  }
  
  //-----------------------
  //DeepSleep判定
  //-----------------------
  if (timerCounter == 0) {//割り込みなく、DeepSleepからの起動の時のみ再度DeepSleepにする
    rst_info *prst = ESP.getResetInfoPtr();
    Serial.printf("reset reason = %d, %s\n", prst->reason, ESP.getResetReason().c_str());

    if (REANSON_DEEP_SLEEP_AWAKE == prst->reason ) {
      led_off();
      Serial.println("Deep Sleep awake ESP.deepSleep");
      ESP.deepSleep(30 * 60 * 1000 * 1000, WAKE_RF_DISABLED ); //30分　（30 * 60 * 1000 * 1000）
      delay(1000);
      return;
    }
  }
  Serial.println("count up");

  //-----------------------
  //カウントアップ
  //-----------------------
  memory_setTimeArray(nowTime);
  Serial.print("interrupt Magnet SW : ");
  Serial.println(timerCounter);
  if (timerCounter != 0) {
    for (int i = 0; i < timerCounter; i++) {
      memory_setTimeArray(nowTime + i); //割り込みされたことを保存する
    }
  }
  memory_save();

  //-----------------------
  //終了処理
  //-----------------------
  wifi_off();
  digitalWrite(LED, LOW);
  digitalWrite(RESET_CONTROL, HIGH);
  digitalWrite(IO2_CONTROL, LOW);
  Serial.println("Deep Sleep Finish");
  ESP.deepSleep(30 * 60 * 1000 * 1000, WAKE_RF_DISABLED ); //30分　（30 * 60 * 1000 * 1000）
  delay(1000);
}


void loop() {
  // put your main code here, to run repeatedly:

}

