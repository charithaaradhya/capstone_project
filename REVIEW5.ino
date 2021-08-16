//REVIEW 5 CODE:
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,4);

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

#include <WiFi.h>
//#include <ESPAsyncWebServer.h>;
// Replace with your network credentials
const char* ssid = "C002";
const char* password = "GreenFinch";
// Set web server port number to 80
WiFiServer server(80);
//AsyncWebServer server(80);
// Variable to store the HTTP request
String header;
// Auxiliar variables to store the current output state
String Accept[5];
String Reject[5];//= "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

String inputString;
long inputInt;
int price[10];
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
String socket_id[]={"A1BC","A2BC","A3BC"};
String skt_state[10];

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
    Accept[x]="off";
    Reject[x]="off";
    skt_state[x]="FREE";
    digitalWrite(relay[x],HIGH);  
}


void setup() {
   Serial.begin(115200);
  pinMode(21,OUTPUT);//SDA of LCD
  pinMode(22,OUTPUT);//SCL ofLCD
  
  pinMode(18,OUTPUT);//relay[0]
  pinMode(23,OUTPUT);//relay[1]
  
  pinMode(34,INPUT);//CT sensor[0]
  pinMode(35,INPUT);//ct sensor[1]
  /*
   other current sensor and relay
   */

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME!!"); 
  inputString.reserve(10);  //input limited to 10 characters 
  delay(1000);
  
 
  
  n=2;//NO.of SOCKETS
  Serial.printf("no of sockets:%d",n);
  for(int i=0;i<n;i++){
    initialize(i);
  
}
   
}


int input_from_user()
{ //takes for input from user, checks if socket is free , if free asks user to enter watt hours required. 
  int x=0;//return variable i.e., socket number
  inputString="";
  char key=0;
  skt_i="";
  double CT=0;
  
  lcd.setCursor(0,3);
  lcd.print("ENTER THE SOCKET"); 
  delay(800);
  Serial.println(" enter the socket:");
  double ct=0;
  double st=millis();
  while((key==0)&&(ct<3000)){
    //waits 3 seconds for 1st key input
         key = keypad.getKey();
         ct=millis()-st;
        } 
  lcd.clear();
  lcd.home();
  if(key){
    double ST=millis();
     while((key!='#' )&&(CT<10000)){
    //the whole socket id should be given in max 10 sec
    if(key){            
    
    if (key == '*') {
      inputString = "";// clear input
      lcd.clear();
      lcd.print(inputString);
    }
    else{
    inputString += key;// append new character to input string
    lcd.clear();
    lcd.print(inputString);
    }
   }
  key = keypad.getKey();
  CT=millis()-ST;
  }
  if(CT>10000){lcd.clear();lcd.home();lcd.print("TIME OUT FOR SOCKET ");delay(1000);}

  if (key == '#') {
      if (inputString.length() > 0) {
        Serial.print(inputString);
        Serial.print("\n");
        skt_i=inputString;  //copy to skt_i
        inputString = "";   // clear input
      
 
      }
    } 
}
else return -1;// if no key is pressed then return -1

Serial.print(skt_i);
Serial.print("\n");
//if input given by user matches socket id return socket no.  else eturn -1
if(skt_i=="A1BC"){x=0;}
else if(skt_i=="A2BC"){x=1;}           
else if(skt_i=="A3BC"){x=2;}
else{x=100;return -1;}
Serial.print("SKT_NO:");
Serial.println(x); 
Serial.print('\n');  

if(socket[x]==0){
  //if socket is not in use only then ask for watthours
  double st=millis();
  double ct=0;

        Serial.println("enter the watt hours needed:");
        lcd.clear();
        lcd.home();
        lcd.print("enter the watt hours needed:");
       
        char key=0;
       
        while((key==0)&&(ct<5000)){
          //waits for 5sec for 1st char of input
         key = keypad.getKey();
         ct=millis()-st;
        }
        //if no input for 5 sec then TIME OUT
        if(ct>=5000){lcd.clear();lcd.home();lcd.print("TIME OUT FOR SOCKET ");lcd.print(socket_id[x]);delay(1000);}
           
        if(key){
           lcd.clear();
           lcd.home();
           double ct=0;
           while((key!='#')and(ct<10000) ){
            //the whole input watthrs should be entered in 10sec
               if(key){            
                    
                  if (key == '*') {
                          inputString = "";
                          lcd.clear(); 
                          lcd.home();
                          lcd.print(inputString);// clear input
                          }
                  else{
                        inputString += key;
                        lcd.print(inputString);
                       }
                }             
                 key = keypad.getKey();
                 ct=millis()-st;
         }
         if(ct>=10000){lcd.clear();lcd.home();lcd.print("TIME OUT FOR SOCKET ");lcd.print(socket_id[x]);delay(1000);}
        
         if (key == '#') {
                      if (inputString.length() > 0) {
                          inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
                          lcd.clear();
                          lcd.home();
                          lcd.print("WATT_HRS:");
                          lcd.setCursor(0,1);
                          lcd.print(inputInt);
                          inputString = "";   
                          delay(800);// clear input
                           
                          }
                      }
     }
   
          input[x]=inputInt;//copy to global variable
          inputInt=0; 
          if(input[x]==0){return -1;}//if no input return -1 
          Serial.print(input[x]);
          Serial.println("Wh");
          
          return x;      
  }

else{
      Serial.printf("socket %d busy\n",x);
      lcd.clear();
      lcd.home();
      lcd.print("SOCKET BUSY! TRY OTHER SOCKET.");
      delay(1000);
      return -1;
      } 
skt_i="";

       
}


int price_calc(int x){
 //calculates price for given watt hours for socket x 
if(x==-1){
          Serial.print("-1\n");
           return -1;
  }
else{
           price[x]=input[x]*2;
           Serial.printf("total price would be %d\n",price[x
           
           
           
           ]); 
           lcd.clear();
           lcd.home();
           lcd.print("WATT_HRS:");
           lcd.print(input[x]);
           lcd.setCursor(0,1);
           lcd.print("PRICE:");
           lcd.print(price[x]);
           lcd.setCursor(0,2);
           lcd.print("ENTER '1' TO CONTINUE");
           
           Serial.print("ENTER '1' TO CONTINUE"); 
           char key=0;
           double st=millis();
           double ct=0;
           while((key==0)&&(ct<10000)){//enter 1 within 10 sec
                key = keypad.getKey();
                ct=millis()-st;
              } 

            Serial.print(key);  
              
            if(key=='1'){
              socket[x]=1;//socket is no more free
              return 1;
              
            }
            else
           { 
             lcd.clear();
             lcd.home();
             lcd.print("TIME OUT FOR SOCKET:");
             lcd.print(socket_id[x]);
             delay(1000);
             return -1;
             }
         }
}
int my_server(int x){
  Serial.printf("\ndata sent for authentication (socket %d)\n",x);
  lcd.clear();
  lcd.home();
  lcd.print("data sent for authentication...");
   // delay(1000);
  double sstime=millis();
  double sctime=0;
while(sctime<60000){ 
    sctime=millis()-sstime; 
    WiFiClient client = server.available();   // Listen for incoming clients if client connects and has incomming data returns client else 0
    if (client) {                           
      currentTime = millis();
      previousTime = currentTime;
     // Serial.println("owner logged in");          // print a message out in the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
         if (client.available()) {             // if there's bytes to read from the client,
               char c = client.read();             // read a byte, then
               Serial.write(c);                    // print it out the serial monitor
               header += c;
                     if (c == '\n') {                  
                           if (currentLine.length() == 0) {
                                                           // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                                                           // and a content-type so the client knows what's coming, then a blank line:
                                   client.println("HTTP/1.1 200 OK");
                                   client.println("Content-type:text/html");
                                   client.println("Connection: close");
                                   client.println();
                                   if (header.indexOf("GET /accept") >= 0) {
                                            Serial.println("ACCEPTED");
                                             Accept[x] = "on";
                                           //reply from owner
                                            SKT_FLAG[x]=1;
                                            skt_state[x]="ACTIVE";
                                          /*  client.println("<!DOCTYPE html><html>");
                                            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                                            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
                                            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                                            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                                            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                                            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
                                            client.println("<body><h1>ESP32 Web Server</h1>");
                                         
                                            client.println("<p>SOCKET"+String(x)+"</p>");
                                            client.println("<p>WATT_HOURS:"+String(input[x])+"</p>");
                                            client.println("<p>PRICE:</p>");
                                            */
                                            client.println("<p>REQUEST ACCEPTED!!! THANK YOU.</p>");
                                            
                                            header = "";
    // Close the connecti
                                            return 1;
                              
                                             } 
            
                                else if (header.indexOf("GET /reject") >= 0) {
                                            Serial.println("REJECT");
              
                                            SKT_FLAG[x]=0;
                                            Reject[x]="on";
                                            skt_state[x]="FREE";
                                            
                                       /*     
                                            client.println("<!DOCTYPE html><html>");
                                            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                                            client.println("<link rel=\"icon\" href=\"data:,\">");
                                          // CSS to style the on/off buttons 
                                          // Feel free to change the background-color and font-size attributes to fit your preferences
                                             client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                                            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                                            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                                            client.println(".button2 {background-color: #555555;}</style></head>");
                                          
                                          // Web Page Heading
                                            client.println("<body><h1>ESP32 Web Server</h1>");
                                         
                                            client.println("<p>SOCKET"+String(x)+"</p>");
                                            client.println("<p>WATT_HOURS:"+String(input[x])+"</p>");
                                           */ 
                                            header="";
                                            initialize(x);
                                            client.println("<p>REQUEST DECLINED!!! THANK YOU.</p>");
                                            return -1;
                                             // digitalWrite(output27, HIGH);
                                            }
                      
            
            // Display the HTML web page
                             client.println("<!DOCTYPE html><html>");
                             client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                             client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
                            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                           // client.println(".p1{ background-color: #4CAF50; border: none; color: blue; padding: 16px 40px;");
                            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
                            
                            client.println("<body><h1> ESP32 Web Server</h1>");
                            client.println("<p style=color:blue;>REQUEST:-</p>");
                            client.println("<p style=color:blue;>SOCKET"+String(x)+"</p>");
                            client.println("<p style=color:blue;>WATT_HOURS:"+String(input[x])+"</p>");
                            client.println("<p style=color:blue;>PRICE:"+String(price[x])+"Rs.</p>");
            // If the output26State is off, it displays the ON button  
            client.println("<p>\n</p>") ;
            client.println("<p>SOCKET:"+String(0)+" State: "+skt_state[0]+"</p>");
            //client.println("<p>State " + skt_state[0] + "</p>");
            client.println("\n");
            client.println("<p>SOCKET:"+String(1)+" State: " + skt_state[1] + "</p>");
          //  client.println("<p>State " + skt_state[1] + "</p>");     
                            if (Accept[x]=="off") {
                              client.println("<p><a href=\"/accept\"><button class=\"button\">ACCEPT</button></a></p>");
                            }
               
                                                        // Display current state, and ON/OFF buttons for GPIO 27  
                                                       // client.println("<p>GPIO 27 - State " + output27State + "</p>");
                                                        // If the output27State is off, it displays the ON button       
                              if (Reject[x]=="off") {
                                client.println("<p><a href=\"/reject\"><button class=\"button\">REJECT</button></a></p>");
                              }
                           client.println("</body></html>");
            
            // The HTTP response ends with another blank line
                            client.println();
            // Break out of the while loop
                              break;
          }
          else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
   // Serial.println("Client disconnected.");
    Serial.println("");
  }
}
}
    
int authentication(int in){
   Serial.printf("\ndata sent for authentication (socket %d)\n",in);
   lcd.clear();
   lcd.home(); 
   lcd.print("data sent for authentication...");
   delay(800);
    /*send to owner*/
    my_server(in);
    //reply from owner
    /*SKT_FLAG[1]=1;
    SKT_FLAG[0]=1;
    */
    if(SKT_FLAG[in]==1)
    {
    Serial.printf("permission granted for socket %d\n",in);
    lcd.clear();
    lcd.home();
    lcd.print("permission granted for socket");
    lcd.print(socket_id[in]);
    delay(800);
    StartTime[in]= millis();
    turn_on_relay(in);
    }
    else{
      Serial.printf("permission denied for socket %d\n",in);
      lcd.clear();
      lcd.home();
      lcd.print("permission denied for socket");
      delay(800);
      lcd.print(socket_id[in]);
      socket[in]=0;//socket is again free
      }


  }
void turn_on_relay(int x)
 {//checks the state variable of socket and turns on the relay accordingly 
  

  if(socket[x]==1)
  {
    digitalWrite(relay[x],LOW);
    Serial.printf("CHARGING......socket %d\n",x);
    lcd.clear();
    lcd.home();
    lcd.print("CHARGING>socket");
    lcd.print(socket_id[x]);
    delay(800);
   
    
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
      Vpp[k]=getVPP(sensor[k]);//gets peak to peak voltage for socket x
      Vp[k]=Vpp[k]/2.0;//calculate peak voltage
      VRMS[k]=Vp[k]*0.707;//calculate RMS voltage
      IRMS[k]=((VRMS[k]* 1000)/mVperAmp)-0.13; //calculates the RMScurrent
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
            Serial.print("\n");
            lcd.clear();
            lcd.home();
            lcd.print("SOCKET:");
            lcd.print(socket_id[k]);
            lcd.setCursor(0,1);
            lcd.print("WATT_HRS:");
            lcd.print(Watth[k]);
            delay(800);
           }
      else{
            digitalWrite(relay[k],HIGH);
            socket[k]=0;//socket freed
            SKT_FLAG[k]=0;
            Serial.print("Wh==");
            Serial.print(Watth[k]);
            Serial.println("Wh");
            Serial.print("CHARGING COMPLETE......");
            Serial.printf("UNPLUG_socket%s\n",socket_id[k]);
            //calculate final price
            lcd.clear();
            lcd.home();
            lcd.print("CHARGING COMPLETE......");
            lcd.setCursor(0,1);
            lcd.printf("UNPLUG_socket%s\n",socket_id[k]);
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
   
   
 
