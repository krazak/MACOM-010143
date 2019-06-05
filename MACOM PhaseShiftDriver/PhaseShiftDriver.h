#pragma once

#include <string>

class TCPClient;


class __declspec(dllexport) PhaseShiftDriver
{
public:
	PhaseShiftDriver();
	bool CommandPhaseShift(int Phase, std::string &sError);
	void Initialize();
	void Shutdown();
	
private:
	TCPClient * m_pComIfc;
	static const int s_ciBits = 4;
	static const double dStepSize;
};