int cnt = 0;
const int ohm_RGB[] = { 100, 100, 100 };
float voltR[3], voltG[2], voltB;
float mA_RGB[3];
float mean_mA_RGB[3];
float ohm_vr;
int PIN_Rin[] = { A0, A1, A2 };
int PIN_Gin[] = { A3, A4 };
int PIN_Bin = A5;
int PIN_RGBout[] = { 9, 10, 11 };

char buf_voltR[3][20], buf_voltG[2][20], buf_voltB[20];
char buf_mA_RGB[3][20];
char buf_ohm_vr[20];
char str[80];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
    pinMode(PIN_RGBout[i_rgb], OUTPUT);
  }
}

void loop() {
  sprintf(str, "cnt: %d", cnt);
  Serial.println(str);
  if (cnt < 5) {
    // when off:
    Serial.println("OFF:");
    for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
      digitalWrite(PIN_RGBout[i_rgb], LOW);
    }
    read();
    delay(1000);
    // when on
    Serial.println("ON:");
    for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
      digitalWrite(PIN_RGBout[i_rgb], HIGH);
    }
    read();
    // calculate average current
    for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
      mean_mA_RGB[i_rgb] += mA_RGB[i_rgb];
    }
    delay(1000);
  } else if (cnt == 5) {
    // calculate average current
    for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
      mean_mA_RGB[i_rgb] /= 5;
    }
    // display mean current
    for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
      dtostrf(mean_mA_RGB[i_rgb], 5, 2, buf_mA_RGB[i_rgb]);
    }
    sprintf(str, "---mean current of RGB: %smA %smA %smA---", buf_mA_RGB[0], buf_mA_RGB[1], buf_mA_RGB[2]);
    Serial.println(str);
  } else {
    // reset
    for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
      digitalWrite(PIN_RGBout[i_rgb], LOW);
    }
    int ratio_RGB[3] = { 0 };
    // R-LED, G-LED, B-LED単体のとき（各々15mAの電流を流す）
    for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
      // calculate
      ratio_RGB[i_rgb] = 255 * 15.0 / mean_mA_RGB[i_rgb];
      if (ratio_RGB[i_rgb] > 255) {
        ratio_RGB[i_rgb] = 255;
      }
      // lightup
      lightup(ratio_RGB, 2000);
      // reset
      ratio_RGB[i_rgb] = 0;
    }



    // PDを使う場合の最適値（R+G:B=0.25:0.37)
    for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
      // calculate
      switch (i_rgb) {
        case 0:
          ratio_RGB[0] = 255 * 15.0 / mean_mA_RGB[0];
          break;
        case 1:
          ratio_RGB[1] = 255 * 15.0 * 5.865873551 / 7.0 / mean_mA_RGB[1];
          break;
        case 2:
          ratio_RGB[2] = 255 * 15.0 * (2 * 0.37 / 0.25 * 0.610694143) / 2.5 / mean_mA_RGB[1];
          break;
      }
      if (ratio_RGB[i_rgb] > 255) {
        ratio_RGB[i_rgb] = 255;
      }
      // lightup
      lightup(ratio_RGB, 2000);
      // reset
      ratio_RGB[i_rgb] = 0;
    }


    // R-LEDとG-LEDを混ぜ合わせたとき
    // calculate
    for (int i_ratio = 0; i_ratio < 21; ++i_ratio) {
      ratio_RGB[0] = 255 * 15.0 / mean_mA_RGB[0] * (float)i_ratio / 20.0;
      ratio_RGB[1] = 255 * 15.0 * 5.865873551 / 7.0 / mean_mA_RGB[1] * (float)(20 - i_ratio) / 20.0;
      // lightup
      lightup(ratio_RGB, 2000);
    }
    // reset
    ratio_RGB[0] = 0;
    ratio_RGB[1] = 0;
    
    
    // R-LED, G-LED, B-LEDを混ぜ合わせたとき
    // calculate
    ratio_RGB[2] = 255 * 15.0 * (1 * 0.37 / 0.25 * 0.610694143) / 2.5 / mean_mA_RGB[1];
    for (int i_ratio = 0; i_ratio < 21; ++i_ratio) {
      ratio_RGB[0] = 255 * 15.0 / mean_mA_RGB[0] * (float)i_ratio / 20.0;
      ratio_RGB[1] = 255 * 15.0 * 5.865873551 / 7.0 / mean_mA_RGB[1] * (float)(20 - i_ratio) / 20.0;
      // lightup
      lightup(ratio_RGB, 2000);
    }
    // reset
    ratio_RGB[0] = 0;
    ratio_RGB[1] = 0;
    ratio_RGB[2] = 0;
  }
  ++cnt;
}

void read() {
  // calculate Volt
  for (int r = 0; r < 3; ++r) {
    voltR[r] = analogRead(PIN_Rin[r]) * 5.0 / 1024.0;
    dtostrf(voltR[r], 5, 2, buf_voltR[r]);
  }
  for (int g = 0; g < 2; ++g) {
    voltG[g] = analogRead(PIN_Gin[g]) * 5.0 / 1024.0;
    dtostrf(voltG[g], 5, 2, buf_voltG[g]);
  }
  voltB = analogRead(PIN_Bin) * 5.0 / 1024.0;
  dtostrf(voltB, 5, 2, buf_voltB);
  // calculate current
  mA_RGB[0] = (voltR[0] - voltR[1]) * 1000.0 / (float)ohm_RGB[0];
  mA_RGB[1] = (voltG[0] - voltG[1]) * 1000.0 / (float)ohm_RGB[1];
  mA_RGB[2] = (4.5 - voltB) * 1000.0 / (float)ohm_RGB[2];
  for (int i = 0; i < 3; ++i) {
    dtostrf(mA_RGB[i], 5, 2, buf_mA_RGB[i]);
  }
  // calculate variable resistance
  ohm_vr = (voltR[1] - voltR[2]) * 1000.0 / mA_RGB[0];
  dtostrf(ohm_vr, 5, 2, buf_ohm_vr);
  // display
  sprintf(str, "R-LED: %sV, %sV %sV, %smA, %dOhm, %sOhm", buf_voltR[0], buf_voltR[1], buf_voltR[2], buf_mA_RGB[0], ohm_RGB[0], buf_ohm_vr);
  Serial.println(str);
  sprintf(str, "G-LED: %sV, %sV, %smA, %dOhm", buf_voltG[0], buf_voltG[1], buf_mA_RGB[1], ohm_RGB[1]);
  Serial.println(str);
  sprintf(str, "B-LED: %sV, %sV, %smA, %dOhm", "4.5", buf_voltB, buf_mA_RGB[2], ohm_RGB[2]);
  Serial.println(str);
}

void lightup(int ratio_RGB[], int time_delay) {
  for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
    // write
    analogWrite(PIN_RGBout[i_rgb], ratio_RGB[i_rgb]);
  }
  // display
  sprintf(str, "ratio of RGB: %d/255 %d/255 %d/255", ratio_RGB[0], ratio_RGB[1], ratio_RGB[2]);
  Serial.println(str);
  // prepare for next action
  delay(time_delay);
  for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
    digitalWrite(PIN_RGBout[i_rgb], LOW);
  }
};