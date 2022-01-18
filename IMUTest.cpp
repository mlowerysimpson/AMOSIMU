#define IMUTEST_BUILD 1

#include "../RemoteControlTest/IMU.h"
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <memory>


//example program that tests out the operation of the AltIMU-10 v5 Gyro, Accelerometer, Compass, and Altimeter from Pololu Electronics (www.pololu.com)

/**
 * @brief return true if a magnetometer calibration flag (-magcal) was specified in the program arguments
 * 
 * @param argc the number of program arguments
 * @param argv an array of character pointers that corresponds to the program arguments
 * @return true if a magnetometer calibration flag (-magcal) is present in the array of program arguments
 * @return false if no magnetometer calibration flag is present in the array of program arguments.
 */
bool isMagCalFlagPresent(int argc, char * argv[]) {
	for (int i=0;i<argc;i++) {
		if (strlen(argv[i])<7) continue;
		if (strncmp(argv[i],"-magcal",7)==0) {
			return true;
		}
	}
	return false;
}

/**
 * @brief return true if a factory XY magnetometer calibration flag (-fmxy) was specified in the program arguments
 *
 * @param argc the number of program arguments
 * @param argv an array of character pointers that corresponds to the program arguments
 * @return true if a factory XY magnetometer calibration flag (-fmxy) is present in the array of program arguments
 * @return false if no factory XY magnetometer calibration flag is present in the array of program arguments.
 */
bool isFactoryMagCalXYFlagPresent(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strlen(argv[i]) < 5) continue;
        if (strncmp(argv[i], "-fmxy", 5) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief return true if a factory XZ magnetometer calibration flag (-fmxz) was specified in the program arguments
 *
 * @param argc the number of program arguments
 * @param argv an array of character pointers that corresponds to the program arguments
 * @return true if a factory XZ magnetometer calibration flag (-fmxz) is present in the array of program arguments
 * @return false if no factory XZ magnetometer calibration flag is present in the array of program arguments.
 */
bool isFactoryMagCalXZFlagPresent(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strlen(argv[i]) < 5) continue;
        if (strncmp(argv[i], "-fmxz", 5) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief return true if a factory temperature calibration flag (-ftempcal) was specified in the program arguments
 *
 * @param argc the number of program arguments
 * @param argv an array of character pointers that corresponds to the program arguments
 * @return true if a factory temperature calibration flag (-ftempcal) is present in the array of program arguments
 * @return false if no factory temperature calibration flag is present in the array of program arguments.
 */
bool isTempCalFlagPresent(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strlen(argv[i]) < 9) continue;
        if (strncmp(argv[i], "-ftempcal", 9) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief return true if a -h or -H help flag was specified in the program arguments
 *
 * @param argc the number of program arguments
 * @param argv an array of character pointers that corresponds to the program arguments
 * @return true if a help flag (-h or -H) is present in the array of program arguments
 * @return false if no help flag is present in the array of program arguments.
 */
bool isHelpFlagPresent(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strlen(argv[i]) < 2) continue;
        if (strncmp(argv[i], "-h", 2) == 0) {
            return true;
        }
        else if (strncmp(argv[i], "-H", 2) == 0) {
            return true;
        }
    }
    return false;
}

void ShowIMUTestUsage() {
    printf("IMUTest\n");
    printf("Usage: IMUTest [-h] [-magcal] [-fmxy] [-fmxz] [-ftempcal]\n");
    printf("If no arguements are specified, the program collects and prints out data from the IMU for about 5 seconds.\n");
    printf("Optional flags:\n");
    printf("-h: prints out this help message.\n");
    printf("-magcal: does a calibration of the X and Y magnetometers by assuming that the device is rotated slowly in a complete circle.\n");
    printf("-fmxy: does a factory calibration of the X and Y magnetometers (similar to that done with the -magcal flag) except that it requires the user to toggle calibration data sampling on and off with the press of a button.\n");
    printf("-fmxz: does a factory calibration of the X and Z magnetometers (requires IMU device to be aligned on edge with Y-axis pointed up or down) and requires the user to toggle calibration data sampling on and off with the press of a button.\n");
    printf("-ftempcal: does a factory temperature calibration.\n");
}


/**
 * @brief the main function for the IMUTest program
 * 
 * @param argc the number of program arguments specified on the command line to the program
 * @param argv a character array corresponding to each of the program arguments
 * @return int the return code from the IMUTest program
 */
int main(int argc, char * argv[])
{
  const int NUM_SAMPLES = 100;
  const int NUM_TO_AVG = 1;//number of individual samples to average for each call to IMU::GetSample
  pthread_mutex_t i2cMutex = PTHREAD_MUTEX_INITIALIZER;;//mutex for controlling access to i2c bus
  IMU imu(&i2cMutex);
  if (imu.m_bInitError) {
	  printf("An error occurred trying to initialize the IMU.\n");
	  return -1;
  }
  if (isMagCalFlagPresent(argc,argv)) {
    if (!imu.DoXYMagCal()) {
      printf("Error doing magnetometer calibration.\n");
      return -2;
    }
    return 0;
  }
  else if (isHelpFlagPresent(argc, argv)) {
      ShowIMUTestUsage();
      return 0;
  }
  else if (isFactoryMagCalXYFlagPresent(argc, argv)) {
      wiringPiSetupGpio();//need to call this before doing anything with GPIO
      if (!imu.DoXYMagCalWithToggledSampling()) {
          printf("Error doing factory magnetometer XY calibration.\n");
          return -4;
      }
      return 0;
  }
  else if (isFactoryMagCalXZFlagPresent(argc, argv)) {
      wiringPiSetupGpio();//need to call this before doing anything with GPIO
      if (!imu.DoXZMagCalWithToggledSampling()) {
          printf("Error doing factory magnetometer XZ calibration.\n");
          return -5;
      }
      return 0;
  }
  else if (isTempCalFlagPresent(argc, argv)) {
      wiringPiSetupGpio();//need to call this before doing anything with GPIO
      if (!imu.DoTempCal()) {
          printf("Error doing factory temperature calibration.\n");
          return -6;
      }
      return 0;
  }
  IMU_DATASAMPLE imu_sample;
  for (int i=0;i<NUM_SAMPLES;i++) {
		if (!imu.GetMagSample(&imu_sample, NUM_TO_AVG)) {//collect raw magnetometer data from the LIS3MDL 3-axis magnetometer device and process it to get the magnetic vector and temperature
			printf("Error getting magnetometer sample #%d.\n",i+1);
			return -2;
		}
    if (!imu.GetAccGyroSample(&imu_sample, NUM_TO_AVG)) {//collect accelerometer, gyro, and temperature data from the LSM6DS33 3-axis acc/gyro device
      printf("Error getting acc/gyro sample #%d.\n",i+1);
      return -3;
    }
    //sample #, accX, accY, accZ, gyroX, gyroY, gyroZ, temperature
    //un-comment the following lines to show mag, acc, and gyro data
     printf("%d (%.3f sec): magX = %.6f, magY = %.6f, magZ = %.6f, accX = %.6f, accY = %.6f, accZ = %.6f, gyroX = %.3f, gyroY = %.3f, gyroZ = %.3f, temperature = %.1f deg C\n",
       i+1,imu_sample.sample_time_sec,imu_sample.mag_data[0],imu_sample.mag_data[1],imu_sample.mag_data[2],
       imu_sample.acc_data[0],imu_sample.acc_data[1],imu_sample.acc_data[2],
       imu_sample.angular_rate[0],imu_sample.angular_rate[1],imu_sample.angular_rate[2],imu_sample.acc_gyro_temperature);
        
    imu.ComputeOrientation(&imu_sample);
    printf("%d (%.3f sec): roll = %.1f deg, pitch = %.1f deg, heading = %.1f deg\n",i+1,imu_sample.sample_time_sec,imu_sample.roll,imu_sample.pitch,imu_sample.heading);
  }
  return 0 ;
}
