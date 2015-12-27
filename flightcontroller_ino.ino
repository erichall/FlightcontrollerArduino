#include <SPI.h>

#include <SoftwareSerial.h>
#include <Wire.h>
#include <Servo.h>

const int ledPin = 6;
int ledState = LOW;
unsigned long prevMillis = 0;
const long interval = 1000;

//=========================================================
//           Global variables from manual input
//=========================================================

SoftwareSerial Bluetooth(11,10);
String in_throttle;
String in_yaw;
String in_roll;
String in_pitch;

int values[6];
int throttle;
int yaw;
int roll;
int pitch;

int last_t, last_y, last_r, last_p;

int tflag = 0;
int yflag = 0;
int pflag = 0;
int rflag = 0;

//=========================================================
//           Globals for the gyro
//=========================================================

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23

int Addr = 105; //I2C adress of the gyro..

int x,y,z;
int cal_int;
int gyro_roll_cal, gyro_pitch_cal, gyro_yaw_cal;
int gyro_roll, gyro_pitch, gyro_yaw;

//=========================================================
//           Globals for Motors
//=========================================================
Servo M1;
Servo M2;
Servo M3;
Servo M4;

boolean flightState = false;

//=========================================================
//           Interrupt globals
//=========================================================
byte last_channel;
int receiver_input_channel;
unsigned long timer;




void setup(){
 // PCICR |= (1 << PCIE0);
 // PCMSK0 |= (1 << PCINT2); //digital input 10, trigger intrrupt on change
  
  Serial.begin(19200);
  Bluetooth.begin(19200);
  Wire.begin();
  
  while(!Serial);
  Serial.println("Channels open..");
  pinMode(ledPin,OUTPUT);
  digitalWrite(6,HIGH);
  
  writeI2C(CTRL_REG1, 0x1F);   //Turn on all axis, disable power down
  writeI2C(CTRL_REG3, 0x08);   //Enable controll readay signal
  writeI2C(CTRL_REG4, 0x80);   //set scale 500 deg / sec
  
  String gyro_done = "Gyro is up and running..";
  delay(100);
  for(int i = 0; i < gyro_done.length(); i++){
    Serial.print(gyro_done.charAt(i));
    delay(100);
  }
  Serial.println("Calibrating gyro offset.");
  gyrocal();
  
  
  
  String arming = "Arming motors..";
  delay(100);
  
  M1.attach(2);
  M2.attach(8);
  M3.attach(4);
  M4.attach(5);
  
  for(int i = 0; i < arming.length(); i++){
    Serial.print(arming.charAt(i));
    delay(200);
  }
  Serial.println("Arming done");
  
  while(throttle > 10) {
    Serial.println("Please move throttle stick to zero");
  }
  
  flightState = true;
  
  
}

void loop(){
  getInputs();
  getGyroValues();


  if(flightState){
    motorWrite(throttle);
  }
  print_signals();
  
}


//=========================================================
//           Motor
//=========================================================
void motorWrite(int input){
         M1.writeMicroseconds(input);
         M2.writeMicroseconds(input + 381); // +381..
         M3.writeMicroseconds(input);
         M4.writeMicroseconds(input);
}


//=========================================================
//           The gyro setup
//=========================================================
void getGyroValues(){
  byte MSB, LSB;
  
   MSB = readI2C(0x29);
   LSB = readI2C(0x28);
   
   gyro_roll = ((MSB << 8) | LSB);
   
   //remove the drift offset from gyroval
   if(cal_int == 2000){
     gyro_roll -= gyro_roll_cal;
   }
   
   MSB = readI2C(0x2B);
   LSB = readI2C(0x2A);
   
   gyro_pitch = ((MSB << 8) | LSB);
   if(cal_int == 2000){
     gyro_pitch -= gyro_pitch_cal;
   }
   MSB = readI2C(0x2D);
   LSB = readI2C(0x2C);
   
   gyro_yaw = ((MSB << 8) | LSB);
   if(cal_int == 2000){
     gyro_yaw -= gyro_yaw_cal;
   }
}

int readI2C (byte regAddr){
  Wire.beginTransmission(Addr);
  Wire.write(regAddr);           //Write to register
  Wire.endTransmission();        //Terminate equest
  Wire.requestFrom(Addr, 1);    //Read a byte
  while(!Wire.available()){};    //Wait to receive
  return (Wire.read());
}

void writeI2C(byte regAddr, byte val){
  Wire.beginTransmission(Addr);
  Wire.write(regAddr);
  Wire.write(val);
  Wire.endTransmission();
  }


void print_signals() {
  Serial.print(throttle);
  Serial.print("\t");
  Serial.print(yaw);
  Serial.print("\t");
  Serial.print(pitch);
  Serial.print("\t");
  Serial.print(roll);
  Serial.print("\t");
  Serial.print(gyro_roll);
  Serial.print("\t");
  Serial.print(gyro_pitch);
  Serial.print("\t");
  Serial.print(gyro_yaw);
  Serial.println();
    
}

void blink(){
  unsigned long currentMilli = millis();  
  
  if(currentMilli - prevMillis >= interval) {
    prevMillis = currentMilli;
    
    if(ledState == HIGH){
      ledState = LOW;
    }else {
      ledState = HIGH;
    }
    
    digitalWrite(ledPin, ledState);
  }
}

void gyrocal(){
   for(cal_int = 0; cal_int < 2000; cal_int++){
     
     blink();
       getGyroValues();
       
 
       gyro_roll_cal +=gyro_roll;
       
       gyro_pitch_cal += gyro_pitch; 
       
       gyro_yaw_cal += gyro_yaw;
       
       delay(3);
       

   } 
   gyro_roll_cal /= 2000;
   gyro_pitch_cal /= 2000;
   gyro_yaw_cal /= 2000;
}

//=========================================================
//           Read incoming values from controller
//=========================================================
void getInputs(){
  if(Bluetooth.available()){
      char ch = Bluetooth.read();
  
      if(ch == 'T'){
        tflag = 1;
      } 
      else if(ch == 'Y'){
        yflag = 1;
      }
      else if(ch == 'P'){
        pflag = 1;
      } 
      else if(ch == 'R'){
        rflag = 1;
      }
   
      if(tflag){
          char val = Bluetooth.read();
          while(val != ','){
            in_throttle += val;
            val = Bluetooth.read();
          }
         // Serial.println(in_throttle);
          throttle = in_throttle.toInt();
          in_throttle = "";
          tflag = 0;
      }else if(yflag){
          char val = Bluetooth.read();
          while(val != ','){
            in_yaw += val;
            val = Bluetooth.read();
          }
          //Serial.println(in_yaw);
          yaw = in_yaw.toInt();
          in_yaw = "";
          yflag = 0;
      }else if(rflag){
          char val = Bluetooth.read();
          while(val != ','){
            in_roll += val;
            val = Bluetooth.read();
          }
          //Serial.println(in_roll);
          roll = in_roll.toInt();
          in_roll = "";
          rflag = 0;
      }else if(pflag){
          char val = Bluetooth.read();
          while(val != ','){
            in_pitch += val;
            val = Bluetooth.read();
          }
          //Serial.println(in_pitch);
          pitch = in_pitch.toInt();
          in_pitch = "";
          pflag = 0;
      }
      
      
      
          
  }
  
}

