#ifndef __ROKU_DISCOVER_H__
#define __ROKU_DISCOVER_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define MAX_ROKU_CLIENTS  16

struct RokuInfo
{
  IPAddress address;
  uint16_t port;
  char USN[16];
};

class RokuDiscover
{
  public:
    RokuDiscover();

    void update();

    void startDiscovery(uint32_t timeout);
    void stopDiscovery();
    void doDiscovery();

    uint8_t getNumDiscovered() {
      return m_nDiscoveredRoku;
    }

    RokuInfo* getRokuInfo(uint8_t index) {
      return &m_DiscoveredRoku[index];
    }

    bool isInDiscovery() {
      return m_bInDiscovery;
    }

  private:
    bool responseValid(char* buf, size_t len);
    void getConnectionInfo(char* buf, size_t len, IPAddress* pAddress, uint16_t* pPort);
    void getUSN(char* buf, size_t len, char* usn);

    int strfnd(char* search, char* match, size_t len, size_t offset);

    WiFiUDP m_UDPClient;
    char m_RspBuffer[UDP_TX_PACKET_MAX_SIZE];

    bool m_bInDiscovery;
    uint32_t m_DiscoveryTimeout, m_DiscoveryStarted;

    uint8_t m_nDiscoveredRoku;
    RokuInfo m_DiscoveredRoku[MAX_ROKU_CLIENTS];
};

#endif
