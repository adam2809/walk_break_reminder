#include <Wire.h>

/*
Current Usage
=====================================
// 5.37mA with GY-521 LED attached
// 3.96mA with Led Broken/removed, Temp and Gyro still active, high speed parsing
// 0.71mA with Broken LED, Gyro's disabled
// 0.65mA with Broken LED, Gyro's disabled, Temp sensor disabled
*/

/*
Deep Sleep with External Wake Up
=====================================
NOTE:
======
Only RTC IO can be used as a source for external wake
source. They are pins: 0,2,4,12-15,25-27,32-39.
*/

// MPU registers
#define SIGNAL_PATH_RESET  0x68
#define I2C_SLV0_ADDR      0x37
#define ACCEL_CONFIG       0x1C
#define MOT_THR            0x1F  // Motion detection threshold bits [7:0]
#define MOT_DUR            0x20  // This seems wrong // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define MOT_DETECT_CTRL    0x69
#define INT_ENABLE         0x38
#define PWR_MGMT           0x6B //SLEEPY TIME
#define INT_STATUS 0x3A
#define MPU6050_ADDRESS 0x68 //AD0 is 0

void configureMPU(int sens);
void writeByte(uint8_t address, uint8_t subAddress, uint8_t data);
