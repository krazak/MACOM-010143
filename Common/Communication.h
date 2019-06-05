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
	bool SendData(const char *pData, int length);
	bool Connect();
	bool IsInitialized() { return m_bInitialized; };
	bool IsConnected() { return m_bConnected; };
	string GetError() { return m_sError; };
	void Shutdown();
private:
	bool m_bInitialized;
	bool m_bConnected;
	string m_sError;
	SOCKET m_socket;
	addrinfo *m_addrinfo_rslt;
	static const char *address;
	static const char *port;
};



class IDataSuscriber
{
public:
	virtual void OnReceivedClientData(char buffer[], int &length) = 0;
};


class TCPServer
{
public:
	TCPServer(IDataSuscriber *m_pDataSubscriber);
	~TCPServer();
	void HandleClientRequests(int iSocket);
	bool Start();
	string GetError() { return m_sError; };
	void Shutdown();
private:
	std::mutex m_control;
	string m_sError;
	bool m_bShutdown;
	static const char *port;
	IDataSuscriber *m_pDataSubscriber;
	std::thread *m_pClientHndlThread;
	SOCKET ListenSocket;
	SOCKET ClientSocket;

	bool Read(int iSocket, char buffer[], int &length);
	bool Send(int iSocket, char buffer[], int length);

};
