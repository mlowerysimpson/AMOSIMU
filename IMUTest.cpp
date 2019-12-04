#include "../RemoteControlTest/IMU.h"
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
	  printf("An error occurred trying to initialized the IMU.\n");
	  return -1;
  }
  if (isMagCalFlagPresent(argc,argv)) {
    if (!imu.DoMagCal()) {
      printf("Error doing magnetometer calibration.\n");
      return -2;
    }
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
