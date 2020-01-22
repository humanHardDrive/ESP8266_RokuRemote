#ifndef __ROKU_H__
#define __ROKU_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <map>

class Roku
{
  public:
    Roku();
    Roku(IPAddress address, uint16_t port);

    void connect();
    void connect(IPAddress address, uint16_t port);
    void disconnect();

    void update();

    bool isConnected() {
      return m_Client.connected();
    }

  private:
    IPAddress m_Address;
    uint16_t m_Port;

    WiFiClient m_Client;
};

#endif
