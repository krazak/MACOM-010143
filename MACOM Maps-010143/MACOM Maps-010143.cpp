// MACOM Maps-010143.cpp : Defines the entry point for the console application.
//

#include "..\Common\Communication.h"
#include "MACOM Maps-010143.h"
#include <iostream>
#include <thread>

using namespace std;

const double Map_010143::s_dStepSize = 22.5;

Map_010143::Map_010143() :
	m_dPhaseShift(0.0),
	m_pServer(nullptr)
{
}

Map_010143::~Map_010143()
{
	Shutdown();
	if (nullptr != m_pServer)
	{
		delete m_pServer;
	}
}


void Map_010143::Start()
{
	m_pServer = new TCPServer(this);
	m_pServer->Start();
}

void Map_010143::Shutdown()
{
	m_pServer->Shutdown();
}

void Map_010143::OnReceivedClientData(char buffer[], int &length)
{
	short rawValue = buffer[0];
	rawValue = rawValue >> 2;
	m_dPhaseShift = rawValue * s_dStepSize;
	cout << "The current phase shift is " << m_dPhaseShift << " degrees" << endl;
}


int main()
{
	Map_010143 *phaseShifter = new Map_010143();
	std::thread commThread(&Map_010143::Start, phaseShifter);
	int choice = 0;
	while (choice != 1)
	{
		cout << "To exit phase shifter enter 1." << endl << endl;
		cin >> choice;
	}
	phaseShifter->Shutdown();
	commThread.join();
	delete phaseShifter;
    return 0;
}







