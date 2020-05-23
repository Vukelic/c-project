#pragma once
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"

struct Queue
{
	int value;
	int socket;
	Queue *next;
};

struct ListaSocket
{
	SOCKET socket;
	struct ListaSocket *next;
};

void initQueue(Queue **head, Queue **tail);
void InitList(ListaSocket **head);
int Receive(SOCKET socket, char* msg, int msg_len, Queue **head, Queue **tail, int* broj_poruka, int socket_index, int* broj_igraca, char* poruka_poredjenja);
void Enqueue(Queue **head, Queue **tail, int number, int socket);
int TakeLast(Queue **head);
void printQueue(Queue *head);
void SendMsg(SOCKET socket, char *msg, int msg_len);
void AddToList(ListaSocket **head, SOCKET s);
SOCKET ElementAt(ListaSocket *head, int index);
int Dequeue(Queue **head);
