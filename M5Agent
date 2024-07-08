#include <M5Stack.h>
#include <ArduinoOSCWiFi.h>

#define MIC1 36
#define MIC2 35

#define SAMPLES 500
#define SAMPLING_FREQUENCY 40000

int middle_value = 1920;

int rms1 = 0;
int rms2 = 0;

const IPAddress ip(172, 20, 10, 5);
const IPAddress gateway(172, 20, 10, 1);
const IPAddress subnet(255, 255, 255, 0);
const char* host = "172.20.10.3";

void setup() {
  M5.begin();
  M5.Speaker.write(0); // スピーカーをオフする
  Serial.begin(9600);
  M5.lcd.setBrightness(20);  // LCDバックライトの輝度を下げる

  pinMode(MIC1, INPUT);
  pinMode(MIC2, INPUT);

  WiFi.begin("rikiのiPhone", "12345678");
  WiFi.config(ip, gateway, subnet);
    
  OscWiFi.publish(host, 7181 , "/micin", rms1, rms2);

}

void loop() {
  M5.update();
  unsigned int sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));

  int buff1[SAMPLES];
  int buff2[SAMPLES];

  for (int i = 0; i < SAMPLES; i++)
  {
    unsigned long t = micros();
    buff1[i] = analogRead(MIC1);
    buff2[i] = analogRead(MIC2);
    while ((micros() - t) < sampling_period_us) ;
  }

  rms1 = 0;
  rms2 = 0;

  for (int i = 0; i < SAMPLES; i++)
  {
    int vol1 = abs(buff1[i] - middle_value);
    int vol2 = abs(buff2[i] - middle_value);

    rms1 += vol1;
    rms2 += vol2;
  }

  rms1 = round(rms1/SAMPLES);
  rms2 = round(rms2/SAMPLES);

  //Serial.print(peak1);
  //Serial.print(" ");
  //Serial.println(peak2);

  OscWiFi.update(); 
}
