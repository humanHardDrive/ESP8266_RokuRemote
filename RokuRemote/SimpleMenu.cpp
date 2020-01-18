#include "SimpleMenu.h"

#include <Arduino.h>

SimpleMenu::SimpleMenu(MenuOption* pOptionList) :
  m_pOptionList(pOptionList),
  m_pOptionFn(NULL),
  m_pOptionData(NULL)
{

}

void SimpleMenu::update(char c)
{
  /*Handle the catchall cases first*/
  if (c == '\e') /*Escape*/
  {
    /*Leave the currently executing option*/
    m_pOptionFn = NULL;
    clearScreen();
    showMenu();
  }
  /*Is there a currently executing option?*/
  else if (m_pOptionFn)
  {
    if (!m_pOptionFn(c, m_pOptionData))
      m_pOptionFn = NULL;
  }
  else if (c == '\r') /*Return key*/
  {
    clearScreen();
    showMenu();
  }
  /*Just show the menu*/
  else
    doMenu(c);
}

void SimpleMenu::clearScreen()
{
  Serial.print("\e[2J");
}

void SimpleMenu::doMenu(char c)
{
  MenuOption* pOption = m_pOptionList;

  if (!pOption)
    return;

  while (pOption->k != '\0')
  {
    if (c == pOption->k)
    {
      if (pOption->fn)
      {
        /*Run the option function once*/
        clearScreen();
        Serial.println(pOption->sDisplayString);
        Serial.println();
        
        if (pOption->fn('\0', pOption->pData))
        {
          /*If it returns true, then it needs to be run more*/
          m_pOptionFn = pOption->fn;
          m_pOptionData = pOption->pData;
        }
      }

      return;
    }

    pOption++;
  }
}

void SimpleMenu::showMenu()
{
  MenuOption* pOption = m_pOptionList;

  if (!pOption)
    return;

  while (pOption->k != '\0')
  {
    Serial.print('['); Serial.print(pOption->k); Serial.print("] - ");
    Serial.println(pOption->sDisplayString);

    pOption++;
  }
}
