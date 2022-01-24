#include <DS3231.h>
#include <Keypad.h>

#include <Servo.h>
#include <SoftwareSerial.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  

DS3231  rtc(SDA, SCL);
Time t;

SoftwareSerial sim(3,4);//G B

//------------------------------------------SETTING UP KEYPAD-------------------------------------------------------------------

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {13, 12, 11, 10}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );



//--------------------------------------------VARIABLES--------------------------------------------------------------------------
//inputs
String inputString;// keypad input string 
String invalidMsg="INVALID ENTRY";
String welcome="WELCOME";



//Container One time Variables 
int c1Hrs1,c1Mins1;
int c1Hrs2,c1Mins2;
int c1Hrs3,c1Mins3;

//container one dosage variables 
int c1Dosage;

//Container Two time Variables 
int c2Hrs1,c2Mins1;
int c2Hrs2,c2Mins2;
int c2Hrs3,c2Mins3;

//container Two dosage variables 
int c2Dosage;



//buzzer and led 

int buzzPin = 2;
int ledPin=14;
int delayTime=5000;

//Servo/Dispensing
int pos=0;
int j;
int servoPin1=16;//grey
int servoPin2=17;//white



 

Servo myServo1;//creating a virtual object to interact with the servo
Servo myServo2;

//GSM/Remote Alterting
int _timeout;
String _buffer;
String SMS = "It is medication time";
//contact variables 
String contactNo;
String countryCode="+254";

void setup() {
  Serial.begin(9600);
  inputString.reserve(10); // maximum number of digit for a number is 10, change if needed
  pinMode (buzzPin, OUTPUT);//buzzer
  pinMode (ledPin, OUTPUT);//LED

  myServo1.attach(servoPin1);//servo1
  myServo2.attach(servoPin2);//servo2

  myServo1.write(pos);/// servo to position 0/dispenser closed
  myServo2.write(pos);

  rtc.begin();
  
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  
  _buffer.reserve(50);
  sim.begin(9600);
  delay(1000);


  
  Inputs();
}


 
 void loop() {
  
  timeCheck();
  delay(1000);
  lcd.clear();
}



//--------------------------------------------------------REAL TIME CLOCK ALARM--------------------------------------------------
void timeCheck(){
  String a=":";
   t = rtc.getTime();
  Serial.print(t.hour);
  Serial.print(":");
  Serial.print(t.min);
  Serial.print(":");
  Serial.print(t.sec);

  lcd.print(t.hour + a + t.min + a + t.sec);
  Serial.println();
  
  //Container One
  if(t.hour==c1Hrs1 && t.min==c1Mins1 && t.sec==0 ){
   
   
   lcd.setCursor(0,1);
   lcd.print("DISPENSER 1");
   dispensingOne();
   alerting();
   clearLCD();}
   
 else if (t.hour==c1Hrs2 && t.min==c1Mins2 && t.sec==0 ){
   
  lcd.setCursor(0,1);
  lcd.print("DISPENSER 1");
   dispensingOne();
   alerting();
   clearLCD(); }
   
 else if(t.hour==c1Hrs3 && t.min==c1Mins3 && t.sec==0 ){
  
   lcd.setCursor(0,1);
   lcd.print("DISPENSER  1");
   dispensingOne();
   alerting();
   clearLCD(); }

//Container Two
 else if(t.hour==c2Hrs1 && t.min==c2Mins1 && t.sec==0 ){
   
    lcd.setCursor(0,1);
    lcd.print("DISPENSER  2");
    dispensingTwo();
    alerting();
    clearLCD();}
   
 else if(t.hour==c2Hrs2 && t.min==c2Mins2 && t.sec==0 ){
  
   lcd.setCursor(0,1);
   lcd.print("DISPENSER 2");
    dispensingTwo();
    alerting();
    clearLCD();}
   
 else  if(t.hour==c2Hrs3 && t.min==c2Mins3 && t.sec==0 ){
  
   lcd.setCursor(0,1);
   lcd.print("DISPENSER 2");
    dispensingTwo();
    alerting();
    clearLCD(); }
  
 }
//system trigger

void alerting(){
   localAlarm();
   SendMessage();
   delay(10000);
   localAlarm();
}
//--------------------------------------------------------LOCAL ALARM FUNCTION--------------------------------------------------- 
  void localAlarm(){
    digitalWrite(buzzPin, HIGH);
    delay(100);
    digitalWrite(ledPin,HIGH);
    delay(delayTime);
    digitalWrite(buzzPin, LOW);
    delay(100);
    digitalWrite(ledPin,LOW);
  }

//--------------------------------------------------------SERVO DISPENSING FUNCTION--------------------------------------------- 
void dispensingOne(){
  
  for(j=1; j<=c1Dosage;j=j+1){
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myServo1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myServo1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  }
}

void dispensingTwo(){
  for(j=1; j<=c2Dosage;j=j+1){
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myServo2.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myServo2.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  }
}

 
//---------------------------------------------------------SENDING SMS FUNCTION--------------------------------------------------
void SendMessage()
{ Serial.println("System Started...");
  Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(200);
  Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + contactNo + "\"\r"); //Mobile phone number to send message
  delay(200);

  sim.println(SMS);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(200);
  _buffer = _readSerial();

  //if (sim.available() > 0){
 // Serial.write(sim.read());
 // }
}


String _readSerial() {
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}


//----------------------------------------------CLEARING THE LCD---------------------------------------------------------------
void clearLCDLine(int line){
for(int n = 0; n < 16; n++) { // 20 indicates symbols in line. For 2x16 LCD write - 16
lcd.setCursor(n,1);
lcd.print(" ");
}
lcd.setCursor(0,1); // set cursor in the beginning of deleted line
}

void clearLCD(){
delay(3000);
 lcd.clear();
}
//---------------------------------------------------------INPUTS TO THE SYSTEM---------------------------------------------------
void Inputs(){
  Serial.println(welcome);
  lcd.print(welcome);
  clearLCD();
  GetContact();

  //container 1
  Cont1Frequency();
  Cont1Dosage();

//container 2
  ContainerTwo();

  }


//-----------------------------keypad inputs loop-------------------------------------  
void keypadLoop(){
   String num;
   char key = keypad.getKey();
   lcd.setCursor(0,1);
   
   while(key != '#')
   {
      switch (key)
      {
         case NO_KEY:
            break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            Serial.print(key);
            lcd.print(key);
            num = num+key;
            
            break;
           

         case '*':
            num= "";
            clearLCDLine(1);
            break;
      }
      key = keypad.getKey(); 
          
   }
   inputString=num;
}
//-----------------------------Contact Number input Function--------------------------
void GetContact(){
  String outputMsg = "Contact: ";
  String contactQuiz = "Input PhoneNo(7****";
  String contactLen;
  
   Serial.println(contactQuiz);
   lcd.print(contactQuiz);
    
   keypadLoop();
   lcd.clear();
   
   contactLen=inputString;
   if (contactLen.length()==9){
   
   contactNo = countryCode + inputString;
   Serial.println();
    Serial.println(outputMsg+ contactNo); 
   lcd.print(outputMsg);
   lcd.setCursor(0,1);
   lcd.print(contactNo);
   clearLCD();
}
   else{
    Serial.println();
    Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    GetContact();}
}


//---------------------------------------------------------CONTAINER ONE TIME & DOSAGE INPUTS-------------------------------------
//--------------------------Frequency of dosage per day------------------------------
void Cont1Frequency()
{  Serial.println();
  String contOne="DISPENSER 1"; 
   String cont1FreqQuiz="Input Times/Day";
   int cont1Freq;
   String outputMsg="Frequency: ";

   
  Serial.println(contOne);
   lcd.print(contOne);
   clearLCD();
   
   Serial.println(cont1FreqQuiz);
   lcd.print(cont1FreqQuiz);
   keypadLoop();
   lcd.clear();

    
    cont1Freq =inputString.toInt();
   
   if (cont1Freq>0 && cont1Freq<=3 ){
   Serial.println();
   Serial.println(outputMsg +cont1Freq);
   lcd.print(outputMsg + cont1Freq);
   clearLCD();


   if(cont1Freq==1){
    Cont1TimeOne();}
    
   else  if(cont1Freq==2){
      Cont1TimeOne();
      Cont1TimeTwo();}
   else if (cont1Freq==3){
    Cont1TimeOne();
    Cont1TimeTwo();
    Cont1TimeThree();}
    }

   else {
    Serial.println();
    Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    Cont1Frequency();}

    
    }
//--------------------------Getting first time---------------------------------------
void Cont1TimeOne(){
  String time1Quiz="Time 1?(24Hrs)";
  long timeOne;
  String outputMsg="Time 1: ";
  
  Serial.println(time1Quiz);
  lcd.print(time1Quiz);
  keypadLoop();
  lcd.clear();

 
 if (inputString.length()==4){
   
   timeOne = inputString.toInt();
   c1Hrs1 = timeOne/100;
   c1Mins1 = timeOne%100;

   if (c1Hrs1>=24 || c1Mins1 >=60 ){
    Serial.println();
    Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    
    Cont1TimeOne();}
   
   else if(c1Hrs1<24 && c1Mins1 <60){
   Serial.println();
   Serial.println(outputMsg + c1Hrs1 + ":" + c1Mins1);
   lcd.print(outputMsg + c1Hrs1 + ":" + c1Mins1);
   clearLCD();
  
   }
  }
  else {
  Serial.println();
  Serial.println(invalidMsg);
  lcd.print(invalidMsg);
  clearLCD();
  
  Cont1TimeOne();
}}

//--------------------------Getting second time -------------------------------------
void Cont1TimeTwo(){
  String time2Quiz="Time 2?(24hrs)";
  long timeTwo;
  String outputMsg="Time 2: ";
  
 Serial.println(time2Quiz);
  lcd.print(time2Quiz);
  keypadLoop();
  lcd.clear();
 
 if (inputString.length()==4){
   
   timeTwo = inputString.toInt();
   c1Hrs2 = timeTwo/100;
   c1Mins2 = timeTwo%100;

   if (c1Hrs2>=24 || c1Mins2 >=60 ){
    Serial.println();
    Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    
    Cont1TimeTwo();}
   
   else if(c1Hrs2<24 && c1Mins2<60){
   Serial.println();
   Serial.println(outputMsg + c1Hrs2 + ":" + c1Mins2);
   lcd.print(outputMsg + c1Hrs2 + ":" + c1Mins2);
   clearLCD();
}}
else {
  Serial.println();
  Serial.println(invalidMsg);
  lcd.print(invalidMsg);
  clearLCD();
  
  Cont1TimeTwo();
}
}

//--------------------------Getting third time---------------------------------------
void Cont1TimeThree(){
   String time3Quiz="Time 3?(24Hrs";
  long timeThree;
  String outputMsg="Time 3: ";
  
  Serial.println(time3Quiz);
  lcd.print(time3Quiz);
  keypadLoop();
  lcd.clear();
 
 if (inputString.length()==4){
   
   timeThree = inputString.toInt();
   c1Hrs3 = timeThree/100;
   c1Mins3 = timeThree%100;

   if (c1Hrs3>=24 || c1Mins3>=60 ){
   Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    Cont1TimeThree();}
   
   else if(c1Hrs3<24 && c1Mins3<60){
   Serial.println();
   Serial.println(outputMsg + c1Hrs3 + ":" + c1Mins3);
   lcd.print(outputMsg + c1Hrs3 + ":" + c1Mins3);
   clearLCD();
}}
else {
  Serial.println();
 Serial.println(invalidMsg);
  lcd.print(invalidMsg);
  clearLCD();
  
  Cont1TimeThree();
}
  
  
  }
 //-------------------------Container 1  Dosage Input Func---------------------------
void Cont1Dosage(){
  String dosageQuiz="input Dosage :";
  String outputMsg="Dosage :";
  
  Serial.println(dosageQuiz);
 lcd.print(dosageQuiz);
  keypadLoop();
  lcd.clear();
  
  c1Dosage=inputString.toInt();

  if(c1Dosage>0 && c1Dosage<=3){
    Serial.println();
    Serial.println(outputMsg + c1Dosage);
    lcd.print(outputMsg + c1Dosage);
    clearLCD();}
    
   else{
    Serial.println();
   Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    
    Cont1Dosage();
   }
  
  }



//---------------------------------------------------------CONTAINER TWO  INPUTS-------------------------------------------------
void ContainerTwo(){
  Serial.println();
  int cont2;
  String cont2Quiz="DISP 2? Press 1";
  String contTwo="DISPENSER 2";
  
 
  Serial.println(cont2Quiz );
  lcd.print(cont2Quiz );
  keypadLoop();
  lcd.clear();
  
  cont2=inputString.toInt();

  if (cont2==1){
  Serial.println();
  Serial.println(contTwo);
  lcd.print(contTwo);
  clearLCD();
  
  Cont2Frequency();
  Cont2Dosage(); }
  }

//--------------------------Frequency of dosage per day------------------------------
void Cont2Frequency()
{  Serial.println();
   
   String cont2FreqQuiz="Input Times/Day";
   int cont2Freq;
   String outputMsg="Frequency: ";

   
  Serial.println(cont2FreqQuiz);
    lcd.print(cont2FreqQuiz);
    keypadLoop();
    lcd.clear();
    
    cont2Freq =inputString.toInt();
   
   if (cont2Freq>0 && cont2Freq<=3 ){
   Serial.println();
    Serial.println(outputMsg + cont2Freq);
   lcd.print(outputMsg + cont2Freq);
   clearLCD();

   
   if(cont2Freq==1){Cont2TimeOne();}
   else if(cont2Freq==2){
      Cont2TimeOne();
      Cont2TimeTwo();}
   else if (cont2Freq==3){
    Cont2TimeOne();
    Cont2TimeTwo();
    Cont2TimeThree();}
    }

   else {
    Serial.println();
    Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    
    Cont2Frequency();}

    
    }
//---------------------------Getting first time--------------------------------------
void Cont2TimeOne(){
  String time1Quiz="Time 1?(24Hrs)";
  long timeOne;
  String outputMsg="Time 1: ";
  
  Serial.println(time1Quiz);
  lcd.print(time1Quiz);
  keypadLoop();
  lcd.clear();

  
 if (inputString.length()==4){
   
   timeOne = inputString.toInt();
   c2Hrs1 = timeOne/100;
   c2Mins1 = timeOne%100;

   if (c2Hrs1>=24 || c2Mins1 >=60 ){
    Serial.println();
    Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    
    Cont2TimeOne();}
   
   else if(c2Hrs1<24 && c2Mins1 <60){
   Serial.println();
   Serial.println(outputMsg + c2Hrs1 + ":" + c2Mins1 );
   lcd.print(outputMsg + c2Hrs1 + ":" + c2Mins1 );
   clearLCD();
  
  
   }
  }
  else {
  Serial.println();
  Serial.println(invalidMsg);
  lcd.print(invalidMsg);
  clearLCD();
  
  Cont2TimeOne();
}}

//----------------------------Getting second time -----------------------------------
void Cont2TimeTwo(){
  String time2Quiz="Time 2(24Hrs)";
  long timeTwo;
  String outputMsg="Time 2: ";
  
  Serial.println(time2Quiz);
  lcd.print(time2Quiz);
  keypadLoop();
  lcd.clear();
 
 if (inputString.length()==4){
   
   timeTwo = inputString.toInt();
   c2Hrs2 = timeTwo/100;
   c2Mins2 = timeTwo%100;

   if (c2Hrs2>=24 || c2Mins2 >=60 ){
    Serial.println();
    Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    
    Cont2TimeTwo();}
   
   else if(c2Hrs2<24 && c2Mins2<60){
   Serial.println();
  Serial.println(outputMsg + c2Hrs2 + ":" + c2Mins2 );
   lcd.print(outputMsg + c2Hrs2 + ":" + c2Mins2);
   clearLCD();
}}
else {
  Serial.println();
  Serial.println(invalidMsg);
  lcd.print(invalidMsg);
  clearLCD();
  
  Cont2TimeTwo();
}
}

//----------------------------Getting time three-------------------------------------
void Cont2TimeThree(){
  String time3Quiz="Time 3(24Hrs)";
  long timeThree;
  String outputMsg="Time 3: ";
  
  Serial.println(time3Quiz);
  lcd.print(time3Quiz);
  keypadLoop();
  lcd.clear();
 
 if (inputString.length()==4){
   
   timeThree = inputString.toInt();
   c2Hrs3 = timeThree/100;
   c2Mins3 = timeThree%100;

   if (c2Hrs3>=24 || c2Mins3 >=60 ){
    Serial.println();
    Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    
    Cont2TimeThree();}
   
   else if(c2Hrs3<24 && c2Mins3<60){
   Serial.println();
    Serial.println(outputMsg + c2Hrs3 + ":" + c2Mins3 );
   lcd.print(outputMsg + c2Hrs3 + ":" + c2Mins3);
   clearLCD();
}}
else {
  Serial.println();
  Serial.println(invalidMsg);
  lcd.print(invalidMsg);
  clearLCD();
  
  Cont2TimeThree();
}
}
 //---------------------------Container Dosage Input Func----------------------------
void Cont2Dosage(){
  String dosageQuiz="input Dosage :";
  String outputMsg="Dosage :";

  
  Serial.println(dosageQuiz);
  lcd.print(dosageQuiz);
  keypadLoop();
  lcd.clear();

  
  c2Dosage=inputString.toInt();

  if(c2Dosage>0 && c2Dosage<=3){
    Serial.println();
   Serial.println(outputMsg + c2Dosage );
    lcd.print(outputMsg + c2Dosage);
    clearLCD();}
   else{
    Serial.println();
    Serial.println(invalidMsg);
    lcd.print(invalidMsg);
    clearLCD();
    
    Cont2Dosage();
   }
  
  }
