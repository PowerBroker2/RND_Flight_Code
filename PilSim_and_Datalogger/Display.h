#pragma once

#include "LiquidCrystal_PCF8574.h"
#include "OneButton.h"
#include "RotaryEncoder.h"
#include "Menu.h"
#include "Board.h"
#include "Interface.h"
#include "Log.h"
#include "Sim.h"




const int LCD_ADDR = 0x27;
const int LCD_COLS = 20;
const int LCD_ROWS = 4;
const float MAX_GAIN_DIFF = 10;
const float MIN_GAIN_DIFF = 0.0001;




Menu menu;
Option back, logMgt, wpf, gainsAdj, pitchAdj, rollAdj, hdgAdj, altAdj, iasAdj, p, i, d;
LiquidCrystal_PCF8574 lcd(LCD_ADDR);
OneButton rotaryBtn = OneButton(ROTARY_BUTTON_PIN, true);
RotaryEncoder encoder(CLK, DT);

int firstRowNum = 0;
float gainDiff  = 0;




void renderMenu(int firstRow)
{
  firstRowNum = firstRow;
  
  int col = 1;
  int row = 0;

  lcd.home();
  lcd.clear();
  lcd.setCursor(col, row);
  
  for (int i = firstRow; i < menu.curNumOptions(); i++)
  {
    row++;
    lcd.print(menu.curMenu()->optionName(i));
    lcd.setCursor(col, row);

    if (row >= LCD_ROWS)
      break;
  }
}




void changeSelection()
{
  int curRow = menu.curSelection();
  
  if ((curRow >= (firstRowNum + LCD_ROWS)) || (curRow < firstRowNum))
    renderMenu(curRow);
  
  int col = 0;
  
  for (int row = 0; row < LCD_ROWS; row++)
  {
    lcd.setCursor(col, row);

    if (row == (curRow - firstRowNum))
      lcd.print('>');
    else
      lcd.print(' ');
  }
}




void formatNumber(const float& number, char* buffer, const int& prec, const int& proc)
{
  int startVal = 0;

  if (number < 0)
  {
    startVal++;
    dtostrf(number, prec + proc + 2, proc, buffer);
    buffer[0] = '-';
  }
  else
    dtostrf(number, prec + proc + 1, proc, buffer);
  
  for (int i = startVal; i < (int)strlen(buffer); i++)
  {
    if ((buffer[i] == ' ') || (buffer[i] == '-'))
      buffer[i] = '0';
    else
      break;
  }
}




void updateGainDisp()
{
  char numBuff[LCD_COLS];
  basic_controller* controller = NULL;
  
  if (!strcmp(menu.curMenuName(), pitchAdj.name()))
    controller = &pitchController;
  else if (!strcmp(menu.curMenuName(), rollAdj.name()))
    controller = &rollController;
  else if (!strcmp(menu.curMenuName(), hdgAdj.name()))
    controller = &headingController;
  else if (!strcmp(menu.curMenuName(), altAdj.name()))
    controller = &altitudeController;
  else if (!strcmp(menu.curMenuName(), iasAdj.name()))
    controller = &iasController;
  
  formatNumber(controller->kp, numBuff, 2, 4);
  memcpy(p.name() + 5, numBuff, strlen(numBuff));
  memset(numBuff, 0, strlen(numBuff));
  
  formatNumber(controller->ki, numBuff, 2, 4);
  memcpy(i.name() + 5, numBuff, strlen(numBuff));
  memset(numBuff, 0, strlen(numBuff));
  
  formatNumber(controller->kd, numBuff, 2, 4);
  memcpy(d.name() + 5, numBuff, strlen(numBuff));
  memset(numBuff, 0, strlen(numBuff));
}




void initGainDisp()
{
  char numBuff[LCD_COLS];
  basic_controller* controller = NULL;
  
  if (!strcmp(menu.curOptName(), pitchAdj.name()))
    controller = &pitchController;
  else if (!strcmp(menu.curOptName(), rollAdj.name()))
    controller = &rollController;
  else if (!strcmp(menu.curOptName(), hdgAdj.name()))
    controller = &headingController;
  else if (!strcmp(menu.curOptName(), altAdj.name()))
    controller = &altitudeController;
  else if (!strcmp(menu.curOptName(), iasAdj.name()))
    controller = &iasController;
  
  formatNumber(controller->kp, numBuff, 2, 4);
  memcpy(p.name() + 5, numBuff, strlen(numBuff));
  memset(numBuff, 0, strlen(numBuff));
  
  formatNumber(controller->ki, numBuff, 2, 4);
  memcpy(i.name() + 5, numBuff, strlen(numBuff));
  memset(numBuff, 0, strlen(numBuff));
  
  formatNumber(controller->kd, numBuff, 2, 4);
  memcpy(d.name() + 5, numBuff, strlen(numBuff));
  memset(numBuff, 0, strlen(numBuff));
}




static void handleRotaryClick()
{
  if (menu.enter())
    renderMenu(0);
  else
    renderMenu(firstRowNum);
  
  changeSelection();

  if (menu.fileRead())
  {
    int len;
    loadWpFile(menu.fileContent(len), len);
  }
}




static void handleRotaryLongClick()
{
  if ((!strcmp(menu.curMenuName(), pitchAdj.name()) ||
       !strcmp(menu.curMenuName(), rollAdj.name())  ||
       !strcmp(menu.curMenuName(), hdgAdj.name())   ||
       !strcmp(menu.curMenuName(), altAdj.name()))  &&
       strstr(menu.curOptName(), "->"))
  {
    if (gainDiff < MAX_GAIN_DIFF)
      gainDiff *= 10;
    else
      gainDiff = MIN_GAIN_DIFF;
  }
}




static void handleRotaryDoubleClick()
{
  if (!strcmp(menu.curMenuName(), pitchAdj.name()) ||
      !strcmp(menu.curMenuName(), rollAdj.name())  ||
      !strcmp(menu.curMenuName(), hdgAdj.name())   ||
      !strcmp(menu.curMenuName(), altAdj.name()))
  {
    if (strstr(menu.curOptName(), "->"))
    {
      menu.curOptName()[2] = ' ';
      menu.curOptName()[3] = ' ';
    }
    else
    {
      gainDiff = MIN_GAIN_DIFF;

      menu.curOptName()[2] = '-';
      menu.curOptName()[3] = '>';
    }

    renderMenu(firstRowNum);
    changeSelection();
  }
}




void menuSetup()
{
  menu.setName("main");
  menu.setSD(&sd);
  
  back.setName("back");
  back.setBackOpt(true);

  logMgt.setName("Manage Logs");
  menu.addOption(&logMgt);

  wpf.setName("Select Waypt File");
  wpf.setFileMenu(true);
  logMgt.addOption(&wpf);

  logMgt.addOption(&back);

  gainsAdj.setName("Adjust Gains");
  menu.addOption(&gainsAdj);

  pitchAdj.setName("Pitch Gains");
  gainsAdj.addOption(&pitchAdj);

  rollAdj.setName("Roll Gains");
  gainsAdj.addOption(&rollAdj);

  hdgAdj.setName("Heading Gains");
  gainsAdj.addOption(&hdgAdj);

  altAdj.setName("Altitude Gains");
  gainsAdj.addOption(&altAdj);

  iasAdj.setName("Airspeed Gains");
  gainsAdj.addOption(&iasAdj);

  gainsAdj.addOption(&back);

  p.setName("P    00.0000");
  i.setName("I    00.0000");
  d.setName("D    00.0000");

  pitchAdj.addOption(&p);
  pitchAdj.addOption(&i);
  pitchAdj.addOption(&d);
  pitchAdj.addOption(&back);
  pitchAdj.setCallback(initGainDisp);

  rollAdj.addOption(&p);
  rollAdj.addOption(&i);
  rollAdj.addOption(&d);
  rollAdj.addOption(&back);
  rollAdj.setCallback(initGainDisp);

  hdgAdj.addOption(&p);
  hdgAdj.addOption(&i);
  hdgAdj.addOption(&d);
  hdgAdj.addOption(&back);
  hdgAdj.setCallback(initGainDisp);

  altAdj.addOption(&p);
  altAdj.addOption(&i);
  altAdj.addOption(&d);
  altAdj.addOption(&back);
  altAdj.setCallback(initGainDisp);

  iasAdj.addOption(&p);
  iasAdj.addOption(&i);
  iasAdj.addOption(&d);
  iasAdj.addOption(&back);
  iasAdj.setCallback(initGainDisp);
}




void setupDisplay()
{
  menuSetup();
  
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();

  renderMenu(0);
  changeSelection();
  rotaryBtn.attachClick(handleRotaryClick);
  rotaryBtn.attachLongPressStop(handleRotaryLongClick);
  rotaryBtn.attachDoubleClick(handleRotaryDoubleClick);
}




void displayTick()
{
  static mode prevBoardMode = boardMode;
  static bool errorShown = false;
  
  if ((boardMode == PILSIM_INACTIVE) || (boardMode == PILSIM_ACTIVE))
  {
    if (boardMode != prevBoardMode)
    {
      if ((boardMode == PILSIM_ACTIVE) && wpFileSelected)
      {
        renderMenu(firstRowNum);
        changeSelection();
      }
      else if (boardMode == PILSIM_INACTIVE)
      {
        renderMenu(firstRowNum);
        changeSelection();
      }
    }

    if (boardMode == PILSIM_ACTIVE)
    {
      if (!wpFileSelected && !errorShown)
      {
        lcd.home();
        lcd.clear();
        lcd.print("WP File Not Selected");

        errorShown = true;
        
        return;
      }

      if (wpFileSelected)
        simTick();
    }
    else
      errorShown = false;
    
    rotaryBtn.tick();
    encoder.tick();
  
    static int pos = 0;
    int newPos = encoder.getPosition();
    
    if (pos != newPos)
    {
      if (newPos > pos)
      {
        if ((!strcmp(menu.curMenuName(), pitchAdj.name()) ||
             !strcmp(menu.curMenuName(), rollAdj.name())  ||
             !strcmp(menu.curMenuName(), hdgAdj.name())   ||
             !strcmp(menu.curMenuName(), altAdj.name()))  &&
             strstr(menu.curOptName(), "->"))
        {
          basic_controller* controller = NULL;
          
          if (!strcmp(menu.curMenuName(), pitchAdj.name()))
            controller = &pitchController;
          else if (!strcmp(menu.curMenuName(), rollAdj.name()))
            controller = &rollController;
          else if (!strcmp(menu.curMenuName(), hdgAdj.name()))
            controller = &headingController;
          else if (!strcmp(menu.curMenuName(), altAdj.name()))
            controller = &altitudeController;
          else if (!strcmp(menu.curMenuName(), iasAdj.name()))
            controller = &iasController;

          if (strstr(menu.curOptName(), "P -> "))
            controller->kp -= gainDiff;
          else if (strstr(menu.curOptName(), "I -> "))
            controller->ki -= gainDiff;
          else if (strstr(menu.curOptName(), "D -> "))
            controller->kd -= gainDiff;

          updateGainDisp();
          renderMenu(firstRowNum);
          changeSelection();
        }
        else if (menu.down())
          changeSelection();
      }
      else if (newPos < pos)
      {
        if ((!strcmp(menu.curMenuName(), pitchAdj.name()) ||
             !strcmp(menu.curMenuName(), rollAdj.name())  ||
             !strcmp(menu.curMenuName(), hdgAdj.name())   ||
             !strcmp(menu.curMenuName(), altAdj.name()))  &&
             strstr(menu.curOptName(), "->"))
        {
          basic_controller* controller = NULL;
          
          if (!strcmp(menu.curMenuName(), pitchAdj.name()))
            controller = &pitchController;
          else if (!strcmp(menu.curMenuName(), rollAdj.name()))
            controller = &rollController;
          else if (!strcmp(menu.curMenuName(), hdgAdj.name()))
            controller = &headingController;
          else if (!strcmp(menu.curMenuName(), altAdj.name()))
            controller = &altitudeController;
          else if (!strcmp(menu.curMenuName(), iasAdj.name()))
            controller = &iasController;

          if (strstr(menu.curOptName(), "P -> "))
            controller->kp += gainDiff;
          else if (strstr(menu.curOptName(), "I -> "))
            controller->ki += gainDiff;
          else if (strstr(menu.curOptName(), "D -> "))
            controller->kd += gainDiff;

          updateGainDisp();
          renderMenu(firstRowNum);
          changeSelection();
        }
        else if (menu.up())
          changeSelection();
      }
      
      pos = newPos;
    }
  }
  else if (boardMode == LOG)
  {
    if (boardMode != prevBoardMode)
    {
      lcd.home();
      lcd.clear();
      lcd.print("Logging Active");
    }
  }

  prevBoardMode = boardMode;
}
