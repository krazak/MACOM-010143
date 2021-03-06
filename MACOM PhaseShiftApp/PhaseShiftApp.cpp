// PhaseShiftApp.cpp : Defines the entry point for the console application.
//

#include "..\MACOM PhaseShiftDriver\PhaseShiftDriver.h"

#include <iostream>

using namespace std;

void PrintOptions();

int main()
{
	int option;
	double dPhase;
	string sError;
	PrintOptions();
	PhaseShiftDriver phaseShiftDrvr;
	phaseShiftDrvr.Initialize();

	cin >> option;
	while (option != 2)
	{
		switch (option)
		{
		case 1:
			cout << "Enter the phase angle" << endl;
			cin >> dPhase;
			if (dPhase < 0 || dPhase > 360)
			{
				cout << "The phase angle is out of range." << endl;
			}
			else
			{
				if (!phaseShiftDrvr.CommandPhaseShift(dPhase, sError))
				{
					cout << sError << endl;
				}
			}
			break;
		case 2:
			phaseShiftDrvr.Shutdown();
		}
		PrintOptions();
		cin >> option;
	}
    return 0;
}

void PrintOptions()
{
	cout << "Please choose number from one of the options:" << endl;
	cout << "1. Command phase shift (0-360)" << endl;
	cout << "2. Exit" << endl;
}

