#include "Roku.h"
#include <string>

Roku::Roku() :
  m_Port(0),
  m_sURL("")
{
}

Roku::Roku(IPAddress address, uint16_t port) :
  m_Address(address),
  m_Port(port)
{
  generateURL();
}

bool Roku::queryApps()
{
  WiFiClient client;
  HTTPClient http;
  String req;

  req = m_sURL + "query/apps";
  Serial.println(req);
  if (m_Port && http.begin(client, req))
  {
    int httpCode = http.GET();

    if (httpCode > 0)
    {
      Serial.println(httpCode);
      Serial.println(http.getString());
    }
    else
      Serial.println(http.errorToString(httpCode));

    http.end();
    return true;
  }

  return false;
}

void Roku::update()
{
}

void Roku::generateURL()
{
  m_sURL = "http://" + m_Address.toString() + ":" + String(m_Port) + "/";
}
