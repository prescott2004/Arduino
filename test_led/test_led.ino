const int ohm_RGB[] = {100, 100, 100};
float v1_RGB[3], v2_RGB[3];
float mA_RGB[3];
int PIN_r1RGB[] = {A2, A0, A1};
int PIN_r2RGB[] = {A5, A3, A4};
int PIN_wRGB[] = {9, 11, 10};

char buf_v1_RGB[3][20];
char buf_v2_RGB[3][20];
char buf_mA_RGB[3][20];
char str[80];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for(int rgb=0; rgb<3; ++rgb){
    pinMode(PIN_wRGB[rgb], OUTPUT);
  }
}

void loop() {
  // when off:
  for (int rgb=0; rgb<3; ++rgb){
    digitalWrite(PIN_wRGB[rgb], LOW);
  }
  read();
  delay(1000);
  // when on
  for (int rgb=0; rgb<1; ++rgb){
    digitalWrite(PIN_wRGB[rgb], HIGH);
  }
  read();
  delay(10000);
}

void read(){
  for (int rgb=0; rgb<3; ++rgb){
    v1_RGB[rgb] = analogRead(PIN_r1RGB[rgb]) * 5.0 / 1024.0;
    v2_RGB[rgb] = analogRead(PIN_r2RGB[rgb]) * 5.0 / 1024.0;
    mA_RGB[rgb] = (v2_RGB[rgb] - v1_RGB[rgb]) / (float)(ohm_RGB[rgb]) * 1000.0;
    dtostrf(v1_RGB[rgb], 5, 2, buf_v1_RGB[rgb]);
    dtostrf(v2_RGB[rgb], 5, 2, buf_v2_RGB[rgb]);
    dtostrf(mA_RGB[rgb], 5, 2, buf_mA_RGB[rgb]);
    sprintf(str, "LED%d: %sV, %sV, %smA", rgb+1, buf_v2_RGB[rgb], buf_v1_RGB[rgb], buf_mA_RGB[rgb]);
    Serial.println(str);
  }
}