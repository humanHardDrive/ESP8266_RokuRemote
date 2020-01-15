#include "RokuDiscover.h"

const char* DISCOVERY_REQUEST = "M-SEARCH * HTTP/1.1\r\n"
                                "Host: 239.255.255.250:1900\r\n"
                                "Man: \"ssdp:discover\"\r\n"
                                "ST: roku:ecp\r\n\r\n";

RokuDiscover::RokuDiscover() :
  m_bInDiscovery(false),
  m_nDiscoveredRoku(0)
{

}

void RokuDiscover::update()
{
  if (m_bInDiscovery)
  {
    doDiscovery();

    if ((millis() - m_DiscoveryStarted) > m_DiscoveryTimeout)
      m_bInDiscovery = false;
  }
}

void RokuDiscover::startDiscovery(uint32_t timeout)
{
  m_nDiscoveredRoku = 0;
  m_DiscoveryTimeout = timeout;

  m_UDPClient.beginPacket(IPAddress(239, 255, 255, 250), 1900);
  m_UDPClient.write(DISCOVERY_REQUEST);
  m_UDPClient.endPacket();

  m_DiscoveryStarted = millis();
  m_bInDiscovery = true;
}

void RokuDiscover::stopDiscovery()
{
  m_bInDiscovery = false;
}

void RokuDiscover::doDiscovery()
{
  int packetSize = m_UDPClient.parsePacket();

  if(packetSize)
  {
    m_UDPClient.read(m_RspBuffer, UDP_TX_PACKET_MAX_SIZE);
  }
}
