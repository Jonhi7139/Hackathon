  #include <Arduino.h>
  #include "B13DOF.h"
  #include "Serializer.h"

  B13DOF sensor;
  Serializer send;

  void dBMM150(const environment &x);
  void dBMI088Accel(const environment &x);
  void dBMI088Gyro(const environment &x);
  void dBME680(const environment &x);

  void setup() {
    Serial.begin(9600);
    while(!Serial);
    bool success = sensor.begin();
    while(!success)
    {
      Serial.println("Not possible to connect");
      delay(1000);
      success = sensor.begin();
    }
  }

  void loop() {
    

    if(sensor.update(6))//Goes from 0 to 6 (Check B13DOF.cpp for more information)
    {
      const auto& data = sensor.getData();
      /*
      dBMM150(data);
      dBMI088Accel(data);
      dBMI088Gyro(data);
      dBME680(data);
      */
      Serial.write("##");
      send.serialSend(data);  
    }
  }
  void dBMM150(const environment &x)
  {
      Serial.println("BMM150 DATA:");
      Serial.print("mag x = "); Serial.print(x.mx); Serial.println(" uT");
      Serial.print("mag y = "); Serial.print(x.my); Serial.println(" uT");
      Serial.print("mag z = "); Serial.print(x.mz); Serial.println(" uT");
      Serial.println();
  }

  void dBMI088Accel(const environment &x)
  {
    Serial.println("Accelerometer DATA:");
    Serial.print("Acceleration in x is: "); Serial.print(x.ax);  Serial.println("m/s^2");
    Serial.print("Acceleration in y is: "); Serial.print(x.ay);  Serial.println("m/s^2");
    Serial.print("Acceleration in z is: "); Serial.print(x.az);  Serial.println("m/s^2");
    Serial.println();

    Serial.print("Internal temperature: ");Serial.print(x.tempI); Serial.println("°C");
    Serial.print("Time measured by the sensor: "); Serial.print(x.time); Serial.println("ps");
    Serial.println();
  }

  void dBMI088Gyro(const environment &x)
  {
    Serial.println("Gyroscope DATA:");

    Serial.print("Angular velocity in x: "); Serial.print(x.gx); Serial.println("rad/s");
    Serial.print("Angular velocity in y: "); Serial.print(x.gy); Serial.println("rad/s");
    Serial.print("Angular velocity in z: "); Serial.print(x.gz); Serial.println("rad/s");
    Serial.println();
  }

  void dBME680(const environment &x)
  {
    Serial.println("BME680 DATA:");
    Serial.print("Temperatura = "); Serial.print(x.tempO); Serial.println(" C");

    Serial.print("Humidade = "); Serial.print(x.humidity); Serial.println("%");

    Serial.print("Pressão = "); Serial.print(x.pressure); Serial.println(" hPa");

    Serial.print("Gas resistance  = "); Serial.print(x.Gas); Serial.println(" KOhms");

    Serial.println();
  }


