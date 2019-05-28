// PhaseShiftApp.cpp : Defines the entry point for the console application.
//

#include "..\..\PhaseShiftDriver\PhaseShiftDriver\PhaseShiftDriver.h"

#include "stdafx.h"
#include <iostream>

using namespace std;

void PrintOptions();

int main()
{
	int option;
	double dPhase;
	PrintOptions();
	PhaseShiftDriver phaseShiftDrvr;

	cin >> option;
	while (option != 3)
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
				phaseShiftDrvr.CommandPhaseShift(dPhase);
			}
			break;
		case 2:
			cout << "Read the phase angle";
			break;
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
	cout << "2. Read phase angle." << endl;
	cout << "3. Exit" << endl;
}

