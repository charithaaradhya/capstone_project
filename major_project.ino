#include "BluetoothSerial.h"
//#include "string.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

const int sensorIn =34 ;//(analog pin of current sensor is connected to 34th pin esp32)
int mVperAmp = 185; // sensitivity of 5A current sensor 
int Relay=23;//control pin to relay is connected to 23rd pin of esp32
int Relay2=22;
double Vpp = 0;
double Vp=0;
double VRMS = 0;
double IRMS = 0;
double totalI=0;
double input;//input by user
unsigned long StartTime;
unsigned long CurrentTime;
unsigned long ElapsedTime ;
double Watth=0;
double hours=0;
char skt_byte=0;
String skt_i="";

void setup() {
  Serial.begin(115200);
  pinMode(23,OUTPUT);
  pinMode(34,INPUT);
  pinMode(22,OUTPUT);
  /*
   other current sensor and relay
   */
   delay(1000);
  digitalWrite(Relay,HIGH);
  digitalWrite(Relay2,HIGH);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  if (Serial.available()) {
   SerialBT.write(Serial.read());
  
  }
USER:  
  Serial.println("enter the watt hours needed:");
  while (SerialBT.available() == 0) {
    // Wait for User to Input Data
  }
  
  if (SerialBT.available()) {
    char value=SerialBT.read();
    String abc=String(value);
    int input=abc.toInt();
    Serial.print(input);
    Serial.println("Wh");
    delay(20);
    Serial.println("enter the socket:");
              while (SerialBT.available() == 0) {
                        // Wait for User to Input Data
                          }
              skt_byte=0;
              skt_i="";
              while(SerialBT.available()){
                         skt_byte=SerialBT.read();
                        //Serial.println(skt_byte);
                         if(skt_byte !='\n'){
                              skt_i += skt_byte;
                         }
                         }
      Serial.println(skt_i);
      if(skt_i=="a1bc")
      {
      digitalWrite(Relay,LOW);
      Serial.print("CHARGING......");
      StartTime = millis();
      Serial.print("StartTime");
      socket(input,Relay);
      }
      else if(skt_i=="a2bc")
      {
      digitalWrite(Relay2,LOW);
      Serial.print("CHARGING......");
      StartTime = millis();
      Serial.print("StartTime");
      socket(input,Relay2);
      }
      else{
      Serial.println("enter valid socket code");
      goto USER;
      }
 }
 }




void socket(int input,int relay)
{
while(1){
Vpp = getVPP();//to get proportional peak  voltage
Vp=Vpp/2.0;
VRMS =Vp*0.707; 
IRMS = ((VRMS * 1000)/mVperAmp)-0.13;
Serial.print("instantineous_current:");
Serial.print(IRMS);
Serial.println("Amps");
CurrentTime = millis();
ElapsedTime = CurrentTime - StartTime;
hours=ElapsedTime/3600000.0;
Serial.print("Elapsed TIme:");
Serial.print(hours,3);
Serial.println("hours");


//delay(2000);
//total current supplied should be limited to input

if(Watth<input){
  totalI=IRMS+totalI;
  Serial.print("total_current:");
  Serial.print(totalI);
  Serial.println("A"); 
  Watth=230*IRMS*hours;
  Serial.print("watt_hours:");
 Serial.println(Watth);
 Serial.println("Wh");
  
Serial.print("\n");

  }
else{
    digitalWrite(relay,HIGH);//load off
    Serial.print("Wh==");
    Serial.print(Watth);
    Serial.println("Wh");
    Serial.print("CHARGING COMPLETE......");
    Serial.println("UNPLUG");
    goto USER;
    }

delay(2000);
}
}

float getVPP()
{
float result;
int readValue; //value read from the sensor
int maxValue = 0; // store max value here
int minValue = 4095; // store min value here

uint32_t start_time = millis();
while((millis()-start_time) < 3000) //sample for 1 Sec
{
readValue = analogRead(sensorIn);
// see if you have a new maxValue
if (readValue > maxValue)
{
/*record the maximum sensor value*/
maxValue = readValue;
}
if (readValue < minValue)
{
/*record the minimum sensor value*/
minValue = readValue;
}
}
result = ((maxValue-minValue) * 3.3)/(4095.0);
return result;

}
