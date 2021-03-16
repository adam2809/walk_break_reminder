#include "motion_detection.hpp"

void writeByte(uint8_t address, uint8_t subAddress, uint8_t data){
	Wire.begin();
	Wire.beginTransmission(address);  // Initialize the Tx buffer
	Wire.write(subAddress);           // Put slave register address in Tx buffer
	Wire.write(data);                 // Put data in Tx buffer
	Wire.endTransmission();           // Send the Tx buffer
}

// sens argument configures wake up sensitivity
void configureMPU(int sens){
	writeByte( MPU6050_ADDRESS, 0x6B, 0x00);
	writeByte( MPU6050_ADDRESS, SIGNAL_PATH_RESET, 0x07);//Reset all internal signal paths in the MPU-6050 by writing 0x07 to register 0x68;
	writeByte( MPU6050_ADDRESS, ACCEL_CONFIG, 0x01);//Write register 28 (==0x1C) to set the Digital High Pass Filter, bits 3:0. For example set it to 0x01 for 5Hz. (These 3 bits are grey in the data sheet, but they are used! Leaving them 0 means the filter always outputs 0.)
	writeByte( MPU6050_ADDRESS, MOT_THR, sens);  //Write the desired Motion threshold to register 0x1F (For example, write decimal 20).
	writeByte( MPU6050_ADDRESS, MOT_DUR, 40 );  //Set motion detect duration to 1  ms; LSB is 1 ms @ 1 kHz rate
	writeByte( MPU6050_ADDRESS, MOT_DETECT_CTRL, 0x15); //to register 0x69, write the motion detection decrement and a few other settings (for example write 0x15 to set both free-fall and motion decrements to 1 and accelerometer start-up delay to 5ms total by adding 1ms. )
	writeByte( MPU6050_ADDRESS, 0x37, 140 ); // now INT pin is active low
	writeByte( MPU6050_ADDRESS, INT_ENABLE, 0x40 ); //write register 0x38, bit 6 (0x40), to enable motion detection interrupt.
	writeByte( MPU6050_ADDRESS, PWR_MGMT, 8 ); // 101000 - Cycle & disable TEMP SENSOR
	writeByte( MPU6050_ADDRESS, 0x6C, 7); // Disable Gyros
}
