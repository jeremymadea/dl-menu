#include "DLMenu.h"

///
/// FloatMenuItem
///
DLFloatMenuItem::DLFloatMenuItem(LiquidCrystal *lcd, const char *label, int address, int d) : DLMenuItem(lcd, label, address){
  // prepare stuff
  n = new DLNumber(address);
  n->getUintValue();
  
  digits = d;
  sections = digits+1; // all digits plus exponent
  numerals = new char[digits+1]; // to store 'exploded' digits from a float
  explode();
}

void DLFloatMenuItem::explode(void){
  float f = n->getFloatValue();
  
  // exponent first
  exponent = (int)floor(log10(fabs(f)));

  if(exponent > 9 || exponent < -9) exponent = 0;

  // convert the float to d.ddd...
  f = f / (float)pow(10, exponent);
  
  for(int i = 0; i < digits; i++){
    numerals[i] = (int)floor(f);
    f = f - floor(f);
    f = f*10.;
  }
}

void DLFloatMenuItem::printNumber(void){
  lcd->setCursor(0, 1);

  // first character: plus or minus sign
  if(numerals[0] >= 0) lcd->print("+");
  else lcd->print("-");
  
  lcd->print(abs(numerals[0]));
  
  lcd->print(".");

  for(int i = 1; i < digits; i++){
    lcd->print((int)numerals[i]);
  }

  lcd->print("e");

  if(exponent >= 0) lcd->print("+");
  else lcd->print("-");

  lcd->print(abs((int)exponent));
}

void DLFloatMenuItem::placeCursor(void){
  int pos;
  // set cursor on the right spot:
  // 0123456789ABCDEF  < lcd column
  //  0 1234  5  < section
  // +1.3568e+2
  if(s == 0) pos = 1;
  else if(s == sections-1) pos = s + 4;
  else pos = s + 2;

  lcd->setCursor(pos, 1);
  lcd->cursor();
}

void DLFloatMenuItem::show(bool endFirst){
  // set the currently displayed section
  // print the label
  lcd->clear();
  progmem_to_lcd(lcd, 0, label);

  // print the number
  printNumber();

  // set cursor position
  if(endFirst) s = sections-1;
  else s = 0;

  placeCursor();
}

void DLFloatMenuItem::add(int i){
  if(s == 0) numerals[s] = constrain(numerals[s] + i, -9, 9);
  else if(s > 0 && s < sections-1) numerals[s] = constrain(numerals[s] + i, 0, 9);
  else exponent = constrain(exponent + i, -9, 9);

  printNumber();
  placeCursor();
}

void DLFloatMenuItem::increase(void){ add(1); };
void DLFloatMenuItem::decrease(void){ add(-1); };

void DLFloatMenuItem::hide(void){
  // assemble a float from numerals and exponent and save the Number
  float f = 0;
  int i;

  for(i = 0; i < digits; i++){
    f += (float)numerals[i] * powf(10, -i);
  }

  f = f * powf(10, exponent);

  if(n->getFloatValue() != f) n->setValue(f);
  
  lcd->clear();
}

void DLFloatMenuItem::setValue(float value){
  n->setValue(value);
  explode();
}
