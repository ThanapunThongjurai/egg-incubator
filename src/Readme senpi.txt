/*This chip uses I2C bus to communicate, specials pins are required to interface
   Board:                                    SDA                    SCL                    Level
   Uno, Mini, Pro, ATmega168, ATmega328..... A4                     A5                     5v
   Mega2560................................. 20                     21                     5v
   Due, SAM3X8E............................. 20                     21                     3.3v
   Leonardo, Micro, ATmega32U4.............. 2                      3                      5v
   Digistump, Trinket, ATtiny85............. 0/physical pin no.5    2/physical pin no.7    5v
   Blue Pill, STM32F103xxxx boards.......... PB7                    PB6                    3.3v/5v
   ESP8266 ESP-01........................... GPIO0/D5               GPIO2/D3               3.3v/5v
   NodeMCU 1.0, WeMos D1 Mini............... GPIO4/D2               GPIO5/D1               3.3v/5v
   ESP32.................................... GPIO21/D21             GPIO22/D22             3.3v*/

/* DEMO - 1, every temperature or humidity call will read 6 bytes over I2C, total 12 bytes */
  //Serial.println(F("DEMO 1: read 12-bytes, show 255 if communication error is occurred"));
  //Serial.print(F("Temperature: ")); Serial.print(myAHT10.readTemperature()); Serial.println(F(" +-0.3C")); //by default "AHT10_FORCE_READ_DATA"
  //Serial.print(F("Humidity...: ")); Serial.print(myAHT10.readHumidity());    Serial.println(F(" +-2%"));   //by default "AHT10_FORCE_READ_DATA"
  /* DEMO - 2, temperature call will read 6 bytes via I2C, humidity will use same 6 bytes */
  //Serial.println(F("DEMO 2: read 6 byte, show 255 if communication error is occurred"));
  //Serial.print(F("Temperature: ")); Serial.print(myAHT10.readTemperature(AHT10_FORCE_READ_DATA)); Serial.println(F(" +-0.3C"));
  //Serial.print(F("Humidity...: ")); Serial.print(myAHT10.readHumidity(AHT10_USE_READ_DATA));      Serial.println(F(" +-2%"));
  /* DEMO - 3, same as demo2 but different call procedure */
  //Serial.println(F("DEMO 3: read 6-bytes, show 255 if communication error is occurred"));
    readStatus = myAHT10.readRawData(); //read 6 bytes from AHT10 over I2C
        if (readStatus != AHT10_ERROR)
        {
            Serial.print(F("Temperature: ")); Serial.print(myAHT10.readTemperature(AHT10_USE_READ_DATA)); Serial.println(F(" +-0.3C"));
            Serial.print(F("Humidity...: ")); Serial.print(myAHT10.readHumidity(AHT10_USE_READ_DATA));    Serial.println(F(" +-2%"));
        }
        else
        {
            Serial.print(F("Failed to read - reset: ")); 
            Serial.println(myAHT10.softReset());         //reset 1-success, 0-failed
        }
  //delay(); //recomended polling frequency 8sec..30sec






//Serial.println("WiFi.status() " + (String)WiFi.status());
//Serial.println("WL_CONNECTED " + (String)WL_CONNECTED);


//*!Serial.println("connected :)" + (String)WiFi.SSID()); //make blynk donw
//TODO:WIFI SSID GET IT TOMAKE US BETTER



  //Wire.setClock(400000); //experimental I2C speed! 400KHz, default 100KHz


//Reconnect(); //เพราะมี blynk อยู่ ใช้  reconnect sensor ด้วย
/*
void Reconnect()
{
  //*ใช้ i2c scanner เช็คเอาก็ได้นะแต่ขี้เกียจละ
  if (readStatus == AHT10_ERROR)
  {
    Serial.print(F("[SENSOR]:-----ATH10 softReset Process: "));
    Serial.println(myAHT10.softReset()); //reset 1-success, 0-failed
    myAHT10.softReset();
    Temperature = 0;
    Humidity = 0;
  }
}
*/
