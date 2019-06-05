#pragma once

class Map_010143 : IDataSuscriber
{
public:
	Map_010143();
	~Map_010143();
	void Start();
	void Shutdown();
	void OnReceivedClientData(char buffer[], int &length) override;
private:
	static const double s_dStepSize;
	double m_dPhaseShift;
	TCPServer *m_pServer;
};

