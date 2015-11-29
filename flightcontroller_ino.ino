#include <SoftwareSerial.h>


SoftwareSerial Bluetooth(10,11);
String throttle;
String yaw;
String roll;
String pitch;

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
        pitch += ch;
      }
      
      if(rflag && ch != 'R' && ch != ','){
        pitch += ch;
      }
      
      if(tflag && ch != 'T' && ch != ','){
        throttle += ch;
      }
      
      if(yflag && ch != 'Y' && ch != ','){
        yaw += ch;
      }
      
      if(ch == ',' && tflag){
        tflag = 0;
        Serial.println(throttle);
        throttle = "";
      }
      if(ch == ',' && yflag){
        yflag = 0;
        Serial.println(yaw);
        yaw = "";
      }
      if(ch == ',' && pflag){
        pflag = 0;
        Serial.println(pitch);
        pitch = "";
      }
      if(ch == ',' && rflag){
        rflag = 0;
        //Serial.println(roll);
        roll = "";
      }
  }
}

