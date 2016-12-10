/*
  Basic
  Setup a very basic menu
  
  This example code is in the public domain.

  created 10 Dec 2016
  by Adriano Prenleloup
*/

#include <LiquidCrystal_I2C.h>
#include <LCDMiniMenu.h>

LiquidCrystal_I2C lcd(0x3f, 16, 2);

MenuConfig menu_cfg[] = {
  {0, 1, "1", "Hello", NULL, NULL},
  {0, 2, "2", "World!", NULL, NULL}
};

LCDMiniMenu menu(&lcd, menu_cfg, 2);

void setup() {
  menu.init(true, false);
  //menu.navigate(0);
  menu.draw();
}

void loop() {

}
