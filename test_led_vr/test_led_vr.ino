const bool DEBUG = false;
const int ohm_RGB[] = { 100, 100, 100 };
float voltR[3], voltG[2], voltB;
float mA_RGB[3];
float mean_mA_RGB[3];
int ratio_RGB[3] = { 0 };
int i_ratio_input = 0;
float ohm_vr;

// 使うピンの番号
int PIN_Rin[] = { A0, A1, A2 };
int PIN_Gin[] = { A3, A4 };
int PIN_Bin = A5;
int PIN_RGBout[] = { 9, 10, 11 };

// additional LED
int subratio_GB[2] = { 0 };
int subPIN_GBout[2] = { 5, 6 };

// バッファ
char buf_voltR[3][20], buf_voltG[2][20], buf_voltB[20];
char buf_mA_RGB[3][20];
char buf_ohm_vr[20];
char str[80];

// put your setup code here, to run once:
void setup() {
  Serial.begin(9600);
  for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
    pinMode(PIN_RGBout[i_rgb], OUTPUT);
  }

  // 各LEDに電流を流す（5回）
  for (int cnt = 0; cnt < 5; ++cnt) {
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
  }
  // 各LEDにデフォルトで流れる電流を導出（平均値から）
  for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
    mean_mA_RGB[i_rgb] /= 5;
  }

  // display mean current
  for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
    dtostrf(mean_mA_RGB[i_rgb], 5, 2, buf_mA_RGB[i_rgb]);
  }
  sprintf(str, "---mean current of RGB: %smA %smA %smA---", buf_mA_RGB[0], buf_mA_RGB[1], buf_mA_RGB[2]);
  Serial.println(str);

  // 2つ目のG-LEDを光らせる
  for (int i_gb = 0; i_gb < 2; ++i_gb) {
    pinMode(subPIN_GBout[i_gb], OUTPUT);
  }
  subratio_GB[0] = 255 * 15.0 / mean_mA_RGB[1];
  subratio_GB[1] = 0;
  lightup_sub(1000);

  if (!DEBUG) {
    return;
  }
  // R-LED, G-LED, B-LED単体のとき（各々15mAの電流を流す）
  for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
    // calculate
    if (i_rgb == 0) {
      ratio_RGB[i_rgb] = 255 * 16.0 / mean_mA_RGB[i_rgb];
    } else {
      ratio_RGB[i_rgb] = 255 * 15.0 / mean_mA_RGB[i_rgb];
    }
    // lightup
    lightup(2000);
    // reset
    ratio_RGB[i_rgb] = 0;
  }


  // PDを使う場合の最適値（R+G:B=0.25:0.37)
  for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
    // calculate
    switch (i_rgb) {
      case 0:
        ratio_RGB[0] = 255 * 16.0 / mean_mA_RGB[0];
        break;
      case 1:
        ratio_RGB[1] = 255 * 15.0 / mean_mA_RGB[1] * 5.865873551 / 7.0;
        break;
      case 2:
        ratio_RGB[2] = 255 * 15.0 / mean_mA_RGB[2] * (2 * 0.37 / 0.25 * 0.610694143) / 2.5;
        break;
    }
    // lightup
    lightup(2000);
    // reset
    ratio_RGB[i_rgb] = 0;
  }


  // R-LEDとG-LEDを混ぜ合わせたとき
  Serial.println("----R-LED + G-LED----");
  // calculate
  for (int i_ratio = 0; i_ratio < 21; ++i_ratio) {
    sprintf(str, "ratio of R-LED: %d/20", i_ratio);
    Serial.println(str);
    ratio_RGB[0] = 255 * 16.0 / mean_mA_RGB[0] * (float)i_ratio / 20.0;
    ratio_RGB[1] = 255 * 15.0 / mean_mA_RGB[1] * 5.865873551 / 7.0 * (float)(20 - i_ratio) / 20.0;
    // lightup
    lightup(1000);
  }


  // R-LED, G-LED, B-LEDを混ぜ合わせたとき
  Serial.println("----R-LED + G-LED + B-LED----");
  // calculate
  ratio_RGB[2] = 255 * 15.0 / mean_mA_RGB[2] * (1 * 0.37 / 0.25 * 0.610694143) / 2.5;
  for (int i_ratio = 0; i_ratio < 21; ++i_ratio) {
    sprintf(str, "ratio of R-LED: %d/20", i_ratio);
    Serial.println(str);
    ratio_RGB[0] = 255 * 16.0 / mean_mA_RGB[0] * (float)i_ratio / 20.0;
    ratio_RGB[1] = 255 * 15.0 / mean_mA_RGB[1] * 5.865873551 / 7.0 * (float)(20 - i_ratio) / 20.0;
    // lightup
    lightup(1000);
  }
  Serial.println("input something... (+: increase R-LED ratio, -: decrease R-LED ratio, y: Y-LED mode, w: W-LED mode)");
}


void loop() {
  if (Serial.available() > 0) {
    char val;
    bool flg = false;
    val = Serial.read();
    switch (val) {
      // 混色
      case '+':
        i_ratio_input = (i_ratio_input + 1) % 21;
        flg = true;
        break;
      case '-':
        i_ratio_input = (i_ratio_input + 20) % 21;
        flg = true;
        break;
      case 'y':
        ratio_RGB[2] = 0;
        flg = true;
        break;
      case 'w':
        ratio_RGB[2] = 255 * 15.0 / mean_mA_RGB[2] * (1 * 0.37 / 0.25 * 0.610694143) / 2.5;
        flg = true;
        break;
      // 単色光
      case 'r':
        ratio_RGB[0] = 255 * 16.0 / mean_mA_RGB[0];
        ratio_RGB[1] = 0;
        ratio_RGB[2] = 0;
        lightup(100);
        break;
      case 'g':
        ratio_RGB[0] = 0;
        ratio_RGB[1] = 255 * 15.0 / mean_mA_RGB[1];
        ratio_RGB[2] = 0;
        lightup(100);
        break;
      case 'b':
        ratio_RGB[0] = 0;
        ratio_RGB[1] = 0;
        ratio_RGB[2] = 255 * 15.0 / mean_mA_RGB[2];
        lightup(100);
        break;
      //2つ目のLEDへの書き込み用
      case 'G':
        subratio_GB[0] = 255 * 15.0 / mean_mA_RGB[1];
        subratio_GB[1] = 0;
        lightup_sub(100);
        break;
      case 'B':
        subratio_GB[0] = 0;
        subratio_GB[1] = 255 * 15.0 / mean_mA_RGB[2];
        lightup_sub(100);
        break;
      default:
        goto DELAY;
        break;
    }
    if (flg) {
      // calculate
      ratio_RGB[0] = 255 * 16.0 / mean_mA_RGB[0] * (float)i_ratio_input / 20.0;
      ratio_RGB[1] = 255 * 15.0 / mean_mA_RGB[1] * 5.865873551 / 7.0 * (float)(20 - i_ratio_input) / 20.0;
      // display
      sprintf(str, "input char: %c, ratio of R-LED: %d/20", val, i_ratio_input);
      Serial.println(str);
    }
    Serial.println("---input something... (+: increase R-LED ratio, -: decrease R-LED ratio, y: Y-LED mode, w: W-LED mode)---");
  }
DELAY:
  delay(1000);
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

void lightup(int time_delay) {
  // calibrate
  for (int i_rgb = 0; i_rgb < 3; ++i_rgb) {
    if (ratio_RGB[i_rgb] > 255) {
      ratio_RGB[i_rgb] = 255;
    }
    // write
    analogWrite(PIN_RGBout[i_rgb], ratio_RGB[i_rgb]);
  }
  // display
  sprintf(str, "ratio of RGB: %d/255 %d/255 %d/255", ratio_RGB[0], ratio_RGB[1], ratio_RGB[2]);
  Serial.println(str);
  // prepare for next action
  delay(time_delay);
};

void lightup_sub(int time_delay) {
  // calibrate
  for (int i_gb = 0; i_gb < 2; ++i_gb) {
    if (subratio_GB[i_gb] > 255) {
      subratio_GB[i_gb] = 255;
    }
    // write
    analogWrite(subPIN_GBout[i_gb], subratio_GB[i_gb]);
  }
  // display
  sprintf(str, "ratio of additional GB: %d/255 %d/255", subratio_GB[0], subratio_GB[1]);
  Serial.println(str);
  // prepare for next action
  delay(time_delay);
};