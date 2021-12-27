#ifndef _ESPFC_DEVICE_GYRO_MPU6500_H_
#define _ESPFC_DEVICE_GYRO_MPU6500_H_

#include "BusDevice.h"
#include "GyroMPU6050.h"
#include "helper_3dmath.h"
#include "Debug.h"

#define MPU6500_USER_CTRL         (0x6A)
#define MPU6500_BIT_RESET         (0x80)
#define MPU6500_SIGNAL_PATH_RESET (0x68)
#define MPU6500_ACCEL_CONF2       (0x1D)
// Register 0x37/55 - INT_PIN_CFG / Pin Bypass Enable Configuration
#define MPU6500_BIT_RESERVED                (1 << 0)
#define MPU6500_BIT_BYPASS_EN               (1 << 1)
#define MPU6500_BIT_INT_ANYRD_2CLEAR        (1 << 4)

// Register 0x38/56 INT_ENABLE
#define MPU6500_BIT_RAW_RDY_EN              (1 << 0)  //enable exti

// Register 0x6a/106 - USER_CTRL / User Control 
#define MPU6500_BIT_I2C_IF_DIS              (1 << 4)  //disable i2c interface 

namespace Espfc {

namespace Device {

class GyroMPU6500: public GyroMPU6050
{
  public:
    int begin(BusDevice * bus) override
    {
      return begin(bus, MPU6050_DEFAULT_ADDRESS);
    }

    int begin(BusDevice * bus, uint8_t addr) override
    {
      setBus(bus, addr);

      if(!testConnection()) return 0;

      _bus->writeByte(_addr, MPU6050_RA_PWR_MGMT_1, MPU6500_BIT_RESET);
      delay(100);

      _bus->writeByte(_addr, MPU6500_SIGNAL_PATH_RESET, 0x07);
      delay(100);

      _bus->writeByte(_addr, MPU6050_RA_PWR_MGMT_1, 0);
      delay(100);
     //set clock to pll 
      setClockSource(MPU6050_CLOCK_PLL_XGYRO);
      setSleepEnabled(false);
      delay(100);      

      setDLPFMode(GYRO_DLPF_188);
      delay(100);

    //set gyro and acc range 
    _bus->writeByte(addr, MPU6050_RA_GYRO_CONFIG, 3 << 3); //INV_FSR_2000DPS
    delay(15);
    _bus->writeByte(addr, MPU6050_RA_ACCEL_CONFIG, 3 << 3);//INV_FSR_16G
    delay(15);
  //todo:
  //set sample rate betaflight gyro  8khz  acc 1khz 
  //set interupt enabled 
      return 1;
    }

    GyroDeviceType getType() const override
    {
      return GYRO_MPU6500;
    }

    void setDLPFMode(uint8_t mode) override
    {
      GyroMPU6050::setDLPFMode(mode);
      _bus->writeByte(_addr, MPU6500_ACCEL_CONF2, mode);
    }


    bool testConnection() override
    {
      uint8_t whoami = 0;
      _bus->readByte(_addr, MPU6050_RA_WHO_AM_I, &whoami);
      //0x70 for MPU6500 SPI
      return whoami == 0x70 ;
    }
};

}

}

#endif
