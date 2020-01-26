#ifndef __ROKU_H__
#define __ROKU_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include <map>

class Roku
{
  public:
    Roku();
    Roku(IPAddress address, uint16_t port);

    void setIP(IPAddress address) {
      m_Address = address;
      generateURL();
    }
    void setPort(uint16_t port) {
      m_Port = port;
      generateURL();
    }

    void update();

    bool queryApps();

  private:
    void generateURL();
  
    IPAddress m_Address;
    uint16_t m_Port;
    String m_sURL;
};

#endif
