//program for testing the output of the AMOS-IMU module as a function of temperature
//outputs lines of data out serial port (i.e. does not save any data locally)
#include <Wire.h>
#include <LIS3MDL.h>
#include <LSM6.h>

LIS3MDL mag;
LSM6 imu;
LIS3MDL::vector<int16_t> magData;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();

  if (!mag.init())
  {
    Serial.println("Failed to detect and initialize magnetometer!");
    while (1);
  }
  if (!imu.init()) {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  mag.enableDefault();
  imu.enableDefault();
}

void loop() {
  mag.read();
  imu.read();
  Serial.print(mag.m.x);
  Serial.print(", ");
  Serial.print(mag.m.y);
  Serial.print(", ");
  Serial.print(mag.m.z);
  Serial.print(", ");
  Serial.print(mag.m_fTempDegC);
  Serial.print(", ");
  Serial.print(imu.a.x);
  Serial.print(", ");
  Serial.print(imu.a.y);
  Serial.print(", ");
  Serial.print(imu.a.z);
  Serial.print(", ");
  Serial.print(imu.g.x);
  Serial.print(", ");
   Serial.print(imu.g.y);
  Serial.print(", ");
   Serial.print(imu.g.z);
  Serial.print(", ");
  
  Serial.print(imu.m_fTempDegC);
  Serial.println();
  delay(1000);
}
