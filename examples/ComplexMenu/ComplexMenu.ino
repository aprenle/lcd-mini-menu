#include "Arduino.h"

#include <Key.h>
#include <Keypad.h>

/// Gestione tastiera
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
    {'1','2','3', 'A'},
    {'4','5','6', 'B'},
    {'7','8','9', 'C'},
    {'*','0','#', 'D'}
};

byte rowPins[ROWS] = {46, 44, 42, 40}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {47, 45, 43, 41}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


//#include <LiquidCrystal_I2C.h>
#include <LCDSimpleMenu.h>

LiquidCrystal_I2C lcd(0x3f, 16, 2);

void funzionemenu(int funzid);
void toggleonoff(int funzid);
void incvalue(int funzid);

void showOnOff(char* dst, const char* paramname);
void showIntValue(char* dst, const char* paramname);

#define MENU_SIZE 11

MenuConfig mymenu[] = {
  {0, 1, "1 ", "submenu 1", NULL, NULL},
      {   1, 11, "A1", "Funz a (1.1)", NULL, &funzionemenu},
      {   1, 12, "A2", "Funz b (1.2)", NULL, &funzionemenu},
      {   1, 13, "A3", "Funz c (1.3)", NULL, &funzionemenu},
  {0, 2, "2 ", "submenu 2", NULL, NULL},
      {   2, 21, "B1", "Funz 2.1", NULL, &funzionemenu},
      {   2, 22, "B2", "Funz 2.2", NULL, &funzionemenu},
  {0, 3, "3 ", "submenu 3", NULL, NULL},
      {   3, 31, "C1", "Funz 3.1", NULL, &funzionemenu},
  {0, 4, "4 ", "Funz4    ", &showOnOff, &toggleonoff},
  {0, 5, "5 ", "Funz5   ", &showIntValue, &incvalue},
};
LCDSimpleMenu menu_m(&lcd, mymenu, MENU_SIZE);

bool fstarted = false;
unsigned long tstarted = 0;

void funzionemenu(int funzid)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selez: f");
  lcd.print(funzid);
  lcd.setCursor(0, 1);
  lcd.print("Eseguo funz...");
  fstarted = true;
  tstarted = millis();
}

bool onoffvalue = true;
int intvalue = 1;

void toggleonoff(int funzid)
{
    onoffvalue = !onoffvalue;
}

void incvalue(int funzid)
{
  intvalue++;
  if (intvalue == 11)
    intvalue = 1;
}

void showOnOff(char* dst, const char* paramname)
{
//    sprintf(dst, "%s[%s]", paramname, onoffvalue ? "ON " : "OFF");
    sprintf(dst, "%s[%c]", paramname, onoffvalue ? '\3' : ' ');
}

void showIntValue(char* dst, const char* paramname)
{
  sprintf(dst, "%s[%2d]", paramname, intvalue);
}

#define PIN_LED 13

void setup()
{
  pinMode(PIN_LED, OUTPUT);

  //menu_m.setDisplay(&lcd);
  menu_m.init(true, false);
  lcd.print("Starting");
  //menu_m.draw();
  menu_m.navigate(0);

}


bool ledon = true;

void switch_led()
{
  digitalWrite(PIN_LED, ledon ? HIGH : LOW );
  ledon = !ledon;
}

int i = 0;
int scrolls_cnt = 1;
bool dir = true;

void scroll_menu(bool dir)
{
   if (dir) menu_m.down();
   else menu_m.up();
}

void dummy_action()
{
  // scroll menu up and down and blink led
  i++;
  delay(100);
  if (i > 10)
  {
    i = 0;
    scrolls_cnt = scrolls_cnt % 3 + 1;
    if (scrolls_cnt == 1)
      dir = !dir;
    switch_led();
    scroll_menu(dir);
  }
}

void read_keys()
{
  // do nothing
  char key = keypad.getKey();

  if (key)
  {
    Serial.println(key);
    switch (key) {
      case 'A': menu_m.up(); break;
      case 'D': menu_m.down(); break;
      case 'B': menu_m.back(); break;
      case '1': case '2': case '3':
      case '4': case '5': case '6':
      case '7': case '8': case '9':
        menu_m.select(key - 0x30);
        break;
    }
  }
}

void loop()
{
  if (fstarted)
  {
    if (millis() > tstarted + 10000)
    {
      lcd.clear();
      lcd.print("Completa");
      delay(700);
      fstarted = false;
      ledon = false;
      menu_m.navigate(0);
    }
    delay(50);
    switch_led();
  }
  // perform some dummy actions like scroll menu
  //dummy_action();
  //do something like process keyboard events
  read_keys();
  // eventually redraw menu
  menu_m.run();
}
