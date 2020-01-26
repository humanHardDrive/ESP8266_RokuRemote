#include "Roku.h"

Roku::Roku() :
  m_Port(0),
  m_bConnected(false)
{

}

Roku::Roku(IPAddress address, uint16_t port) :
  m_Address(address),
  m_Port(port),
  m_bConnected(false)
{
}

void Roku::connect()
{
  if (m_Port)
    m_Client.connect(m_Address, m_Port);
}

void Roku::connect(IPAddress address, uint16_t port)
{
  m_Client.connect(address, port);
}

void Roku::disconnect()
{
  if (isConnected())
    m_Client.stop();
}

void Roku::update()
{
  if (m_Client.connected() && !m_bConnected)
  {
    Serial.println("Roku connected");
    m_bConnected = true;
  }
  else if(!m_Client.connected() && m_bConnected)
  {
    Serial.println("Roku disconnected");
    m_bConnected = false;
  }
}
