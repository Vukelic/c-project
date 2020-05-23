#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27016

void SendMsg(SOCKET socket, char *msg, int msg_len);
int Receive(SOCKET socket, char* msg);
char* ProveraBroja(int niziOpseg, int visiOpseg, int predlog);
