#include <SoftwareSerial.h>
#include <Wire.h>

//=========================================================
//           Global variables from manual input
//=========================================================

SoftwareSerial Bluetooth(10,11);
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



void setup(){
  Serial.begin(19200);
  Bluetooth.begin(19200);
  Wire.begin();
  
  while(!Serial);
  Serial.println("Channels open..");
  
  writeI2C(CTRL_REG1, 0x1F);   //Turn on all axis, disable power down
  writeI2C(CTRL_REG3, 0x08);   //Enable controll readay signal
  writeI2C(CTRL_REG4, 0x80);   //set scale 500 deg / sec
  
  String gyro_done = "Gyro is up and running..";
  delay(100);
  for(int i = 0; i < gyro_done.length(); i++){
    Serial.print(gyro_done.charAt(i));
    delay(100);
  }
  Serial.println();
}

void loop(){
  getInputs();
  getGyroValues();
  char ch = Serial.read();
  if(ch == 'A'){
    Serial.print("    Raw X:   "); Serial.print(x / 114);
    Serial.print("    Raw Y:   "); Serial.print(y / 114);
    Serial.print("    Raw Z:   "); Serial.print(z / 114);
    Serial.println();
  }
}


//=========================================================
//           The gyro setup
//=========================================================
void getGyroValues(){
  byte MSB, LSB;
  
   MSB = readI2C(0x29);
   LSB = readI2C(0x28);
   
   x = ((MSB << 8) | LSB);
   
   MSB = readI2C(0x2B);
   LSB = readI2C(0x2A);
   
   y = ((MSB << 8) | LSB);
   
   MSB = readI2C(0x2D);
   LSB = readI2C(0x2C);
   
   z = ((MSB << 8) | LSB);
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

//=========================================================
//           Read incoming values from controller
//=========================================================
void getInputs(){
  if(Bluetooth.available()){
      char ch = Bluetooth.read();
      if(ch == 'T'){
        tflag = 1;
      } 
      if(ch == 'Y'){
        yflag = 1;
      }
      if(ch == 'P'){
        pflag = 1;
      } 
      if(ch == 'R'){
        rflag = 1;
      }
      
      if(pflag && ch != 'P' && ch != ','){
        in_pitch += ch;
      }
      
      if(rflag && ch != 'R' && ch != ','){
        in_roll += ch;
      }
      
      if(tflag && ch != 'T' && ch != ','){
        in_throttle += ch;
      }
      
      if(yflag && ch != 'Y' && ch != ','){
        in_yaw += ch;
      }
 last_t = throttle;
 last_y = yaw;
 last_p = pitch;
 last_r = roll;
      
      if(ch == ',' && tflag){
        tflag = 0;
        int tmpthrottle = in_throttle.toInt();
        if(abs(tmpthrottle-last_t) != 0){
          throttle = tmpthrottle;
         // Serial.print(last_t);
          //Serial.print("\t");
          //Serial.println(throttle);
        }
        in_throttle = "";
      }
      if(ch == ',' && yflag){
        yflag = 0;
        int tmpyaw = in_yaw.toInt();
        if(abs(tmpyaw-last_y) != 0){
          yaw = tmpyaw;
         // Serial.println(yaw);
        }
        in_yaw = "";
      }
      if(ch == ',' && pflag){
        pflag = 0;
        int tmppitch = in_pitch.toInt();
         if(abs(tmppitch-last_p) != 0){
          pitch = tmppitch;
         // Serial.println(pitch);
        }
        in_pitch = "";
      }
      if(ch == ',' && rflag){
        rflag = 0;
        int tmproll = in_roll.toInt();
        if(abs(tmproll-last_r) != 0){
          roll = tmproll;
          Serial.println(roll);
        }
        in_roll = "";
      }
      
  }
}

