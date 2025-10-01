#include <Wire.h>

int SensorPins[] = {15, 14, 29, 28, 27, 26, 24, 23, 22, 5, 6, 2, 17, 7, 16, 9, 10, 11, 12, 35, 34, 32, 31, 30};
int rersensorpins[] = {40, 41, 7, 6, 5, 4, 2, 1, 0, 27, 35, 28, 32, 36, 33, 38, 12, 13, 14, 18, 19, 21, 22, 23};
unsigned long readEE[200] = {0};
unsigned long raw_valueABC[200] = {0};
unsigned long raw_valueEHJ[200] = {0};

short ir, rast = 0, qrast = 0, max_ = 0;
double cos_side, sin_side;
float angle = 0;
int data[2];
unsigned long pinc = 0, pinj = 0, one = 1;

float _err_measure = 3;  // примерный шум измерений
float _q = 0.2;   // скорость изменения значений 0.001-1, варьировать самому

int b = 12, max_i  = 0;
float a = 0;
float simpleKalman(float newVal) {
  float _kalman_gain, _current_estimate;
  static float _err_estimate = _err_measure;
  static float _last_estimate;
  _kalman_gain = (float)_err_estimate / (_err_estimate + _err_measure);
  _current_estimate = _last_estimate + (float)_kalman_gain * (newVal - _last_estimate);
  _err_estimate =  (1.0 - _kalman_gain) * _err_estimate + fabs(_last_estimate - _current_estimate) * _q;
  _last_estimate = _current_estimate;
  return _current_estimate;
}


void sendData() {
  Wire.write((byte*) &data, 2 * sizeof(int));

}

void setup() {
  Serial.begin(115200);
  Wire.begin(0x09);
  Wire.onRequest(sendData);
  for (int i = 0; i < 40; ++i) {
    pinMode(i, INPUT);
  }
  for (int i = 0; i < 24; i++) {
    readEE[0] = 0;
  }

}

void loop() {
  cos_side = 0, sin_side = 0, rast = 0, max_ =  0;;

  ir = 0;
  //Serial.print("---");
  unsigned long startTime_us = micros() ;
  pinc = PINC;
  pinj = PINJ;
  raw_valueABC [ir] = PINA + (PINB << 8) + (pinc << 16);
  raw_valueEHJ [ir] = PINE + (PINH << 8) + (PINJ << 16);
  //  Serial.print((micros() - startTime_us));
  //  Serial.print("    ");
  startTime_us = micros() ;

  while ((micros() - startTime_us) < 833) {
    pinc = PINC;
    pinj = PINJ;
    raw_valueABC [ir] = PINA + (PINB << 8) + (pinc << 16);
    raw_valueEHJ [ir] = PINE + (PINH << 8) + (pinj << 16);

    ir += 1;
  }
  //
  //  Serial.print(ir);
  //  Serial.print("   ");

  for (int i = 0; i < 24; i++) {
    int shift = rersensorpins[i];
    if (shift < 24) {
      for (int j = 0; j < ir; j++) {
        readEE[i] += ((raw_valueABC[j] & (one << shift)) == 0);
      }
    } else {
      shift -= 24;
      for (int j = 0; j < ir; j++) {
        readEE[i] += ((raw_valueEHJ[j] & (one << shift)) == 0);
      }
    }
  }
  int rett = 0;
  for (int i = 0; i < 24; i++) {
    rett += (readEE[i] > 0);
    cos_side += (cos(i * 15.0 * 3.1415926 / 180.0) * readEE[i]  * 100 );
    sin_side += (sin(i * 15.0 * 3.1415926 / 180.0) * readEE[i] * 100 );
    if (readEE[i] > max_) {
      max_ = max(max_, readEE[i]);
      max_i = i;
    }
  }
  Serial.print(rett);
  Serial.print("   ");
  angle = simpleKalman((atan2(sin_side, cos_side) * 180 / 3.1415));
  Serial.print(angle);
  Serial.print("   ");

  //  Serial.print(max_);
  //  Serial.print("   ");
  a = a + 0.03 * (max_ - min(readEE[max_i + 1], readEE[(max_i + 23)%24 ]) - a);
  Serial.print(a);
  Serial.print("   ");

  for (int i = 0; i < 24; i++) {
        Serial.print(readEE[i]);
        Serial.print(" ");
    readEE[i] = 0;
  }
  for (int i = 0; i < 200; i++) {

    raw_valueABC [i] = 0;
    raw_valueEHJ [i] = 0;
  }
  if (rett  == 0) {
    data[0] = 999;
    data[1] = -1;
    } else {
    data[0] = angle;
    data[1] = a * 100;
  }

  Serial.println("");
}
