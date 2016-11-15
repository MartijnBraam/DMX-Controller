#include <DmxSimple.h>

byte frontBuffer[128] = {128};
byte backBuffer[128] = {128};
int state = 0;
int highestChange=0;

void setup() {
  DmxSimple.usePin(3);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  
  Serial.begin(9600);
  Serial.println("DMX Dongle");
}

void loop() {
  if(state==0){
    if (Serial.available() > 0) {
    
      byte command = Serial.read();
      Serial.print("Got command ");
      Serial.print(command);
      Serial.println("");
      
      if(command==1){
        backBufferSetCommand();
      }
      if(command==2){
        backBufferBulkCommand();
      }
      if(command==3){
        cutCommand();
      }
      if(command==4){
        fadeCommand();
      }
      Serial.println("done");
    }
  }
  
  dmxFullSync();
}

void backBufferSetCommand(){
  int channel = blockingSerialRead();
  int value = blockingSerialRead();
  backBuffer[channel-1]=value;
  Serial.print("Set channel ");
  Serial.print(channel);
  Serial.print(" to ");
  Serial.print(value);
  Serial.println("");
}

void backBufferBulkCommand(){
  int start = blockingSerialRead();
  int count = blockingSerialRead();
  for(int i=start;i<start+count;i++){
    backBuffer[i-1]=blockingSerialRead();
  }
}

void cutCommand(){
  for(int i=0;i<128;i++){
    frontBuffer[i]=backBuffer[i];
  }
  Serial.println("Cut");
}

void fadeCommand(){
  int steps = blockingSerialRead();
  int delayms = blockingSerialRead();

  int delta = 0;
  float stepDelta = 0;
  float stepAbs = 0;
  int newValue = 0;
  
  for(int s=0;s<steps;s++){
    for(int channel=1;channel<=128;channel++){
      delta = backBuffer[channel-1] - frontBuffer[channel-1];
      stepDelta = ((float)delta)/((float)steps);
      newValue = frontBuffer[channel-1] + (int)(stepDelta * s);
      DmxSimple.write(channel, newValue);
    }
    delay(delayms);
  }
  for(int i=0;i<128;i++){
    frontBuffer[i]=backBuffer[i];
  }
  dmxFullSync();
}

void dmxQuickSync(){
  for(int i=1;i<=highestChange;i++){
    DmxSimple.write(i, frontBuffer[i-1]);
  }
}
void dmxFullSync(){
  for(int i=1;i<=128;i++){
    DmxSimple.write(i, frontBuffer[i-1]);
  }
}

int blockingSerialRead(){
  while(true){
    if (Serial.available() > 0) {
      return Serial.read();
    }
    delay(1);
  }
}
