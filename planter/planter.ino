

int a;
int on_bool = 0;

#define LIGHT (7) // Light Logic Pin
#define SENSOR (A0)

void setup(){
 pinMode(LIGHT, OUTPUT);  // Set Light Pin to OUT
 //pinMode(SENSOR, INPUT);
}

void loop() {
  on_bool = 1;
  a = analogRead(SENSOR);
  if(a < 900 && (on_bool == 1)){            // Check to see if dark enough to turn on light, and if time for sunlight
    digitalWrite(LIGHT, HIGH);
  }
  else{
    digitalWrite(LIGHT, LOW);
  }
}

