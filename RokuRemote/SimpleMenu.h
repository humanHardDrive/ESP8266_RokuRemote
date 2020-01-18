#ifndef __SIMPLE_MENU_H__
#define __SIMPLE_MENU_H__

struct MenuOption
{
  char k;
  char* sDisplayString;

  bool (*fn)(char, void*);
  void* pData;
};

class SimpleMenu
{
  public:
    SimpleMenu(MenuOption* pOptionList);

    void update(char c);

    void showMenu();

  private:
    void clearScreen();
    void doMenu(char c);

    MenuOption* m_pOptionList;
    bool (*m_pOptionFn)(char, void*);
    void* m_pOptionData;
};

#endif
