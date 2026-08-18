#ifndef B13DOF_H
#define B13DOF_H

#include <BMI088.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "DFRobot_BMM150.h"

struct __attribute__((__packed__)) environment {
//BMI088
    float ax, ay, az;//Accel
    float gx, gy, gz;//Gyro
    float tempI;//internal temperature
    uint64_t time; //Time since begin 

//BME680
    float tempO;
    float humidity;
    float pressure;
    uint32_t Gas;

// BMM150
    int16_t mx, my, mz;//Magnetic field vectors
};
class B13DOF
{
    public:
        B13DOF();

        bool begin();
        bool update(int i);
        
    //Getter function 
        const environment& getData() const; 
    private:

        void config();
        bool BMI088();
        bool BME680();
        bool BMM150();

    //BMI088
        Bmi088Accel acel;
        Bmi088Gyro gyro;
    //BME680
        Adafruit_BME680 bme;
    
    //BMM150
        DFRobot_BMM150_I2C bmm;
    
    //environmental data
    environment data;
};
#endif
