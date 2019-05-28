#pragma once

#include <string>
#include <mutex>
#include <winsock2.h>


using namespace std;

class TCPClient
{
public:
	TCPClient();
	void Initialize();
	bool SendData(unsigned char *pData);
	bool Connect();
	bool IsInitialized() { return m_bInitialized; };
	bool IsConnected() { return m_bConnected; };
	string GetError() { return m_sError; };
private:
	bool m_bInitialized;
	bool m_bConnected;
	string m_sError;
	SOCKET m_socket;
	addrinfo *m_addrinfo_rslt;
	static const char *address;
	static const char *port;
};


class TCPServer
{
public:
	TCPServer();
	bool Start();
	bool Read(char buffer[], int &length);
	bool Send(char buffer[], int length);
	string GetError() { return m_sError; };
private:
	std::mutex m_control;
	string m_sError;
	int m_iSocket;
	bool m_bShutdown;
	static const char *port;
};
