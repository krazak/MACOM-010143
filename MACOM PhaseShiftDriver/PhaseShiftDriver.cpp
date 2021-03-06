// PhaseShiftDriver.cpp : Defines the entry point for the console application.
//

#include "..\Common\Communication.h"
#include "PhaseShiftDriver.h"

const double PhaseShiftDriver::dStepSize = 22.5;

PhaseShiftDriver::PhaseShiftDriver() 
{
}

void PhaseShiftDriver::Initialize()
{
	m_pComIfc = new TCPClient();
	m_pComIfc->Initialize();
	//unsigned char 
	//SPI_configuration()
}

bool PhaseShiftDriver::CommandPhaseShift(int PhaseShift, std::string &sError)
{
	if (PhaseShift < 0 || PhaseShift >= 360)
	{
		sError = "The phase angle must be between 0 and 360 degrees";
		return false;
	}
	if (!m_pComIfc || !m_pComIfc->IsInitialized())
	{
		sError = "The communication channel has not been initialized.";
		return false;
	}
	if (!m_pComIfc->IsConnected())
	{
		if (!m_pComIfc->Connect())
		{
			sError = "The communication to MACOM-010143 has not been established.";
			return false;
		}		
	} 
	char buffer[2];
	memset(buffer, 0, sizeof(char) * 2);
	//With SPI we are supposed to be sending individaul bits (MSB first), however since we are using TCP/IP we're sending
	//a byte at a time where each byte represents one of the bits (D6-D1).
	bool bRslt = true; 	
	unsigned short usShiftMultiplier = (unsigned short)floor((double)PhaseShift / dStepSize);
	usShiftMultiplier = usShiftMultiplier << 2;   //d1 and d2 are dummy bits.
	memcpy(buffer, (char *)&usShiftMultiplier, sizeof(short));
	return m_pComIfc->SendData(buffer, 2);
}

void PhaseShiftDriver::Shutdown()
{
	m_pComIfc->Shutdown();
}
