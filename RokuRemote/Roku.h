#ifndef __ROKU_H__
#define __ROKU_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <map>

class Roku
{
  public:
    Roku();
    Roku(uint8_t* pIP, uint16_t port);

    void connect(uint8_t* pIP, uint16_t port);

    void update();

    bool isConnected();

  private:
    uint8_t m_IP[4];
    uint16_t m_Port;

    WiFiClient m_Client;
};

#endif
