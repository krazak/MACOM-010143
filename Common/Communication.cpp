#include "Communication.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

const char *TCPClient::address = "127.0.0.1";
const char *TCPClient::port = "4000";

TCPClient::TCPClient() :
	m_bConnected(false),
	m_bInitialized(false),
	m_sError(""),
	m_socket(INVALID_SOCKET),
	m_addrinfo_rslt(nullptr)
{
}

void TCPClient::Initialize()
{
	// Initialize Winsock
	m_sError = "";
	WSADATA wsaData;
	int iRslt;
	if ((iRslt = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) 
	{
		m_sError = "Failed WSAStartup. Code: " + iRslt;
		m_bInitialized = false;
		return;
	}
	addrinfo af;
	memset(&af, 0, sizeof(af));
	af.ai_family = AF_INET;
	af.ai_socktype = SOCK_STREAM;
	af.ai_protocol = IPPROTO_TCP;

	if ((iRslt = getaddrinfo(address, port, &af, &m_addrinfo_rslt)) != 0)
	{
		m_sError = "Failed getaddinfo. Code: " + iRslt;
		m_bInitialized = false;
		WSACleanup();
		return;
	}
	// Create a new socket to make a client connection.
	m_socket = socket(m_addrinfo_rslt->ai_family, m_addrinfo_rslt->ai_socktype, m_addrinfo_rslt->ai_protocol);
	if (m_socket == INVALID_SOCKET)
	{
		m_sError = "Failed to create socket. Code: " + WSAGetLastError();
		m_bInitialized = false;
		WSACleanup();
		return;
	}
	m_bInitialized = true;
}



bool TCPClient::Connect()
{
	m_sError = "";
	if (!m_bInitialized)
	{
		return false;
	}
	int iRslt;
	iRslt = connect(m_socket, m_addrinfo_rslt->ai_addr, m_addrinfo_rslt->ai_addrlen);
	if (iRslt != 0)
	{
		m_sError = "Failed to connect.  Code: " + WSAGetLastError();
		m_bConnected = false;
		return false;
	}
	m_bConnected = true;
	return true;
}

bool TCPClient::SendData(const char *pData, int length)
{
	if (m_bInitialized)
	{
		if (!m_bConnected)
		{
			m_sError = "No connection detected.";
			return false;
		}
		int iRslt = send(m_socket, pData, length, 0);
		if (SOCKET_ERROR == iRslt)
		{
			m_sError = "Send failed. Code: " + WSAGetLastError();
			closesocket(m_socket);
			WSACleanup();
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

void TCPClient::Shutdown()
{
	closesocket(m_socket);
}

const char *TCPServer::port = "4000";

TCPServer::TCPServer(IDataSuscriber *pDataSubscriber) :
	m_pDataSubscriber(pDataSubscriber),
	m_pClientHndlThread(nullptr)
{
}

TCPServer::~TCPServer()
{
	if (nullptr != m_pClientHndlThread)
	{
		delete m_pClientHndlThread;
	}
}

bool TCPServer::Start()
{
	m_sError = "";
	WSADATA wsaData;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	INT Ret;

	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("WSAStartup() failed with error %d\n", Ret);
		WSACleanup();
		return false;
	}
	else
		printf("WSAStartup() is fine!\n");

	struct addrinfo *res = NULL;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int status = getaddrinfo(NULL, port, &hints, &res);
	if (status != 0)
	{
		std::cout << "[ERROR]: " << status << " Unable to get address info for Port " << port << "." << std::endl;
		return false;
	}
	
	// Prepare a socket to listen for connections

	if ((ListenSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == SOCKET_ERROR)//, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		std::cout << "WSASocket() failed with error " << WSAGetLastError() << std::endl;
		return false;
	}
	else
		std::cout << "WSASocket() is OK!" << std::endl;

	if (::bind(ListenSocket, res->ai_addr, res->ai_addrlen) == SOCKET_ERROR)
	{
		std::cout << "[ERROR]: " << WSAGetLastError() << " Unable to bind Socket." << std::endl;
		freeaddrinfo(res);
		closesocket(ListenSocket);
		return false;
	}

	if (listen(ListenSocket, 1))
	{
		printf("listen() failed with error %d\n", WSAGetLastError());
		return false;
	}
	else
		printf("listen() is OK!\n");
	
	int iSocket;
	while (!m_bShutdown)
	{	
		//only accepting one client at a time.
		if ((ClientSocket = accept(ListenSocket, NULL, NULL)) == INVALID_SOCKET)
		{
			m_sError = "Failed to accept connection. Code: " + WSAGetLastError();
			return false;
		}
		else
		{
			printf("accepted a new connection!\n");
		}
		if (nullptr != m_pClientHndlThread)
		{
			m_pClientHndlThread->join();
			delete m_pClientHndlThread;
		}
		m_pClientHndlThread = new std::thread(&TCPServer::HandleClientRequests, this, ClientSocket);
	}
	
	return true;
}

void TCPServer::HandleClientRequests(int iSocket)
{
	char buffer[32];
	int length;
	bool bRslt = true;
	while (bRslt)
	{
		m_control.lock();
		if (m_bShutdown)
		{
			break;
		}		
		m_control.unlock();
		bRslt = Read(iSocket, buffer, length);
		if (bRslt)
		{
			m_pDataSubscriber->OnReceivedClientData(buffer, length);
		}
	}
}

void TCPServer::Shutdown()
{
	shutdown(ClientSocket, 0);
	shutdown(ListenSocket, 0);  //no longer interested in reading data
	closesocket(ClientSocket);
	closesocket(ListenSocket);
	m_control.lock();
	m_bShutdown = true;
	m_control.unlock();
	if (nullptr != m_pClientHndlThread)
	{
		if (m_pClientHndlThread->joinable())
		{
			m_pClientHndlThread->join();
		}
	}
}

bool TCPServer::Read(int iSocket, char buffer[], int &length)
{
	m_sError = "";
	length = recv(iSocket, buffer, 2, MSG_WAITALL);
	if (length > 0)
	{
		return true;
	}
	else if (length == 0)
	{
		m_sError = "Connection closed";
	}
	else
	{
		m_sError = "Received failed. Code: " + WSAGetLastError();
	}
	return false;
}

bool TCPServer::Send(int iSocket, char buffer[], int length)
{
	m_sError = "";
	int iRslt = send(iSocket, buffer, length, 0);
	if (SOCKET_ERROR == iRslt)
	{
		m_sError = "Send failed. Code: " + WSAGetLastError();
		closesocket(iSocket);
		WSACleanup();
		return false;
	}
	return true;
}


