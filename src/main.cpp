#include <Arduino.h>
#include <ESP32Servo.h>

const int servoPin1 = 13; // Servo ปกติ
const int servoPin2 = 14; // Servo หมุนกลับด้าน
const int moveTime = 1000;
Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(115200);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  servo1.write(0);
  servo2.write(180); // เริ่มตัวกลับด้านที่ 180

  Serial.println("Enter angle (0-180):");
}

void loop() {
  if (Serial.available() > 0) {
    int angle = Serial.parseInt();
    if (angle >= 0 && angle <= 180) {
      int totalTime = moveTime; // เวลา 5 วินาทีรวมไป-กลับ
      int steps = 100;

      float stepAngle = angle / (steps / 2.0);
      int stepDelay = totalTime / steps;

      float currentAngle1 = 0;        // Servo 13
      float currentAngle2 = 180;      // Servo 14

      // หมุนไปมุมที่ป้อน
      for (int i = 0; i < steps / 2; i++) {
        currentAngle1 += stepAngle;
        currentAngle2 -= stepAngle;
        if (currentAngle1 > angle) currentAngle1 = angle;
        if (currentAngle2 < 180 - angle) currentAngle2 = 180 - angle;
        servo1.write(currentAngle1);
        servo2.write(currentAngle2);
        delay(stepDelay);
      }

      // หมุนกลับไปตำแหน่งเริ่มต้น
      for (int i = 0; i < steps / 2; i++) {
        currentAngle1 -= stepAngle;
        currentAngle2 += stepAngle;
        if (currentAngle1 < 0) currentAngle1 = 0;
        if (currentAngle2 > 180) currentAngle2 = 180;
        servo1.write(currentAngle1);
        servo2.write(currentAngle2);
        delay(stepDelay);
      }

      Serial.print("Completed movement for angle: ");
      Serial.println(angle);

    } else {
      Serial.println("Invalid angle! Enter 0-180.");
    }
    Serial.flush();
  }
}