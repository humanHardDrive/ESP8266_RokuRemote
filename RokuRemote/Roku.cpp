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
  String rsp = query("query/apps");

  if (rsp.length())
  {
    Serial.println(rsp);
    return true;
  }

  return false;
}

bool Roku::queryActiveApp()
{
  return false;
}

void Roku::update()
{
  /*Keep the current app up to date*/
  if ((millis() - m_nLastActiveQueryTime) > 5000)
  {
    queryActiveApp();
    m_nLastActiveQueryTime = millis();
  }
}

void Roku::generateURL()
{
  m_sURL = "http://" + m_Address.toString() + ":" + String(m_Port) + "/";
}

String Roku::query(String q)
{
  WiFiClient client;
  HTTPClient http;
  String req, sRetVal = "";

  req = m_sURL + q;
  Serial.println(req);
  if (m_Port && http.begin(client, req))
  {
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
      sRetVal = http.getString();

    http.end();
  }

  return sRetVal;
}

bool Roku::post(String p)
{
  return false;
}
