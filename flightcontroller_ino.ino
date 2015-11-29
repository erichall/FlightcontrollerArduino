#include <SoftwareSerial.h>


SoftwareSerial Bluetooth(10,11);
int throttle;
int yaw;
int roll;
int pitch;

int tflag = 0;
int yflag = 0;
int pflag = 0;
int rflag = 0;

String shit;

void setup(){
  Serial.begin(19200);
  Bluetooth.begin(19200);
  
  while(!Serial);
  Serial.println("Channels open..");
}

void loop(){
  
  if(Bluetooth.available()){
      char ch = Bluetooth.read();
      
       if(ch == 'T')
         tflag = 1;
         
       if(ch == ','){
         tflag = 0;
         Serial.println(throttle);
         throttle = 0;
       }
         
       if(tflag){
         while()
         throttle += ch;
       }
    
  }
}



