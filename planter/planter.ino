#include <RedBot.h>

// Initialize Sensors
RedBotSensor Sensor1 = RedBotSensor(A1);
RedBotSensor Sensor2 = RedBotSensor(A2);
//RedBotSensor Sensor3 = RedBotSensor(A3);

int a;
int on_bool = 0;

#define LIGHT (7) // Light Logic Pin

void setup(){
 pinMode(LIGHT, OUTPUT);  // Set Light Pin to OUT
}

void loop() {
  on_bool = 1;
  a = Sensor1.read();            // Calculate average sunlight from sensors
  a += Sensor2.read();
  //a += Sensor3.read();
  a /= 2;
  if(a < 200 && (on_bool == 1)){            // Check to see if dark enough to turn on light, and if time for sunlight
    digitalWrite(LIGHT, HIGH);
  }
  else{
    digitalWrite(LIGHT, LOW);
  }
}

