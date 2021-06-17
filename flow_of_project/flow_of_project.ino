
int SKT_FLAG[10];
#include "BluetoothSerial.h"
//#include "string.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
unsigned long previousTime_input=0;;
long timeInterval= 3000;
char skt_byte=0;
String skt_i="";
int mVperAmp = 185;
unsigned long StartTime[10];
unsigned long ElapsedTime[10];
unsigned long CurrentTime;
int relay[]={23,22,21};
int sensor[]={34,35,36};
double Vpp[10];
double Vp[10];
int n=0;
double IRMS[10];
double VRMS[10];
double Watth[10];
double hours[10];
int socket[10];
int input[10];

void initialize(int x){
  
  
    socket[x]=0;
    SKT_FLAG[x]=0;
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


void setup() {
  Serial.begin(115200);
  pinMode(23,OUTPUT);
  pinMode(34,INPUT);
  pinMode(22,OUTPUT);
  pinMode(35,INPUT);
  pinMode(21,OUTPUT);
   pinMode(36,INPUT);
  /*
   other current sensor and relay
   */
   delay(1000);
  /*digitalWrite(Relay,HIGH);
  digitalWrite(Relay2,HIGH);
  */
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  n=3;
  Serial.printf("no of sockets:%d",n);
  /*while (Serial.available() == 0) {
    // Wait for User to Input Data
  }
n = Serial.parseInt(); 
Serial.print(n);
Serial.println("\n");
*/
for(int i=0;i<n;i++){
    initialize(i);
  
}
}


int input_socket()
{ //takes for input from user, checks if socket is free , if free asks user to enter watt hours required. 
  int x;
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

      
      if(skt_i=="a1bc"){
              x=0;}
      else if(skt_i=="a2bc"){x=1;}           
      else if(skt_i=="a3bc"){x=2;}
      else{
          x=100;
          }
     
if(x==100){
  return -1;
  
  }

if(socket[x]==0){
          //socket[x]=1;
        Serial.println("enter the watt hours needed:");
        while (SerialBT.available() == 0) {
          // Wait for User to Input Data
             }
  
         if (SerialBT.available()) {
          char value=SerialBT.read();
          String abc=String(value);
          input[x]=abc.toInt();
          Serial.print(input[x]);
           Serial.println("Wh");
         
         }
         return x;
         }

  else{
      Serial.printf("socket %d busy\n", x);
      return -1;
      }  
         


           
      }
      
int price_calc(int x){
if(x==-1){
  return 0;
  
  }

else{
           int price=0;
           Serial.printf("total price would be %d\n",price); 
           Serial.print("ENTER 'y' TO CONTINUE");  
             while (SerialBT.available() == 0) {
          // Wait for User to Input Data
             }
           if (SerialBT.available()) {
            
            char v=SerialBT.read();
            if(v=='y'){
              socket[x]=1;
              return 1;
              }
            
            }
            else
            return -1;
         }
}
    

void turn_on_relay(int x)
 {//checks the state variable of socket and turns on the relay accordingly 
  

  if(socket[x]==1)
  {
    digitalWrite(relay[x],LOW);
    Serial.printf("CHARGING......socket %d\n",x);
    // StartTime[x]= millis();
    
  }
  /*else if(socket[x]==0){
     digitalWrite(relay[x],HIGH);
    */ 
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

  void processing_mod(int k){
  
  if(socket[k]==1){   
  Vpp[k]=getVPP(sensor[k]);
  Vp[k]=Vpp[k]/2.0;
  VRMS[k]=Vp[k]*0.707;
  IRMS[k]=((VRMS[k]* 1000)/mVperAmp)-0.13;
  CurrentTime = millis();
  ElapsedTime[k] = CurrentTime-StartTime[k];
  hours[k]=ElapsedTime[k]/3600000.0;
  if(Watth[k]<input[k]){
    //totalI=IRMS1+totalI;
    Serial.printf("instantineous_current_socket%d:",k);
    Serial.print(IRMS[k]);
    Serial.println("Amps");
    Serial.print("Elapsed Time:");
    Serial.print(hours[k],3);
    Serial.println("hours");
  /*Serial.print("total_current:");
  Serial.print(totalI 2);
  Serial.println("A");*/ 
    Watth[k]=230*IRMS[k]*hours[k];
    Serial.print("watt_hours_socket:");
    Serial.print(Watth[k]);
    Serial.println("Wh");  
    Serial.print("PRICE::");
    Serial.print("\n");
    //price calculation for active socket 
     }
  else{
    digitalWrite(relay[k],HIGH);
    socket[k]=0;//socket freed
    SKT_FLAG[k]=0;
    Serial.print("Wh==");
    Serial.print(Watth[k]);
    Serial.println("Wh");
    Serial.print("CHARGING COMPLETE......");
    Serial.printf("UNPLUG_socket%d\n",k);
    //calculate final price
    
    initialize(k);
    
  }
  }
  
 } 
 
  
void loop() {
 
   int in=input_socket();//also checks if the socket is free
   int UI=price_calc(in);
    //price calculation for socket
    
  if(UI==1){
    //send data to owner
    Serial.printf("\ndata sent for authentication (socket %d)\n",in);
    delay(1000);
    //authentication from owner for socket x ,do: SKT_FLAG [x]=1
    SKT_FLAG[1]=1;
    SKT_FLAG[0]=1;
    
    if(SKT_FLAG[in]==1)
    {
    Serial.printf("permission granted for socket %d\n",in);
    StartTime[in]= millis();
    turn_on_relay(in);
    }
    else{
      Serial.printf("permission denied for socket %d\n",in);
      
      }
  }
  
  delay(1000);
  
  for(int i=0;i<n;i++){
  if(SKT_FLAG[i]==1){
          
    processing_mod(i);

  }
  }
   
   
 

}
