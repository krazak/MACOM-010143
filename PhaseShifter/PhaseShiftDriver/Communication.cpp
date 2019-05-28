#include "stdafx.h"
#include "Communication.h"
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>



const char *TCPClient::address = "127.0.0.1";
const char *TCPClient::port = "8080";

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

bool TCPClient::SendData(unsigned char *pData)
{
	if (!m_bInitialized)
	{
		if (!Connect())
		{
			return false;
		}
	}
	return true;
}

const char *TCPServer::port = "8080";

TCPServer::TCPServer() 
{
}

bool TCPServer::Start()
{
	SOCKET s;
	m_sError = "";
	WSADATA wsaData;
	struct addrinfo *result = NULL;
	struct addrinfo hints;


	// Initialize Winsock
	int iRslt;
	if ((iRslt = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		m_sError = "Failed WSAStartup. Code: " + iRslt;
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	if ((iRslt = getaddrinfo(NULL, port, &hints, &result)) != 0)
	{
		m_sError = "Failed getaddinfo. Code: " + iRslt;
		WSACleanup();
		return false;
	}

	// Creating socket file descriptor 
	if ((s = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == INVALID_SOCKET)
	{
		m_sError = "Failed to create socket. Code: " + WSAGetLastError();
		WSACleanup();
		return false;
	}

	bind(s, result->ai_addr, (int)result->ai_addrlen);

	if (listen(s, 1) < 0)
	{
		m_sError = "Failed to listen on socket. Code: " + WSAGetLastError();
		return false;
	}
	if ((m_iSocket = accept(s, nullptr, nullptr))<0)
	{
		m_sError = "Failed to accept connection. Code: " + WSAGetLastError();
		return false;
	}
	return true;
}

bool TCPServer::Read(char buffer[], int &length)
{
	m_sError = "";
	length = recv(m_iSocket, buffer, 1024, 0);
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

bool TCPServer::Send(char buffer[], int length)
{
	m_sError = "";
	int iRslt = send(m_iSocket, buffer, length, 0);
	if (SOCKET_ERROR == iRslt)
	{
		m_sError = "Send failed. Code: " + WSAGetLastError();
		closesocket(m_iSocket);
		WSACleanup();
		return false;
	} 
	return true;
}


