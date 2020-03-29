#include "Roku.h"
#include <string>

std::map<String, uint8_t> Roku::PATH_TO_MSG_ID =
{
  {"apps", 0},
  {"active-app", 1}
};

void pathSplit(char* s, char* sFirst, char* sLast)
{
  uint16_t i = 0;

  while (*s && *s != '/')
  {
    sFirst[i] = *s;

    i++;
    s++;
  }

  sFirst[i] = '\0'; //Append null terminator
  sLast[0] = '\0';

  if (*s == '/')
    strcpy(sLast, s + 1);
}

void XMLCallback(uint8_t statusFlags, char* pTagName, uint16_t tagNameSize, char* pData, uint16_t dataSize)
{
  char first[128], last[128];
  pathSplit(pTagName + 1, first, last);

  Serial.print("Flags: "); Serial.println((int)statusFlags);
  if (statusFlags & STATUS_START_TAG)
  {
    Serial.print(first); Serial.print(' '); Serial.println(last);
  }
  else
    Serial.println(pTagName);

  Serial.println(pData);
  Serial.println();
}

Roku::Roku() :
  m_Port(0),
  m_sURL("")
{
  m_XMLParser.init((uint8_t*)m_XMLBuffer, sizeof(m_XMLBuffer), XMLCallback);
}

Roku::Roku(IPAddress address, uint16_t port) :
  m_Address(address),
  m_Port(port)
{
  generateURL();
}

bool Roku::queryApps()
{
  return query("query/apps");
}

bool Roku::queryActiveApp()
{
  return query("query/active-app");
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

bool Roku::query(String q)
{
  WiFiClient client;
  HTTPClient http;
  String req, sRetVal = "";

  req = m_sURL + q;
  if (m_Port && http.begin(client, req))
  {
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
      sRetVal = http.getString();

    http.end();

    Serial.println(sRetVal);
    for (uint16_t i = 0; i < sRetVal.length(); i++)
      m_XMLParser.processChar(sRetVal.c_str()[i]);

    return true;
  }

  return false;
}

bool Roku::post(String p)
{
  return false;
}
