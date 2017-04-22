/*
 * Planter control code by Douglas McIlrath
 */

#define LEFT 8
#define RIGHT 9
#define UP 10
#define DOWN 11
#define SELECT 12

#define LIGHT 7
#define PUMP 13

#define SENSOR A0

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(1,2,3,4,5,6);

// Interface Mode
enum Mode {
  idle,
  light_config,
  pump_config,
  light_manual,
  pump_manual
};


void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // pin setup
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(SELECT, INPUT);

  pinMode(LIGHT, OUTPUT);
  pinMode(PUMP, OUTPUT);

  // ensure off at start for safety
  digitalWrite(LIGHT, LOW);
  digitalWrite(PUMP, LOW);
  
}

String format_value(int val, int places, char fill) {
  String formatted = "";
  int digits = floor(log(val));
  for (int i = 0; i < places-digits-1; i++)
    formatted += fill;
  formatted += String(val);
  return formatted;
}

String swap(String s, char c, int place)
{
  String swapped;
  for (int i = 0; i < s.length(); i++) {
    if (s.length() - place - 1 == i)
      swapped += c;
    else
      swapped += s[i];
  }
  return swapped;
}

void getDigits(int val, int * arr) {
  for (int i = 0; i < 4; i++) {
    arr[i] = val % 10;
    val = val / 10;
  }
}

int getProd(int * arr) {
  int sum = 0;
  for (int i = 0; i < 4; i++) {
    sum += arr[i] * pow(10, i);
  }
  return sum;
}

void loop() {
  // If true, pump/light are on regardless of other states.
  bool LIGHT_FORCE_ON = false;
  bool PUMP_FORCE_ON = false;

  // button states
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;
  bool select = false;

  // start on idle screen
  Mode iface_mode = idle;

  // time counter
  int counter = 0;

  // for editing values
  int place = 0;
  String disp;
  int digits[4];

  // daily pump/light variables
  int light = 0;
  int pump = 0;

  for(;;) {
    delay(50);
    counter++;

    // fetch button states
    if (digitalRead(LEFT) == HIGH)
      left = true;
    if (digitalRead(RIGHT) == HIGH)
      right = true;
    if (digitalRead(UP) == HIGH)
      up = true;
    if (digitalRead(DOWN) == HIGH)
      down = true;
    if (digitalRead(SELECT) == HIGH)
      select = true;

    // pump and light code
    if (LIGHT_FORCE_ON)
      digitalWrite(LIGHT, HIGH);
    else {
      if (analogRead(SENSOR) < 900) {
        digitalWrite(LIGHT, HIGH);
      }
      else
        digitalWrite(LIGHT, LOW);
    }
    if (PUMP_FORCE_ON)
      digitalWrite(PUMP, HIGH);
    else {
      /* Pump code here */
      digitalWrite(PUMP, LOW);
    }

    // interface code
    switch(iface_mode) {
      case idle:
        if (digitalRead(SELECT) == LOW && select) {
          iface_mode = light_config;
          place = 1;
          select = false;
          break;
        }
        if (digitalRead(DOWN) == LOW && down) {
          iface_mode = light_manual;
          down = false;
          break;
        }
        // rotating settings display
        lcd.setCursor(0, 0);
        if (counter < 40) {
          lcd.print("Light setting:   ");
          lcd.setCursor(0, 1);
          lcd.print(format_value(light, 2, ' ') + " hours / day   ");
        }
        else if (counter < 80) {
          lcd.print("Pump setting:    ");
          lcd.setCursor(0, 1);
          lcd.print(format_value(pump, 4, ' ') + " mL / day    ");
        }
        else if (counter < 120) {
          lcd.print("Press \"select\"  ");
          lcd.setCursor(0, 1);
          lcd.print("to edit settings ");
        }
        else if (counter < 160) {
          lcd.print("Press \"down\" to");
          lcd.setCursor(0, 1);
          lcd.print("manually control "); 
        }
        else counter = 0;
        break;
        
      case light_config:
        if (digitalRead(SELECT) == LOW && select) {
          select = false;
          iface_mode = pump_config;
          place = 3;
          break;
        }
        if (digitalRead(LEFT) == LOW && left) {
          left = false;
          place = 1;
        }
        if (digitalRead(RIGHT) == LOW && right) {
          right = false;
          place = 0;
        }
        if (digitalRead(UP) == LOW && up) {
          up = false;
          light += pow(10, place);
        }
        if (digitalRead(DOWN) == LOW && down) {
          down = false;
          light -= pow(10, place);
        }
        if (light > 24) light = 24;
        if (light < 0) light = 0;
      
        lcd.setCursor(0, 0);
        disp = format_value(light, 2, '0');
        if ((counter / 10) % 2)
          disp = swap(disp, ' ', place);
        lcd.print(disp + " hours / day   ");
        lcd.setCursor(0, 1);
        if (counter < 40)
          lcd.print("press \"select\" ");
        else if (counter < 80)
          lcd.print("to continue      ");
        else
          counter = 0;
        break;
        
      case pump_config:
        if (digitalRead(SELECT) == LOW && select) {
          select = false;
          iface_mode = idle;
          break;
        }
        if (digitalRead(LEFT) == LOW && left) {
          left = false;
          place++;
          if (place > 3) place = 3;
        }
        if (digitalRead(RIGHT) == LOW && right) {
          right = false;
          place--;
          if (place < 0) place = 0;
        }
        if (digitalRead(UP) == LOW && up) {
          up = false;
          pump += pow(10, place);
        }
        if (digitalRead(DOWN) == LOW && down) {
          down = false;
          pump -= pow(10, place);
        }
        
        if (pump > 9999) pump = 9999;
        if (pump < 0) pump = 0;
      
        lcd.setCursor(0, 0);
        disp = format_value(pump, 4, '0');
        if ((counter / 10) % 2)
          disp = swap(disp, ' ', place);
        lcd.print(disp + " mL / day  ");
        lcd.setCursor(0, 1);
        if (counter < 40)
          lcd.print("press \"select\" ");
        else if (counter < 80)
          lcd.print("to continue      ");
        else
          counter = 0;
        break;
        
      case light_manual:
        if (digitalRead(SELECT) == LOW && select) {
          iface_mode = pump_manual;
          select = false;
          break;
        }
        if (digitalRead(UP) == LOW && up) {
          LIGHT_FORCE_ON = !LIGHT_FORCE_ON;
          up = false;
        }
      
        lcd.setCursor(0, 0);
        disp = "OFF";
        if (LIGHT_FORCE_ON) disp = "ON ";
        lcd.print("Force light: " + disp);
        lcd.setCursor(0, 1);
        lcd.print("\"up\" to toggle  ");
        break;
        
      case pump_manual:
        if (digitalRead(SELECT) == LOW && select) {
          iface_mode = idle;
          select = false;
          break;
        }
        if (digitalRead(UP) == LOW && up) {
          PUMP_FORCE_ON = !PUMP_FORCE_ON;
          up = false;
        }
      
        lcd.setCursor(0, 0);
        disp = "OFF ";
        if (PUMP_FORCE_ON) disp = "ON  ";
        lcd.print("Force pump: " + disp);
        lcd.setCursor(0, 1);
        lcd.print("\"up\" to toggle  ");
        break;
    }
  }
}

