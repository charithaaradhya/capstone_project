#include "BluetoothSerial.h"
//#include "string.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

int s1=0;//state of socket 1
int s2=0;//state of socket 2
const int sensorIn1 =34 ;//current senosr of socket 1 (analog pin of current sensor is connected to 34th pin esp32)
const int sensorIn2=35;//current senosr of 2
int mVperAmp = 185; // sensitivity of 5A current sensor 
int Relay=23;//control pin to relay is connected to 23rd pin of esp32//socket 1
int Relay2=22;//relay of socket 2
double Vpp1 = 0;
double Vpp2=0;
double Vp1=0;
double Vp2=0;
double VRMS1 = 0;
double VRMS2 = 0;
double IRMS1 = 0;
double IRMS2 = 0;
double totalI=0;
double input1=0,input2=0;//input by user
unsigned long StartTime1,StartTime2;
unsigned long CurrentTime;
unsigned long ElapsedTime1,ElapsedTime2 ;
double Watth1=0;
double Watth2=0;

double hours1=0;
double hours2=0;
char skt_byte=0;
String skt_i="";

void setup() {
  Serial.begin(115200);
  pinMode(23,OUTPUT);
  pinMode(34,INPUT);
  pinMode(22,OUTPUT);
  pinMode(35,INPUT);
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
    //from serial monitor to android BT terminal
   SerialBT.write(Serial.read());
  
  }

  check_for_input();
 
  turn_on_relay();
  socket();
  
  
}


 void turn_on_relay()
 {
  //checks the state variable of socket and turns on the relay accordingly 
  if(s1==1 and s2==0){
    digitalWrite(Relay,LOW);
      Serial.print("CHARGING......socket 1\n");
      //StartTime1 = millis();
  }
  else if(s1==0 and s2==1){
    digitalWrite(Relay2,LOW);
     Serial.print("CHARGING......socket 2\n");
      //StartTime2 = millis();
  }
  else if(s1==1 and s2==1){
    digitalWrite(Relay,LOW);
    Serial.print("CHARGING......socket 1\n");
    //StartTime1 = millis();
    digitalWrite(Relay2,LOW);
    Serial.print("CHARGING......socket 2\n");
     //StartTime2 = millis();
  }
  else {
    digitalWrite(Relay,HIGH);
    digitalWrite(Relay2,HIGH);
    }
  
  }


void socket()
{ //constantly checks if the supplied wh is less than or equal to wh required by user,cutts off when requirement is met.
  //and also checks for user input constantly. 

while(1){
check_for_input();
turn_on_relay();
Vpp1= getVPP(sensorIn1);
Vpp2= getVPP(sensorIn2);//to get proportional peak  voltage
Vp1=Vpp1/2.0;
Vp2=Vpp2/2.0;
VRMS1 =Vp1*0.707; 
VRMS2=Vp2*0.707;
IRMS1=((VRMS1* 1000)/mVperAmp)-0.13;
IRMS2 =((VRMS2* 1000)/mVperAmp)-0.13;
CurrentTime = millis();

ElapsedTime1 = CurrentTime - StartTime1;
ElapsedTime2 = CurrentTime - StartTime2;

hours1=ElapsedTime1/3600000.0;
hours2=ElapsedTime2/3600000.0;

if(s1==1){
if(Watth1<input1){
  totalI=IRMS1+totalI;
  Serial.print("instantineous_current(socket1):");
  Serial.print(IRMS1);
  Serial.println("Amps");
  Serial.print("Elapsed Time1:");
  Serial.print(hours1,3);
  Serial.println("hours");
  /*Serial.print("total_current:");
  Serial.print(totalI 2);
  Serial.println("A");*/ 
  Watth1=230*IRMS1*hours1;
  Serial.print("watt_hours_socket1:");
  Serial.print(Watth1);
  Serial.println("Wh");  
  Serial.print("\n");
  }
else{
    digitalWrite(Relay,HIGH);
    s1=0;//load off
    Serial.print("Wh==");
    Serial.print(Watth1);
    Serial.println("Wh");
    Serial.print("CHARGING COMPLETE......");
    Serial.println("UNPLUG_socket1");
    }
}

if(s2==1){    
if(Watth2<input2){
  totalI=IRMS2+totalI;
  
  Serial.print("instantineous_current(socket2)");
  Serial.print(IRMS2);
  Serial.println("Amps");
  Serial.print("Elapsed Time2:");
  Serial.print(hours2,3);
  Serial.println("hours");
  /*Serial.print("total_current:");
  Serial.print(totalI);
  Serial.println("A");*/ 
  Watth2=230*IRMS2*hours2;
  Serial.print("watt_hours_socket2:");
  Serial.println(Watth2);
  Serial.println("Wh");  
  Serial.print("\n");
  }
else{
    digitalWrite(Relay2,HIGH);
    s2=0;//load off
    Serial.print("Wh==");
    Serial.print(Watth2);
    Serial.println("Wh");
    Serial.print("CHARGING COMPLETE......");
    Serial.println("UNPLUG_socket2");
    }
}
delay(1000);
}
}




float getVPP(int sensorIn)
{
  //returns peak to peak voltage 
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

void check_for_input()
{ //takes for input from user, checks if socket is free , if free asks user to enter watt hours required. 
  
  Serial.println("enter the socket:");
              
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
        if(s1==0)
        {
          s1=1;
        Serial.println("enter the watt hours needed:");
        while (SerialBT.available() == 0) {
          // Wait for User to Input Data
             }
  
         if (SerialBT.available()) {
          char value=SerialBT.read();
          String abc=String(value);
          input1=abc.toInt();
          Serial.print(input1);
           Serial.println("Wh");
            delay(20);
            StartTime1 = millis();
         }
      }
      else
      {
        Serial.print("socket1 busy\n");
        }
      }
        
        else if(skt_i=="a2bc")
      {

        if(s2==0){
        s2=1;
        Serial.println("enter the watt hours needed:");
       while (SerialBT.available() == 0) {
          // Wait for User to Input Data
          }
  
         if (SerialBT.available()) {
         char value=SerialBT.read();
          String abc=String(value);
          input2=abc.toInt();
          Serial.print(input2);
            Serial.println("Wh");
           delay(20);
            StartTime2 = millis();
      }
     
      }
      else
      {
        Serial.print("socket2 busy\n");
        }

      }
}
      
