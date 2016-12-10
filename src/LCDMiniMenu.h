#ifndef LCDMINIMENU_H
#define LCDMINIMENU_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#if (ARDUINO >= 100)
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

typedef void (*MenuLabel)(char*, const char*);
typedef void (*MenuFunction)(int);

typedef struct {
    uint16_t parent_id;   //id of parent menu
    uint16_t id;          //id of menu entry
    const char* ids;      //id to be printed
    const char* label;    //menu entry label
    MenuLabel dynlabel;   //label rendering function
    MenuFunction func;    //function to invoke on entry selection
} MenuConfig;

class LCDMiniMenu
{
  public:
    LCDMiniMenu(LiquidCrystal_I2C* lcd, MenuConfig* menu, size_t menu_size);
    LCDMiniMenu(LiquidCrystal_I2C* lcd, MenuConfig* menu, size_t menu_size, uint8_t acols, uint8_t arows);

    void back();
    void down();
    void draw();
    void init(bool printdir, bool forceredraw);
    void navigate(uint16_t parentid);
    void run();
    void select(int menuid);
    void up();
    bool viewIsChanged();

  private:
    LiquidCrystal_I2C * _lcd;
    MenuConfig* _menu;
    uint8_t _submenu[10];
    uint8_t _submenu_size;
    uint8_t _rows;
    uint8_t _cols;
    uint8_t _menu_size;
    uint8_t _curr_row;
    uint8_t _last_row_drawn;
    bool _print_ids;
    bool _force_redraw;
    char _menu_sep;
    uint16_t _current_root_id;
    uint16_t _curr_menu;

    void initDisplay();
    void setDisplay(LiquidCrystal_I2C* lcd);
    void drawSubMenu();
};

#endif // LCDMINIMENU_H
