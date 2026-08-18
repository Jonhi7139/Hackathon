#include "B13DOF.h"

B13DOF::B13DOF():

//Address
    acel(Wire,0x18),
    gyro(Wire,0x68),
    bme(&Wire), 
    bmm(&Wire, 0x10),
    data{}
{}
/*
bool B13DOF::begin()
{
    Wire.begin();
    if (acel.begin() < 0|| gyro.begin() < 0||!bme.begin(0x76)||!bmm.begin())
    {
        return false;
    }
    config();
    return true;
}
*/
bool B13DOF::begin()
{
    Wire.begin();
    if(bmm.begin())
    {              
    Serial.println("BMM150 failed");
        return false;
    }

    if(!bme.begin(0x76))
    {
        Serial.println("BME680 failed");
        return false;
    }
    if(acel.begin()<0)
    {
        Serial.println("BMI088 Accelerometer failed");
        return false;
    }
    if(gyro.begin()<0)
    {
        Serial.println("BMI088 Gyroscope failed");
        return false;
    }

    config();
    return true;
}

void B13DOF::config()
{
//BME680
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
        /*
        BME68X_OS_16X   
        BME68X_OS_8X   
        BME68X_OS_4X  
        BME68X_OS_2X
        BME68X_OS_1X
        BME68X_OS_NONE                 
        */

    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        /*
        BME68X_FILTER_SIZE_127 
        BME68X_FILTER_SIZE_63   
        BME68X_FILTER_SIZE_31 
        BME68X_FILTER_SIZE_15 
        BME68X_FILTER_SIZE_7
        BME68X_FILTER_SIZE_3 
        BME68X_FILTER_SIZE_1
        BME68X_FILTER_OFF 
        */

    bme.setGasHeater(320, 150); // heats up the sensor to 320°C for 150 ms (any value that fits uint16_t  can be chosen)

//BMM150
    bmm.setOperationMode(BMM150_POWERMODE_NORMAL);
        /* 
        BMM150_POWERMODE_NORMAL  // normal mode  Get geomagnetic data normally
        BMM150_POWERMODE_FORCED  // forced mode  Single measurement, the sensor restores to sleep mode when the measurement is done.
        BMM150_POWERMODE_SLEEP   // sleep mode   Users can visit all the registers, but can't measure geomagnetic data
        BMM150_POWERMODE_SUSPEND // suspend mode At the time the sensor cpu doesn't work and can't implement any operation.
        */

    bmm.setPresetMode(BMM150_PRESETMODE_HIGHACCURACY);
        /*
        BMM150_PRESETMODE_LOWPOWER      // Low power mode, get a small number of data and take the mean value.
        BMM150_PRESETMODE_REGULAR       // Regular mode, get a number of data and take the mean value.
        BMM150_PRESETMODE_ENHANCED      // Enhanced mode, get a large number of data and take the mean value.
        BMM150_PRESETMODE_HIGHACCURACY  // High accuracy mode, get a huge number of data and take the mean value.
        */

    bmm.setRate(BMM150_DATA_RATE_10HZ);
        /*  
        BMM150_DATA_RATE_02HZ
        BMM150_DATA_RATE_06HZ
        BMM150_DATA_RATE_08HZ
        BMM150_DATA_RATE_10HZ
        BMM150_DATA_RATE_15HZ
        BMM150_DATA_RATE_20HZ
        BMM150_DATA_RATE_25HZ
        BMM150_DATA_RATE_30HZ
        */

       bmm.setMeasurementXYZ();   //needed in set up  to measure the values

//BMI088
    acel.setOdr(Bmi088Accel::ODR_100HZ_BW_40HZ);
        /*
        ODR_1600HZ_BW_280HZ,
        ODR_1600HZ_BW_234HZ,
        ODR_1600HZ_BW_145HZ,
        ODR_800HZ_BW_230HZ,
        ODR_800HZ_BW_140HZ,
        ODR_800HZ_BW_80HZ,
        ODR_400HZ_BW_145HZ,
        ODR_400HZ_BW_75HZ,
        ODR_400HZ_BW_40HZ,
        ODR_200HZ_BW_80HZ,
        ODR_200HZ_BW_38HZ,
        ODR_200HZ_BW_20HZ,
        ODR_100HZ_BW_40HZ,
        ODR_100HZ_BW_19HZ,
        ODR_100HZ_BW_10HZ,
        ODR_50HZ_BW_20HZ,
        ODR_50HZ_BW_9HZ,
        ODR_50HZ_BW_5HZ,
        ODR_25HZ_BW_10HZ,
        ODR_25HZ_BW_5HZ,
        ODR_25HZ_BW_3HZ,
        ODR_12_5HZ_BW_5HZ,
        ODR_12_5HZ_BW_2HZ,
        ODR_12_5HZ_BW_1HZ
        */

    acel.setRange(Bmi088Accel::RANGE_3G);
        /*
        RANGE_3G 
        RANGE_6G 
        RANGE_12G 
        RANGE_24G 
        */

    gyro.setOdr(Bmi088Gyro:: ODR_100HZ_BW_32HZ);
        /*
        ODR_2000HZ_BW_532HZ = 0x80,
        ODR_2000HZ_BW_230HZ = 0x81,
        ODR_1000HZ_BW_116HZ = 0x82,
        ODR_400HZ_BW_47HZ = 0x83,
        ODR_200HZ_BW_23HZ = 0x84,
        ODR_100HZ_BW_12HZ = 0x85,
        ODR_200HZ_BW_64HZ = 0x86,
        ODR_100HZ_BW_32HZ = 0x87
        */

    gyro.setRange(Bmi088Gyro::RANGE_500DPS);
        /*
        RANGE_2000DPS
        RANGE_1000DPS 
        RANGE_500DPS 
        RANGE_250DPS 
        RANGE_125DPS 
        */
}
bool B13DOF::update(int i)
{
    switch(i)
    {
        case 0:// BMI088
            if(BMI088())
            {
                return true;
                break;
            }
            else
            {
                return false;
                break;
            }
        case 1: // BME680
            if(BME680())
            {
                return true;
                break;
            }
            else
            {
                return false;
                break;
            }
        case 2: // BMM150
            if(BMM150())
            {
                return true;
                break;
            }
            else
            {
                return false;
                break;
            }
        case 3://BMI088 and BME680
            if(BMI088()&&BME680())
            {
                return true;
                break;
            }
            else
            {
                return false;
                break;
            }
        case 4://BMI088 and BMM150
            if(BMI088()&&BMM150())
            {
                return true;
                break;
            }
            else
            {
                return false;
                break;
            }
        case 5://BME680 and BMM150
            if(BME680()&&BMM150())
            {
                return true;
                break;
            }
            else
            {
                return false;
                break;
            }
        case 6://All sensors
            if(BMI088()&&BME680()&&BMM150())
            {
                return true;
                break;
            }
            else
            {
                return false;
                break;
            }
        default:
            Serial.println("Invalid sensor selection");
            return false;
            break;
    }  
}
//Getter
const environment& B13DOF::getData() const
{
    return data;
}

bool B13DOF:: BMI088()
{
    //BMI088
    acel.readSensor();
    uint64_t newTime = acel.getTime_ps();

    if(data.time!=newTime)
    {
    data.ax=acel.getAccelX_mss();
    data.ay=acel.getAccelY_mss();
    data.az=-1*acel.getAccelZ_mss();
    data.tempI=acel.getTemperature_C();
    data.time= newTime;

    gyro.readSensor();
    data.gx=gyro.getGyroX_rads();
    data.gy=gyro.getGyroY_rads();
    data.gz=gyro.getGyroZ_rads();
    return true;
    }
    else
    {
        Serial.println("BMI088 COULD NOT UPDATE");
        return false;
    }
}

bool B13DOF:: BME680()
{
    if(bme.performReading())
    {
        data.tempO=bme.temperature;
        data.humidity=bme.humidity;
        data.pressure=(bme.pressure/100);   //Pressure in hPa ;
        data.Gas=(bme.gas_resistance/1000); //Gas resistance in KOhms
        return true;
    }
    else
    {
        Serial.println("BME680 COULD NOT UPDATE");
        return false;
    }
}

bool B13DOF:: BMM150()
{
    sBmm150MagData_t magData = bmm.getGeomagneticData();//Measure the data in uT
    if(magData.x==0&&magData.y==0&&magData.z==0)
    {
        Serial.println("BMM150 COULD NOT UPDATE");
        return false;
    }
    else
    {
        data.mx=magData.x;
        data.my=magData.y;
        data.mz=magData.z;
        return true;
    }
}
