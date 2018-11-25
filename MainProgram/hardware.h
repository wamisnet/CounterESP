#ifndef hardware__h
#define hardware__h

#define IO2_CONTROL 13
#define MODE_SW     14
#define RESET_CONTROL 12
#define LED         0
#define MAGNET_SW   2
volatile int timerCounter = 0;
volatile unsigned long time_prev = 0, time_now;
unsigned long time_chat = 1000;

int battery = 0;

void init_pin() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  pinMode(IO2_CONTROL, OUTPUT);
  digitalWrite(IO2_CONTROL, HIGH);
  pinMode(RESET_CONTROL, OUTPUT);
  digitalWrite(RESET_CONTROL, LOW);
  pinMode(MODE_SW, INPUT);
  pinMode(MAGNET_SW, INPUT);
}

void led_off() {
  digitalWrite(LED, LOW);
}

void time_counter() {
  time_now = millis(); //現在の割り込み時刻を取得
  if ( time_now - time_prev > time_chat) {
    timerCounter++;
    time_prev = time_now; //現在の割り込み時刻を前回の割り込み時刻へコピー
  }
}

void Interrupt(){
  attachInterrupt(MAGNET_SW, time_counter, FALLING );
}

void readBattery(){
  battery = analogRead(A0);
}

int getBattery(){
  return battery;
}
#endif
