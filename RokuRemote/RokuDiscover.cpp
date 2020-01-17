#include "RokuDiscover.h"

const char* DISCOVERY_REQUEST = "M-SEARCH * HTTP/1.1\r\n"
                                "HOST: 239.255.255.250:1900\r\n"
                                "MAN: \"ssdp:discover\"\r\n"
                                "ST: roku:ecp\r\n\r\n";

RokuDiscover::RokuDiscover() :
  m_bInDiscovery(false),
  m_nDiscoveredRoku(0)
{
  m_UDPClient.begin(1900);
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

  if (packetSize)
  {
    Serial.println("PACKET!");
    int bytesRead = m_UDPClient.read(m_RspBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println(m_RspBuffer);
    responseValid(m_RspBuffer, bytesRead);
  }
}

void RokuDiscover::parseResponse(char* buf, size_t len)
{

}

bool RokuDiscover::responseValid(char* buf, size_t len)
{
  /*Find the HTTP response*/
  int n = strfnd(buf, "HTTP", len, 0);
  if(n >= 0)
  {
    Serial.print("HTTP/");
    Serial.println(atof(buf + n + 5));

    /*Find the http response code*/
    n = strfnd(buf, " ", len, 0);
    int httpCode = atoi(buf + n + 1);

    /*HTTP OK*/
    Serial.print("HTTP ");
    Serial.println(httpCode);
    if(httpCode == 200)
      return true;
  }

  return false;
}

int RokuDiscover::strfnd(char* search, char* match, size_t len, size_t offset)
{
  int strStart = -1;
  size_t i = 0, matchI = 0;

  for (i = offset; i < len; i++)
  {
    if (match[matchI] == '\0')
      return strStart;

    if (search[i] == match[matchI])
    {
      matchI++;
      if (strStart < 0)
        strStart = i;
    }
    else
      matchI = 0;
  }

  return -1;
}
