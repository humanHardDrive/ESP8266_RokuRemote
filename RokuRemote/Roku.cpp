#include "Roku.h"

Roku::Roku() :
  m_Port(0)
{
  
}

Roku::Roku(IPAddress address, uint16_t port) :
  m_Address(address),
  m_Port(port)
{
}

void Roku::connect()
{
  if(m_Port)
    m_Client.connect(m_Address, m_Port);
}

void Roku::connect(IPAddress address, uint16_t port)
{
  m_Client.connect(address, port);
}

void Roku::disconnect()
{
  if(isConnected())
    m_Client.stop();
}

void Roku::update()
{
}
