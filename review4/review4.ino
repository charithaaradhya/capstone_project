#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

#include <Keypad.h>

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {32, 33, 25, 26}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {27, 14, 13, 19}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

String inputString;
long inputInt;

int SKT_FLAG[10];
unsigned long previousTime_input=0;
long timeInterval= 3000;
char skt_byte=0;
String skt_i="";
int mVperAmp = 185;
unsigned long StartTime[10];
unsigned long ElapsedTime[10];
unsigned long CurrentTime;
int relay[]={18,23};
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
   pinMode(18,OUTPUT);
  /*
   other current sensor and relay
   */
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME!!"); 
   inputString.reserve(10);   
  delay(1000);
  
 
  
  n=2;//NO.of SOCKETS
  Serial.printf("no of sockets:%d",n);
  for(int i=0;i<n;i++){
    initialize(i);
  
}

// input_from_user();
   
}


int input_from_user()
{ //takes for input from user, checks if socket is free , if free asks user to enter watt hours required. 
  int x=0;
  inputString="";
  char key=0;
  skt_i="";
  double CT=0;
  lcd.setCursor(0,2);
  lcd.print("ENTER THE SOCKET"); 
  delay(1000);
  Serial.println(" enter the socket:");
  double ct=0;
  double st=millis();
  while((key==0)&&(ct<3000)){
         key = keypad.getKey();
         ct=millis()-st;
         //Serial.print("1\n");
         //if(ct>5000)break;
        }
          
  //key = keypad.getKey();
  lcd.clear();
  lcd.home();
 // inputString="";
  if(key){
    double ST=millis();
  while((key!='#' )&&(CT<5000)){
   if(key){            // append new character to input string
    
    if (key == '*') {
      inputString = "";                 // clear input
    }
    else{
    inputString += key;
    lcd.clear();
    lcd.print(inputString);
    }
   }
  key = keypad.getKey();
  CT=millis()-ST;
  }
  if (key == '#') {
      if (inputString.length() > 0) {
        //inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
        Serial.print(inputString);
        Serial.print("\n");
        skt_i=inputString;
        inputString = "";               // clear input
        // DO YOUR WORK HERE
 
      }
    } 
}
else return -1;
//skt_i=inputString;
//Serial.println(inputString);
Serial.print(skt_i);
Serial.print("\n");

if(skt_i=="A1BC"){x=0;}
else if(skt_i=="A2BC"){x=1;}           
else if(skt_i=="A3BC"){x=2;}
else{x=100;return -1;}
Serial.print(x); 
Serial.print('\n');  

if(socket[x]==0){
  double st=millis();
  double ct=0;

        Serial.println("enter the watt hours needed:");
        lcd.clear();
        lcd.home();
        lcd.print("enter the watt hours needed:");
        char key=0;
        while((key==0)&&(ct<5000)){
         key = keypad.getKey();
         ct=millis()-st;
         //Serial.print("1\n");
         //if(ct>5000)break;
        }
        if(ct>=5000){lcd.clear();lcd.home();lcd.print("TIME OUT FOR SOCKET ");lcd.print(x);delay(1000);}
           
        if(key){
           lcd.clear();
           lcd.home();
           //ouble ct=0;
           while(key!='#'){
               if(key){            // append new character to input string
                    
                  if (key == '*') {
                          inputString = ""; 
                          lcd.print(inputString);// clear input
                          }
                  else{
                        inputString += key;
                        lcd.print(inputString);
                       }
                }             
                 key = keypad.getKey();
                 
         }
        
         if (key == '#') {
                      if (inputString.length() > 0) {
                          inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
                          //Serial.print(inputInt);
                          //Serial.print("\n");
                          lcd.clear();
                          lcd.home();
                          lcd.print("WATT_HRS:");
                          lcd.setCursor(0,1);
                          lcd.print(inputInt);
                          inputString = "";   
                          delay(3000);// clear input
                           // DO YOUR WORK HERE 
                          }
                      }
     }
   
          input[x]=inputInt; 
          inputInt=0; 
          if(input[x]==0){return -1;}
          Serial.print(input[x]);
          Serial.println("Wh");
          
          return x;      
  }

else{
      Serial.printf("socket %d busy\n", x);
      lcd.clear();
      lcd.home();
      lcd.print("SOCKET BUSY! TRY OTHER SOCKET.");
      delay(1000);
      return -1;
      } 
skt_i="";

       
}


int price_calc(int x){
  
if(x==-1){
          Serial.print("-1\n");
           return -1;
  }
else{
           int price=0;
           Serial.printf("total price would be %d\n",price); 
           lcd.clear();
           lcd.home();
           lcd.print("WATT_HRS:");
           lcd.print(input[x]);
           lcd.setCursor(0,1);
           lcd.print("PRICE:");
           lcd.print(price);
           lcd.setCursor(0,2);
           lcd.print("ENTER '1' TO CONTINUE");
           //lcd.setCursor(0,3);
          // lcd.print("ENTER '2' TO CANCEL");
           
           
           Serial.print("ENTER '1' TO CONTINUE"); 
           char key=0;
           double st=millis();
           double ct=0;
           while((key==0)&&(ct<10000)){
                key = keypad.getKey();
                ct=millis()-st;
              } 
              
            if(key=='1'){
              socket[x]=1;
              return 1;
              
            }
            else
           { return -1;
             lcd.clear();
             lcd.home();
             lcd.print("TIME OUT FOR SOCKET");
             lcd.print(x);
             delay(1000);}
         }
}
    
int authentication(int in){
   Serial.printf("\ndata sent for authentication (socket %d)\n",in);
   lcd.clear();
   lcd.home();
   lcd.print("data sent for authentication...");
    delay(1000);
    /*send to owner*/
    //reply from owner
    SKT_FLAG[1]=1;
    SKT_FLAG[0]=1;
    
    if(SKT_FLAG[in]==1)
    {
    Serial.printf("permission granted for socket %d\n",in);
    lcd.clear();
    lcd.home();
    lcd.print("permission granted for socket");
    lcd.print(in);
    delay(1000);
    StartTime[in]= millis();
    turn_on_relay(in);
    }
    else{
      Serial.printf("permission denied for socket %d\n",in);
      lcd.clear();
      lcd.home();
      lcd.print("permission denied for socket");
      lcd.print(in);
      }


  }
void turn_on_relay(int x)
 {//checks the state variable of socket and turns on the relay accordingly 
  

  if(socket[x]==1)
  {
    digitalWrite(relay[x],LOW);
    Serial.printf("CHARGING......socket %d\n",x);
    lcd.print("CHARGING>socket");
    lcd.print(x);
    // StartTime[x]= millis();
    
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
while((millis()-start_time) < 3000) //sample for 3 Sec
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
      IRMS[k]=((VRMS[k]* 1000)/mVperAmp)-0.15;
      CurrentTime = millis();
      ElapsedTime[k] = CurrentTime-StartTime[k];
      hours[k]=ElapsedTime[k]/3600000.0;
      
      if(Watth[k]<input[k]){
            Serial.printf("instantineous_current_socket%d:",k);
            Serial.print(IRMS[k]);
            Serial.println("Amps");
            Serial.print("Elapsed Time:");
            Serial.print(hours[k],3);
            Serial.println("hours");
            Watth[k]=230*IRMS[k]*hours[k];
            Serial.print("watt_hours_socket:");
            Serial.print(Watth[k]);
            Serial.println("Wh");  
            Serial.print("PRICE::");
            Serial.print("\n");
            lcd.clear();
            lcd.home();
            lcd.print("SOCKET:");
            lcd.print(k);
            lcd.setCursor(0,1);
            lcd.print("WATT_HRS:");
            lcd.print(Watth[k]);
            delay(1000);
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
            lcd.clear();
            lcd.home();
            lcd.print("CHARGING COMPLETE......");
            lcd.printf("UNPLUG_socket%d\n",k);
            delay(2000);
            initialize(k);
            }
     }
  
 } 
 

  
void loop() {
  
  int skt=input_from_user();
  int UI=price_calc(skt);
  if(UI==1){
    //send to owner
    authentication(skt);
  }
  for(int i=0;i<n;i++){
  if(SKT_FLAG[i]==1){
          
    processing_mod(i);

  }
  }
  
  
  
  
  
  }
   
   //price calculation for active socket 
   
   
 
