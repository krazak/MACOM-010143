// PhaseShiftDriver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "SPI_funcs.h"
#include "Communication.h"
#include "PhaseShiftDriver.h"

PhaseShiftDriver::PhaseShiftDriver() 
{
}

PhaseShiftDriver::~PhaseShiftDriver()
{

}

void PhaseShiftDriver::Initialize()
{
	m_pComIfc = new TCPClient();
	m_pComIfc->Initialize();
	//unsigned char 
	//SPI_configuration()
}

bool PhaseShiftDriver::CommandPhaseShift(int PhaseShift)
{
	if (PhaseShift <= 0 || PhaseShift >= 360)
	{
		return false;
	}
	if (!m_pComIfc->IsInitialized())
	{
		return false;
	}
	if (!m_pComIfc->IsConnected())
	{
		if (!m_pComIfc->Connect())
		{
			return false;
		}		
	} 
	unsigned char buffer[4];
	for (int i = 0; i < 4; i++)
	{
		buffer[i] = (PhaseShift >> (i * 8));		
	}
	return m_pComIfc->SendData(buffer);
}
