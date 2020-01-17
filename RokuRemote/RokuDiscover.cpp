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
    int bytesRead = m_UDPClient.read(m_RspBuffer, UDP_TX_PACKET_MAX_SIZE);

    if (responseValid(m_RspBuffer, bytesRead))
    {
      RokuInfo rokuInfo;
      getConnectionInfo(m_RspBuffer, bytesRead, &rokuInfo.address, &rokuInfo.port);
      getUSN(m_RspBuffer, bytesRead, rokuInfo.USN);
    }
  }
}

bool RokuDiscover::responseValid(char* buf, size_t len)
{
  /*Find the HTTP response*/
  int n = strfnd(buf, "HTTP", len, 0);
  if (n >= 0)
  {
    /*Find the http response code*/
    n = strfnd(buf, " ", len, 0);
    int httpCode = atoi(buf + n + 1);

    /*HTTP OK*/
    if (httpCode == 200)
      return true;
  }

  return false;
}

void RokuDiscover::getConnectionInfo(char* buf, size_t len, IPAddress* pAddress, uint16_t* pPort)
{
  /*Find the location info*/
  int n = strfnd(buf, "LOCATION:", len, 0);
  if (n >= 0)
  {
    int ipBuf[4];
    int portBuf;
    sscanf(buf + n, "LOCATION: http://%d.%d.%d.%d:%d", &ipBuf[0], &ipBuf[1], &ipBuf[2], &ipBuf[3], &portBuf);

    *pAddress = IPAddress(ipBuf[0], ipBuf[1], ipBuf[2], ipBuf[3]);
    *pPort = portBuf;
  }
}

void RokuDiscover::getUSN(char* buf, size_t len, char* usn)
{
  /*Find the USN info*/
  int n = strfnd(buf, "USN:", len, 0);
  if(n >= 0)
    sscanf(buf + n, "USN: uuid:roku:ecp:%s", usn);
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
