//  10k ohm
float R_k = 10;
char buf[60];
char buf_mV[20], buf_uA[20];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val_sensor = analogRead(A0);
  int val_sensor_max = analogRead(A1);
  // convert value to mV
  float val_mV = (float)val_sensor * 5080 / 1024;
  // convert value to uA
  float val_uA = (float)val_mV / R_k;
  // print data
  dtostrf(val_mV, 0, 2, buf_mV);
  dtostrf(val_uA, 0, 2, buf_uA);
  sprintf(buf, "val: %d(max: %d), %s[mV], %s[uA]", val_sensor, val_sensor_max, buf_mV, buf_uA);
  Serial.println(buf);
  delay(1000);
}
