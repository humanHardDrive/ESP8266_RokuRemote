#include <EEPROM.h>
#include <string>

#include "NetworkHelper.h"
#include "RokuDiscover.h"
#include "Roku.h"

//Access point configuration
IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

/*Data stored in flash separate from the checksum*/
struct SAVE_DATA_DATA
{
  char SSID[32];
  char password[32];

  byte rokuIP[4];
  uint16_t rokuPort;
};

/*This separates the data from the checksum to avoid magic
  nubmers during calculation*/
struct SAVE_DATA
{
  SAVE_DATA_DATA data;

  uint16_t checksum;
};

const char* sHelperNetworkServerName = "networkhelper"; //Name for the DNS
const char* sHelperNetworkSSID = "esp8266"; //AP SSID
const char* sHelperNetworkPassword = ""; //AP Password
//If the password field is empty, no password is used

bool bConnectedToAP = false;

SAVE_DATA saveData, saveDataMirror;
NetworkHelper helper(sHelperNetworkServerName);
RokuDiscover discoverer;
Roku roku;

uint16_t calcDataChecksum(SAVE_DATA* pData)
{
  uint16_t checksum = 0;

  for (size_t i = 0; i < sizeof(SAVE_DATA_DATA); i++)
    checksum += ((uint8_t*)&pData->data)[i];

  return checksum;
}

void save()
{
  if (memcmp(&saveData, &saveDataMirror, sizeof(SAVE_DATA)))
  {
    Serial.println("Saving");
    saveData.checksum = calcDataChecksum(&saveData);

    EEPROM.put(0, saveData);
    memcpy(&saveDataMirror, &saveData, sizeof(SAVE_DATA));
  }
  else
    Serial.println("Data not different");
}

bool recoverSavedData()
{
  EEPROM.get(0, saveDataMirror);

  if (calcDataChecksum(&saveDataMirror) == saveDataMirror.checksum)
  {
    Serial.println("Checksums match");
    memcpy(&saveData, &saveDataMirror, sizeof(SAVE_DATA));
    return true;
  }
  else
    Serial.println("Invalid checksum");

  return false;
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

void setup()
{
  delay(1000);

  EEPROM.begin(sizeof(SAVE_DATA));

  Serial.begin(115200);
  Serial.println();

  //Recover connection info
  WiFi.persistent(false);

  bool bRecovered = recoverSavedData();
  if (bRecovered && saveData.data.SSID[0])
  {
    Serial.println("Saved info checksum matches");
    Serial.println("Connecting to...");
    Serial.print("SSID: "); Serial.println(saveData.data.SSID);
    Serial.print("Password: "); Serial.println(saveData.data.password);

    WiFi.mode(WIFI_STA);

    if (saveData.data.password[0])
      WiFi.begin(saveData.data.SSID, saveData.data.password);
    else
      WiFi.begin(saveData.data.SSID);

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

      if(saveData.data.rokuPort)
      {
        IPAddress address(*((uint32_t*)saveData.data.rokuIP));
        Serial.print("Connecting to roku "); Serial.print(address); Serial.print(':'); Serial.println(saveData.data.rokuPort);
        roku.setIP(address);
        roku.setPort(saveData.data.rokuPort);

        roku.queryApps();
      }
      else
        Serial.println("No Roku saved");
    }
  }
  else if(bRecovered)
    Serial.println("SSID is emtpy");
  else
    Serial.println("Failed to recover data from flash");

  if (!bConnectedToAP)
  {
    Serial.println("Creating AP");
    Serial.print("SSID: ");
    Serial.println(sHelperNetworkSSID);

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
    strcpy(saveData.data.SSID, ssid.c_str());
    strcpy(saveData.data.password, password.c_str());
  });

  //Start the network helper
  helper.start();
}

void loop()
{
  roku.update();
  discoverer.update();
  helper.background();
}
