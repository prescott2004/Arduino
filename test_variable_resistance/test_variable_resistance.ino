float vol_A0, vol_A1, current, res;
char str[80];
char buf_A0[20], buf_A1[20], buf_current_mA[20], buf_res_K[20];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  vol_A0 = analogRead(A0) * 5.0 / 1024.0;
  vol_A1 = analogRead(A1) * 5.0 / 1024.0;
  current = (5-vol_A0) / 330;
  res = (vol_A0 - vol_A1) / current; 
  dtostrf(vol_A0, 5, 2, buf_A0);
  dtostrf(vol_A1, 5, 2, buf_A1);
  dtostrf(current*1000, 5, 2, buf_current_mA);
  dtostrf(res/1000, 5, 2, buf_res_K);
  sprintf(str, "Volume: %sV %sV, Current: %smA, Res: %sKOhm", buf_A0, buf_A1, buf_current_mA, buf_res_K);
  Serial.println(str);
  delay(1000);
}
