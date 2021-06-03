//#include <VariableTimedAction.h>
#include "BluetoothSerial.h"
//#include "string.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

char skt_byte=0;
String skt_i="";
int mVperAmp = 185;
unsigned long StartTime[10];
unsigned long ElapsedTime[10];
unsigned long CurrentTime;
int relay[]={23,22};
int sensor[]={34,35};
double Vpp[10];
double Vp[10];
int n=0;
double IRMS[10];
double VRMS[10];
double Watth[10];
double hours[10];
int socket[10];
int input[10];
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
  /*digitalWrite(Relay,HIGH);
  digitalWrite(Relay2,HIGH);
  */
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  Serial.println("enter the no of sockets:");
  while (Serial.available() == 0) {
    // Wait for User to Input Data
  }
  n = Serial.parseInt(); 
Serial.print(n);

for(int i=0;i<n;i++){
  
    socket[i]=0;
    input[i]=0;
    StartTime[i]=0;
    ElapsedTime[i]=0;
    Vpp[i]=0;
    Vp[i]=0;
    IRMS[i]=0;
    VRMS[i]=0;
     Watth[i]=0;
    hours[i]=0;
    digitalWrite(relay[i],HIGH);
  
  }
}



void initialize(int x){
  
  
    socket[x]=0;
    input[x]=0;
    StartTime[x]=0;
    ElapsedTime[x]=0;
    Vpp[x]=0;
    Vp[x]=0;
    IRMS[x]=0;
    VRMS[x]=0;
     Watth[x]=0;
    hours[x]=0;
    digitalWrite(relay[x],HIGH);
  
  
  
  
  
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
        if(socket[0]==0)
        {
          socket[0]=1;
        Serial.println("enter the watt hours needed:");
        while (SerialBT.available() == 0) {
          // Wait for User to Input Data
             }
  
         if (SerialBT.available()) {
          char value=SerialBT.read();
          String abc=String(value);
          input[0]=abc.toInt();
          Serial.print(input[0]);
           Serial.println("Wh");
            delay(20);
            StartTime[0]= millis();
         }
      }
      else
      {
        Serial.print("socket1 busy\n");
        }
      }
        
        else if(skt_i=="a2bc")
      {

        if(socket[1]==0){
        socket[1]=1;
        Serial.println("enter the watt hours needed:");
       while (SerialBT.available() == 0) {
          // Wait for User to Input Data
          }
  
         if (SerialBT.available()) {
         char value=SerialBT.read();
          String abc=String(value);
          input[1]=abc.toInt();
          Serial.print(input[1]);
            Serial.println("Wh");
           delay(20);
            StartTime[1] = millis();
      }
     
      }
      else
      {
        Serial.print("socket2 busy\n");
        }

      }
}


void turn_on_relay()
 {//checks the state variable of socket and turns on the relay accordingly 
  
 for(int j=0; j<n;j++){
  if(socket[j]==1)
  {
    digitalWrite(relay[j],LOW);
    Serial.printf("CHARGING......socket %d\n",j);
  }
  else{
     digitalWrite(relay[j],HIGH);
    }
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

  
void Socket()
{ //constantly checks if the supplied wh is less than or equal to wh required by user,cutts off when requirement is met.
  //and also checks for user input constantly. 

while(1){
check_for_input();
turn_on_relay();
for(int k =0;k<n;k++){
  Vpp[k]=getVPP(sensor[k]);
  Vp[k]=Vpp[k]/2.0;
  VRMS[k]=Vp[k]*0.707;
  IRMS[k]=((VRMS[k]* 1000)/mVperAmp)-0.13;
  }

//to get proportional peak  voltage
CurrentTime = millis();
for(int h=0;h<n;h++){
  ElapsedTime[h] = CurrentTime - StartTime[h];
  hours[h]=ElapsedTime[h]/3600000.0;
  }
for(int l=0;l<n;l++){
if(socket[l]==1){
    if(Watth[l]<input[l]){
    //totalI=IRMS1+totalI;
    Serial.printf("instantineous_current_socket%d:",l);
    Serial.print(IRMS[l]);
    Serial.println("Amps");
    Serial.print("Elapsed Time1:");
    Serial.print(hours[l],3);
    Serial.println("hours");
  /*Serial.print("total_current:");
  Serial.print(totalI 2);
  Serial.println("A");*/ 
    Watth[l]=230*IRMS[l]*hours[l];
    Serial.print("watt_hours_socket1:");
    Serial.print(Watth[l]);
    Serial.println("Wh");  
    Serial.print("\n");
     }
  else{
    digitalWrite(relay[l],HIGH);
    socket[l]=0;//load off
    Serial.print("Wh==");
    Serial.print(Watth[l]);
    Serial.println("Wh");
    Serial.print("CHARGING COMPLETE......");
    Serial.printf("UNPLUG_socket%d\n",l);
    initialize(l);
    }
}
}

delay(1000);
}

}

//TimedAction numberThread = TimedAction(700,incrementNumber);
//VariableTimedAction textThread = VariableTimedAction(2000,check_for_input);

void loop() {
  if (Serial.available()) {       
    //from serial monitor to android BT terminal
   SerialBT.write(Serial.read());
  
  }
  

  check_for_input();
 
  turn_on_relay();
  Socket();
  
  
}
