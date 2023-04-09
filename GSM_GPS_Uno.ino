/* This code works with SIM800L Evb version, DHT11 and MLX90614
 * It sets the module on receiving SMS mode, wait if the user has sent a SMS containing keywords "DHT" or "MLX"
 * Check which one is received then proceed to get data from the chosen sensor and send it via SMS to the programmed phone number
 * And come back to receiving mode.
 * Refer to www.SurtrTech.com for more detaims
 */

#include <SoftwareSerial.h>        //Libraries required for Serial communication, ic communication, DHT11 and MLX90614
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
 
Adafruit_MPU6050 mpu;
float value = 5.5;   
String Data_SMS;               //Here's the SMS that we gonna send to the phone number, it may contain DHT data or MLX data


SoftwareSerial sim800l(2, 3);                     // RX,TX for Arduino and for the module it's TXD RXD, they should be inverted


void setup()
{
   
  sim800l.begin(9600);   //Begin all the communications needed Arduino with PC serial and Arduino with all devices (SIM800L+DHT+MLX)
  Serial.begin(9600);     
 
  Serial.println("Starting ...");
  delay(3000);         //Delay to let the module connect to network, can be removed

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(100);

  sim800l.println("AT"); //Once the handshake test is successful, it will back to OK
  Serialcom();
  sim800l.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  Serialcom();
  sim800l.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  Serialcom();
  sim800l.println("AT+CREG?"); //Check whether it has registered in the network
  Serialcom();
  
}



void loop() {
  
 /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.println(a.acceleration.z);
  
   
  if(a.acceleration.y > (value) ){                   //Same thing if the "MLX" word is found, Serial.print things are optionnal
  
  Data_SMS = "Crash Detected at location: 27.2046° N, 77.4977° E";

  Send_Data();

  
}
delay(1000);
     

}


void Serialcom() //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
{
  delay(500);
  while(Serial.available())                                                                      
  {
    sim800l.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(sim800l.available())                                                                      
  {
    Serial.write(sim800l.read());//Forward what Software Serial received to Serial Port
  }
}

void Send_Data()
{
  Serial.println("Sending Data...");     //Displays on the serial monitor...Optional
  sim800l.print("AT+CMGF=1\r");          //Set the module to SMS mode
  delay(100);
  sim800l.print("AT+CMGS=\"+233558107905\"\r");  //Your phone number don't forget to include your country code example +212xxxxxxxxx"
  delay(500);  
  sim800l.print(Data_SMS);  //This string is sent as SMS
  delay(500);
  sim800l.print((char)26);//Required to tell the module that it can send the SMS
  delay(500);
  sim800l.println();
  Serial.println("Data Sent.");
  delay(500);

}
