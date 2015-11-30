#include <SoftwareSerial.h>


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

int tflag = 0;
int yflag = 0;
int pflag = 0;
int rflag = 0;



void setup(){
  Serial.begin(19200);
  Bluetooth.begin(19200);
  
  while(!Serial);
  Serial.println("Channels open..");
}

void loop(){
  getInputs();
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
      
      if(ch == ',' && tflag){
        tflag = 0;
        throttle = in_throttle.toInt();
        values[0] = throttle;
        in_throttle = "";
      }
      if(ch == ',' && yflag){
        yflag = 0;
        yaw = in_yaw.toInt();
        values[1] = yaw;
        in_yaw = "";
      }
      if(ch == ',' && pflag){
        pflag = 0;
        pitch = in_pitch.toInt();
        values[2] = pitch;
        in_pitch = "";
      }
      if(ch == ',' && rflag){
        rflag = 0;
        roll = in_roll.toInt();
        values[3] = roll;
        in_roll = "";
      }
      Serial.println(values)
  }
}

