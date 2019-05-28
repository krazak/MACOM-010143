#pragma once

class TCPClient;


class __declspec(dllexport) PhaseShiftDriver
{
public:
	PhaseShiftDriver();
	bool CommandPhaseShift(int Phase);
	void Initialize();
	
private:
	TCPClient * m_pComIfc;
};