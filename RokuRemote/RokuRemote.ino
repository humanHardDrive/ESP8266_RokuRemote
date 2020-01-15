#include <EEPROM.h>
#include <string>

#include "NetworkHelper.h"
#include "RokuDiscover.h"
#include "Roku.h"

//Access point configuration
IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

//Structure for saving the connection info to be recovred on startup
struct ConnectionInfo
{
  char SSID[32];
  char password[32];

  uint16_t checksum;
};

const char* sHelperNetworkServerName = "networkhelper"; //Name for the DNS
const char* sHelperNetworkSSID = "esp8266"; //AP SSID
const char* sHelperNetworkPassword = ""; //AP Password
//If the password field is empty, no password is used

bool bConnectedToAP = false;

ConnectionInfo savedConnectionInfo;
NetworkHelper helper(sHelperNetworkServerName);

void UpdateConnectionInfo(const char* ssid, const char* password)
{
  strcpy(savedConnectionInfo.SSID, ssid);
  strcpy(savedConnectionInfo.password, password);
  savedConnectionInfo.checksum = CalcConnectionInfoChecksum(&savedConnectionInfo);

  EEPROM.put(0, savedConnectionInfo);
}

bool isSavedInfoValid(ConnectionInfo* info)
{
  //Verify that the stored checksum matches the data
  uint16_t checksum = CalcConnectionInfoChecksum(info);

  return (checksum == info->checksum);
}

uint16_t CalcConnectionInfoChecksum(ConnectionInfo* info)
{
  uint16_t checksum = 0;

  for (uint16_t i = 0; i < sizeof(ConnectionInfo) - sizeof(checksum); i++)
    checksum += ((char*)info)[i];

  return checksum;
}

void Cleanup()
{
  //Stop the network helper
  helper.stop();

  if (bConnectedToAP)
    //Disconnect from the current access point
    WiFi.disconnect();
  else
    //Or stop the current access point
    WiFi.softAPdisconnect (true);

  EEPROM.end();

  //Let the EEPROM commit to flash
  delay(100);
}

void SoftReset()
{
  //Wait for anything to finish with a delay
  delay(1000);
  //Shutdown the server and stop the access point
  Cleanup();

  //Busy loop
  while (1);
  //Caught by the watchdog
  //The first reset after uploading code doesn't work
  //Everyone afterwards does
}

void ResetConnectionInfo(ConnectionInfo* info)
{
  memset(info, 0, sizeof(ConnectionInfo));
  EEPROM.put(0, *info);
}

void setup()
{
  delay(1000);
  
  EEPROM.begin(sizeof(ConnectionInfo));

  Serial.begin(115200);
  Serial.println();

  //Recover connection info
  EEPROM.get(0, savedConnectionInfo);
  WiFi.persistent(false);

#ifndef DONT_REMEMBER
  if (isSavedInfoValid(&savedConnectionInfo) &&
      strlen(savedConnectionInfo.SSID))
  {
    Serial.println("Saved info checksum matches");
    Serial.println("Connecting to...");
    Serial.print("SSID: "); Serial.println(savedConnectionInfo.SSID);
    Serial.print("Password: "); Serial.println(savedConnectionInfo.password);

    WiFi.mode(WIFI_STA);

    if (strlen(savedConnectionInfo.password))
      WiFi.begin(savedConnectionInfo.SSID, savedConnectionInfo.password);
    else
      WiFi.begin(savedConnectionInfo.SSID);

    uint32_t connectionAttemptStart = millis();
    while (WiFi.status() != WL_CONNECTED &&
           (millis() - connectionAttemptStart) < 10000)
    {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      bConnectedToAP = true;

      Serial.println("Connected");
      Serial.print("IP address: "); Serial.println(WiFi.localIP());
    }
  }
  else
    Serial.println("Connection info checksum doesn't match OR SSID not valid");
#endif

  if (!bConnectedToAP)
  {
    Serial.println("Creating AP");
    Serial.print("SSID: ");
    Serial.println(sHelperNetworkSSID);

#ifndef DONT_REMEMBER
    ResetConnectionInfo(&savedConnectionInfo);
#endif

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    if (strlen(sHelperNetworkPassword)) //Password
      WiFi.softAP(sHelperNetworkSSID, sHelperNetworkPassword);
    else //No password
      WiFi.softAP(sHelperNetworkSSID);
  }

  //Setup lambda function to handle network change from helper
  helper.onNetworkChange(
    [](String ssid, String password)
    {
      //Update the connection info in EEPROM
      UpdateConnectionInfo(ssid.c_str(), password.c_str());
      //Use the watchdog to reset the device
      SoftReset();
    });

  //Start the network helper
  helper.start();
}

void loop()
{ 
  helper.background();
}
