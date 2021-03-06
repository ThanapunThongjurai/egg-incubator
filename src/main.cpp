/*
    ** จะไม่มีการ ใช้ ESP32.reset เด็ดขาดทุกอย่างต้องทำงานแยกกัน
    ** no  delay
    TODO : add motor controll for egg 
*/
//*! LibaryLoader
#include <Arduino.h>
#include <elapsedMillis.h>
#include <Wire.h>
#include <AHT10.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
//#include <EEPROM.h>
//*wifi manger
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>

//*! all
//*GPIO_SETTING
#define Relay1 16
#define Relay2 17
#define LedBulidIn 2
//*GOBAL_VAR
float Temperature = 0;
float Humidity = 0;
String relayStatus1 = "OFF";
String relayStatus2 = "OFF";


//*!ObjectSetup
//*Teampreture
uint8_t readStatus = 0;
AHT10 myAHT10(AHT10_ADDRESS_0X38);
//*TIMER
elapsedMillis timeElapsed;
unsigned int interval = 10 * 1000; //secend *1000(is millisec)
elapsedMillis timeElapsedBlynk;
unsigned int intervalBlynk = 5 * 1000; //secend *1000(is millisec)
elapsedMillis timeElapsedBlynkNotiReset;
unsigned int intervalBlynkNotiReset = 10 * 60 * 1000; //secend *1000(is millisec)
//*OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//*BLYNK
char auth[] = "uQE9tc0pxF3kvNageuLAk9ifvoVbElpi";
char ssid[] = "SSID";
char pass[] = "";
char serv[] = "blynk.ecp.in.th";

/////*EEPROM
//ต้องใช้ code รี eeprom
//#define EEPROM_SIZE 3 
//1.start กี่ครั้ง
//2.reconnect กี่ครั้ง 



//*! function
/*
int startCount = 0;
int reconnectCount = 0;
void startCountEEPROM()
{
  startCount = EEPROM.read(0);
  startCount++;
  EEPROM.write(0, startCount);
  EEPROM.commit();
}
void reconnectCountEEPROM()
{
  reconnectCount = EEPROM.read(1);
  reconnectCount++;
  EEPROM.write(1, reconnectCount);
  EEPROM.commit();
}
*/
void C2W()
{
  //*AHT10
  if (myAHT10.begin() != true)
  {
    Serial.println(F("\n[SENSOR]:-----AHT10 FAIL-----")); //(F()) save string to flash & keeps dynamic memory free
  }
  else
  {
    Serial.println(F("\n[SENSOR]:-----AHT10 OK-----"));
  }
  //*OLED
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3c) != false)
  {
    Serial.println(F("[SENSOR]:-----SSD1306 FAIL-----"));
  }
  else
  {
    Serial.println(F("[SENSOR]:-----OLED OK-----"));
  }
  display.display();
  display.clearDisplay();
  //*BLYNK
  Blynk.config(auth, serv, 8080);
  Serial.println(F("[APP]:-----BLYNK OK-----"));
}

//แสดงผล
void OLED()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.print("T ");
  display.print(Temperature);
  display.println("C");
  display.print("H ");
  display.print(Humidity);
  display.println("RH%");
  display.display();
}

void data2comport()
{
  Serial.print(F("Temperature\t: "));
  Serial.print(Temperature);
  Serial.println(F(" +-0.3C"));
  Serial.print(F("Humidity...\t: "));
  Serial.print(Humidity);
  Serial.println(F(" +-2%"));
}
//blynk noti
int notiSwitch = 0;
int notiSwitchReset = 0;
int checkResetNoti = 0;
int checkNotiSwitch = 0;
bool notiStatus = 0;
void blynkRead()
{
  Blynk.virtualWrite(V0, Temperature);
  Blynk.virtualWrite(V1, Humidity);
  //Blynk.virtualWrite(V20, startCount);
  //Blynk.virtualWrite(V21, reconnectCount);
  Blynk.virtualWrite(V3, relayStatus1);
  Blynk.virtualWrite(V4, relayStatus2);
  Blynk.virtualWrite(V32, checkNotiSwitch);
  Blynk.virtualWrite(V33, checkResetNoti);
}
BLYNK_WRITE(V30) // V30 เปิดปิดแจ้งเตือน
{
  notiSwitch = param.asInt();
}
BLYNK_WRITE(V31) // V31 เอาไว้ reset swith แจ้งเตือน
{
  notiSwitchReset = param.asInt();
}
void noti(String add)
{
  if (Temperature <= 35 && timeElapsedBlynk > intervalBlynk)
  {
    timeElapsedBlynk = 0;
    Blynk.notify("อุณหภูมิต่ำกว่าที่ควร 35องศา " + add);
  }
  else if (Temperature >= 40 && timeElapsedBlynk > intervalBlynk)
  {
    timeElapsedBlynk = 0;
    Blynk.notify("อุณหภูมิสูงกว่าที่ควร 39องศา " + add);
  }
}
void BlynkNoti()
{
  checkResetNoti = notiSwitchReset;
  checkNotiSwitch = notiSwitch;

  if (timeElapsedBlynkNotiReset > intervalBlynkNotiReset)
  {
    if (checkResetNoti == 1)
    {
      notiStatus = 1;
    }
    timeElapsedBlynkNotiReset = 0;
  }
  if (checkResetNoti == 0)
  {
    notiStatus = 0;
    timeElapsedBlynkNotiReset = 0;
  }

  if (checkNotiSwitch == 1)
  {
    noti("NOW");
  }
  else if (notiStatus == 1)
  {
    noti("AUTO");
  }
}





//*!สำคัญ
void WorldRead()
{
  readStatus = myAHT10.readRawData(); //read 6 bytes from AHT10 over I2C
  if (readStatus != AHT10_ERROR)
  {
    //Serial.print(F("Temperature: ")); Serial.print(myAHT10.readTemperature(AHT10_USE_READ_DATA)); Serial.println(F(" +-0.3C"));
    //Serial.print(F("Humidity...: ")); Serial.print(myAHT10.readHumidity(AHT10_USE_READ_DATA));    Serial.println(F(" +-2%"));
    Temperature = (float)myAHT10.readTemperature(AHT10_USE_READ_DATA);
    Humidity = (float)myAHT10.readHumidity(AHT10_USE_READ_DATA);
  }
  else
  {
    Serial.print(F("[SENSOR]:-----ATH10 softReset Process: "));
    Serial.println(myAHT10.softReset()); //reset 1-success, 0-failed
    myAHT10.softReset();
  }
}
//*!สำคัญ
void RelayControl()
{
  if (Temperature >= 40)
  {
    relayStatus1 = "OFF";
    relayStatus2 = "OFF";
    digitalWrite(Relay1, HIGH);
    digitalWrite(Relay2, HIGH);
  }
  if (Temperature <= 36.5)
  {
    relayStatus1 = "ON";
    relayStatus2 = "ON";
    digitalWrite(Relay1, LOW);
    digitalWrite(Relay2, LOW);
  }
}
//*!สำคัญ
void setup()
{
  Serial.begin(115200);

  //*WifiManager
  WiFiManager wifiManager;
  //reset saved settings
  //TODO:make rest wifi SSID
  if (true)
  {
    //wifiManager.resetSettings();
  }
  wifiManager.autoConnect("EGG-INCUBATOR");
  //wifiManager.autoConnect();//or use this for auto generated name ESP + ChipID

  //*EEPROM
/////  EEPROM.begin(EEPROM_SIZE);

  //*GPIO
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(LedBulidIn, OUTPUT);
  digitalWrite(Relay1, HIGH);
  digitalWrite(Relay2, HIGH);
  digitalWrite(LedBulidIn, HIGH);

  //*AHT10 and OLED and Blynk connection
  C2W();

  //*WorldRead();
  readStatus = myAHT10.readRawData(); //read 6 bytes from AHT10 over I2C
  if (readStatus != AHT10_ERROR)
  {
    //Serial.print(F("Temperature: ")); Serial.print(myAHT10.readTemperature(AHT10_USE_READ_DATA)); Serial.println(F(" +-0.3C"));
    //Serial.print(F("Humidity...: ")); Serial.print(myAHT10.readHumidity(AHT10_USE_READ_DATA));    Serial.println(F(" +-2%"));
    Temperature = (float)myAHT10.readTemperature(AHT10_USE_READ_DATA);
    Humidity = (float)myAHT10.readHumidity(AHT10_USE_READ_DATA);
  }
}
//*!สำคัญ
void loop()
{
  //*TIMER
  if (timeElapsed > interval)
  {
    timeElapsed = 0;
    
  }

  WorldRead();    //-----สำคัณเหนือสิงอื่นได  //read data i2c
  RelayControl(); //-----สำคัณเหนือสิงอื่นได  //relay Controll

  OLED(); //SS1306 OLED
  //data2comport();//Serial get temp and hum

  Blynk.run();
  blynkRead(); //Blynk visual and analog pin read
  BlynkNoti();
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("wifi lost");
  }
  
}
