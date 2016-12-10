#include "LCDSimpleMenu.h"

#include <Wire.h>

typedef uint8_t byte;

#define CREATE_ADDITIONAL_CHARS

#ifdef CREATE_ADDITIONAL_CHARS

// Bitmap for a small arrow pointing down
byte ARROW_DOWN_CHAR_BM[8] = {
  B00000,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100,
};

// Bitmap for a small arrow pointing up
byte ARROW_UP_CHAR_BM[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00000,
};

// Bitmap for a small check simbol
byte CHECK_CHAR_BM[8] = {
  B00000,
  B00000,
  B00001,
  B00010,
  B10100,
  B01000,
  B00000,
};
#endif

#define ARROW_UP 1
#define ARROW_DW 2
#define CHECK_CHAR 3

#define MAX_DISPLAY_SIZE  4
#define DEFAULT_MENU_SEP  '.'

LCDSimpleMenu::LCDSimpleMenu(LiquidCrystal_I2C* lcd, MenuConfig* menu, size_t menu_size):
  LCDSimpleMenu(lcd, menu, menu_size, 16, 2)
{
}

LCDSimpleMenu::LCDSimpleMenu(LiquidCrystal_I2C* lcd, MenuConfig* menu, size_t menu_size, uint8_t acols, uint8_t arows)
{
  _lcd = lcd;
  _menu = menu;
  _cols = acols;
  _rows = arows;

  _menu_sep = DEFAULT_MENU_SEP;
  _curr_row = 0;
  _last_row_drawn = -1;
  _menu_size = menu_size;
  _current_root_id = 0;
  _submenu_size = 0;
}

void LCDSimpleMenu::setDisplay(LiquidCrystal_I2C* lcd)
{
  _lcd = lcd;
}

void LCDSimpleMenu::init(bool printids, bool forceredraw)
{
  _print_ids = printids;
  _force_redraw = forceredraw;
  initDisplay();
}

void LCDSimpleMenu::initDisplay()
{
  _lcd->init();

#ifdef CREATE_ADDITIONAL_CHARS
  _lcd->createChar(ARROW_UP, ARROW_UP_CHAR_BM);
  _lcd->createChar(ARROW_DW, ARROW_DOWN_CHAR_BM);
  _lcd->createChar(CHECK_CHAR, CHECK_CHAR_BM);
#endif

  _lcd->backlight();
  _lcd->begin(_cols, _rows);
}

void LCDSimpleMenu::draw()
{
    _lcd->clear();
    drawSubMenu();
}

void LCDSimpleMenu::drawSubMenu()
{
  _last_row_drawn = _curr_row;

  int display_size = _rows;
  byte menu_nav_first = _curr_row == 0 ? byte(' ') : byte(ARROW_UP);
  byte menu_nav_last = _curr_row + display_size - 1 == _submenu_size - 1 ? byte(' ') : byte(ARROW_DW);

  uint8_t rowtoshow = min(display_size, _submenu_size);
  for (uint8_t r = 0; r < rowtoshow; r++)
  {
    uint8_t midx = _submenu[_curr_row + r];
    uint16_t id = _menu[midx].id;
    const char* menu_id = _menu[midx].ids;
    const char* menu_lbl = _menu[midx].label;
    MenuLabel label_func = _menu[midx].dynlabel;

    byte menunav = byte(' ');

    if (r == 0)
      menunav = menu_nav_first;
    else if (r == display_size - 1)
      menunav = menu_nav_last;

    _lcd->setCursor(0, r);
    _lcd->write(menunav);
    if (_print_ids)
      _lcd->print(menu_id);
    else
      _lcd->print(id);
    _lcd->print(_menu_sep);
    if (label_func)
    {
      char* buf = new char[_cols + 1];
      label_func(buf, menu_lbl);
      _lcd->print(buf);
      delete[] buf;
    }
    else
      _lcd->print(menu_lbl);
  }
}

bool LCDSimpleMenu::viewIsChanged()
{
  bool changed = false;
  if (_curr_row != _last_row_drawn)
    changed = true;
  return changed;
}

void LCDSimpleMenu::run()
{
  if (_force_redraw || viewIsChanged())
    draw();
}

void LCDSimpleMenu::navigate(uint16_t parentid)
{
  // select ids of all entries with same parentid
  _curr_menu = parentid;
  _submenu_size = 0;
  for (uint8_t i = 0; i < _menu_size; i++)
    if (_menu[i].parent_id == parentid)
      _submenu[_submenu_size++] = i;

  _last_row_drawn = -1;
  _curr_row = 0;
}

void LCDSimpleMenu::select(int menuid)
{
  // check for bounds on current submenu
  if (menuid <= 0 || menuid > _submenu_size)
  {
    navigate(0);
    return;
  }

  uint8_t midx = _submenu[menuid - 1];
  if (_menu[midx].func == NULL)
  {
    navigate(_menu[midx].id);
  }
  else
  {
    _menu[midx].func(_menu[midx].id);
    // when a dynamic label is selected force a redraw
    // to get updated value
    if (_menu[midx].dynlabel)
      draw();
  }
}

void LCDSimpleMenu::back()
{
  // assume that submenu ids are multiplied by 10
  _curr_menu = _curr_menu / 10;
  navigate(_curr_menu);
}

void LCDSimpleMenu::down()
{
  // increase current row (current row is the first of menu page)
  if (_submenu_size - 1 > _curr_row + (_rows - 1))
    _curr_row++;
}

void LCDSimpleMenu::up()
{
  // decrease current row (current row is the first of menu page)
  if (_curr_row > 0)
    _curr_row--;
}
