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




Menu menu;
Option back, logMgt, wpf, gainsAdj, pitchAdj, rollAdj, hdgAdj, altAdj, iasAdj;
LiquidCrystal_PCF8574 lcd(LCD_ADDR);
OneButton rotaryBtn = OneButton(ROTARY_BUTTON_PIN, true);
RotaryEncoder encoder(CLK, DT);

int firstRowNum = 0;




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




static void handleRotaryClick()
{
  if (menu.enter())
    renderMenu(0);

  if (menu.fileRead())
  {
    int len;
    loadWpFile(menu.fileContent(len), len);
  }
  
  changeSelection();
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
    {
      errorShown = false;
    }
    
    rotaryBtn.tick();
    encoder.tick();
  
    static int pos = 0;
    int newPos = encoder.getPosition();
    
    if (pos != newPos)
    {
      if (newPos > pos)
      {
        if (menu.down())
          changeSelection();
      }
      
      else if (newPos < pos)
      {
        if (menu.up())
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
