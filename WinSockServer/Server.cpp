#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include"ServerHeader.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"
#define SERVER_SLEEP_TIME 500

bool InitializeWindowsSockets();
Queue *headd, *tail;

int broj_igraca = 0;
char poruka_poredjenja[] = "";
bool cekaj_odgovor_prvi = true;
bool igra_pocela = false;
bool zadat_interval = false;
HANDLE *thread_array = NULL;
HANDLE t[5];
ListaSocket *head;
bool prekoracenje = false;
bool pom_broj_igraca = false;
int redniBrojKlijenta = 0;
bool odgovor = false;
bool nemaMesta = false;
bool interval1_prosao = true;
bool interval2_prosao = true;
bool poslat_interval_svim_klijentima = true;
int nmbr_poruka = 0;
//char* poruka_gde_je_interval1 = "1";
char* poruka_gde_je_interval1 = (char*)malloc(sizeof(char) * 5);

//char* poruka_gde_je_interval2 = "20";
char* poruka_gde_je_interval2 = (char*)malloc(sizeof(char) * 5);
bool poslao_se_prvi = true;
bool poslao_se_drugi = true;


DWORD WINAPI ThreadFunction(LPVOID param)
{
	memset(poruka_gde_je_interval1, 0, 5);
	memset(poruka_gde_je_interval2, 0, 5);

	int socket_index = (int)param;
	SOCKET acceptedSocket = ElementAt(head, socket_index);
	printf("Krenuo %d\n", socket_index);
	int Received = 1;
	int iResult = 1;
	FD_SET read;
	timeval timeVal;

	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	char recvbuf[DEFAULT_BUFLEN];

	int broj_poruka = 0;
	bool dozvola = false;
	bool cekanje = false;
	bool cekaj_odgovor = true;
	int povratni_socket = 0;

	int primljeno = 0;
	FD_ZERO(&read);
	FD_SET(acceptedSocket, &read);
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;
	char poruka_cekanja[] = "Igra jos nije pocela, molimo sacekajte";
	char pocetak[] = "Igra pocinje. Unesite broj";
	char unesi_interval1_poruka[] = "Unesite nizi interval:";
	char unesi_interval2_poruka[] = "Unesite visi interval:";
	char unesi_username[] = "Unesite username:";
	char poruka_prekoracenja[] = "Igra je vec pocela, ne mozete pristupiti";
	char uzmi_poslednjeg[] = "";
	char gotovo[] = "Igra je zavrsena";

	while (primljeno <= 0) {

		FD_ZERO(&read);
		FD_SET(acceptedSocket, &read);

		int ready = select(0 /* ignored */, NULL, &read, NULL, &timeVal);

		if (ready < 0)
		{
			printf("Select failed!\n");
		}
		if (ready > 0) {

			SendMsg(acceptedSocket, unesi_username, strlen(unesi_username)); //prvi prima poruku sa username-om
			primljeno = 1;
		}
	}

	primljeno = 0;

	while (primljeno <= 0) {

		primljeno = Receive(acceptedSocket, recvbuf, 4, &headd, &tail, &broj_poruka, socket_index, &broj_igraca, poruka_poredjenja); //pristigla poruka sa username-om
	}

	if (socket_index == 0) {

		initQueue(&headd, &tail); //samo pri prvom klijentu napravi queue posle toga sve poruke idu na njega

		char broj_ucesnika[] = "Koliko ucesnika ocekujete?";

		SendMsg(acceptedSocket, broj_ucesnika, strlen(broj_ucesnika)); //druga poruka je poslata 

		dozvola = true; //ako je prvi client daj dozvolu za recive
	}

	while (1)
	{
		if (dozvola) { //primaj od prvog, drugom posalji da se ceka igra

			iResult = Receive(acceptedSocket, recvbuf, 4, &headd, &tail, &broj_poruka, socket_index, &broj_igraca, poruka_poredjenja); //koliko ih ocekuje

			if (iResult > 0)
				dozvola = false;
		}
		else {

			if (igra_pocela == true) {

				if (socket_index == 0)
				{
					if (interval1_prosao) {

						SendMsg(acceptedSocket, unesi_interval1_poruka, strlen(unesi_interval1_poruka)); //poruka sa intervalom
						interval1_prosao = false;
					}
					//iResult = Receive(acceptedSocket, recvbuf, 4, &headd, &tail, &broj_poruka, socket_index, &broj_igraca, poruka_poredjenja); //prima interval nizi

					if (interval2_prosao) {

						SendMsg(acceptedSocket, unesi_interval2_poruka, strlen(unesi_interval2_poruka)); //poruka sa intervalom
						interval2_prosao = false;
					}

					iResult = Receive(acceptedSocket, recvbuf, 4, &headd, &tail, &broj_poruka, socket_index, &broj_igraca, poruka_poredjenja); //prima interval visi

					if (iResult > 0) {
						if (nmbr_poruka == 0) {
							memcpy(poruka_gde_je_interval1, recvbuf, (int)strlen(recvbuf));
						}
						/*if (nmbr_poruka == 1) {
							memset(poruka_gde_je_interval2, 0, 5);
							memcpy(poruka_gde_je_interval2, recvbuf, sizeof(recvbuf));
						}*/
						nmbr_poruka++;
						if (nmbr_poruka == 2) {
							//	memset(poruka_gde_je_interval2, 0, 5);
								//memcpy(poruka_gde_je_interval2, recvbuf, sizeof(recvbuf));
							memcpy(poruka_gde_je_interval2, recvbuf, (int)strlen(recvbuf));
							zadat_interval = true;
						}
					}
				}
				else
				{
					if (zadat_interval)
					{
						if (poslat_interval_svim_klijentima) {

							for (int y = 0; y <= broj_igraca; y++) //br igraca je npr 3 
							{

								SendMsg(ElementAt(head, y), poruka_gde_je_interval1, strlen(poruka_gde_je_interval1));

								SendMsg(ElementAt(head, y), poruka_gde_je_interval2, strlen(poruka_gde_je_interval2));
							}

							poslat_interval_svim_klijentima = false;

							if (socket_index == broj_igraca) {
								cekaj_odgovor = true;
							}

							Sleep(50);
						}

						if (cekaj_odgovor) {

							SendMsg(acceptedSocket, pocetak, strlen(pocetak)); //igra je pocela
							cekaj_odgovor = false;
						}
						
						iResult = Receive(acceptedSocket, recvbuf, 4, &headd, &tail, &broj_poruka, socket_index, &broj_igraca, poruka_poredjenja); //primio je broj, posalji ga prvom clientu na proveru

						if (iResult > 0) {

							itoa(TakeLast(&headd), uzmi_poslednjeg, 10);
							SendMsg(ElementAt(head, 0), uzmi_poslednjeg, strlen(uzmi_poslednjeg)); //saljem prvom clientu poslednjeg iz queue-a
						}

					}
					else
					{
						if (cekanje != true) {

							SendMsg(acceptedSocket, poruka_cekanja, strlen(poruka_cekanja)); //cekaj
							Sleep(50);
						}
						cekanje = true;
					}
				}
			}
			else
			{
				if (strcmp(poruka_poredjenja, "") != 0) {

					povratni_socket = Dequeue(&headd);

					SendMsg(ElementAt(head, povratni_socket), poruka_poredjenja, strlen(poruka_poredjenja)); //saljem prvom clientu poslednjeg iz queue-a

					if ((strcmp(poruka_poredjenja, "Tacno, igra je zavrsena") == 0)) {

						for (int y = 0; y <= broj_igraca; y++)
						{
							if (povratni_socket != y) {

								SendMsg(ElementAt(head, y), gotovo, strlen(gotovo));
							}
						}

						for (int y = 0; y < broj_igraca; y++)
						{
							int iResult = shutdown(ElementAt(head, y), SD_SEND);
							if (iResult == SOCKET_ERROR)
							{
								printf("shutdown failed with error: %d\n", WSAGetLastError());
								closesocket(ElementAt(head, y));
								WSACleanup();
								return 1;
							}
							closesocket(ElementAt(head, y));
						}

						/*	printf("Press any key to ext..");
							break;*/
					}

					igra_pocela = true;
					cekanje = true;
					poslat_interval_svim_klijentima = false;
					zadat_interval = true;
					//cekaj_odgovor = true;
				}

				if (cekanje != true) {

					SendMsg(acceptedSocket, poruka_cekanja, strlen(poruka_cekanja)); //cekaj
				}

				cekanje = true;
			}

		}

		if (iResult < 0)
		{
			// there was an error during recv
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(acceptedSocket);
		}

		Sleep(500);
	}
	return 1;
}

DWORD WINAPI ContolThreadFunction(LPVOID param)
{
	char poruka[] = "Niste u igri. Jbg..";
	while (1) {
		if (redniBrojKlijenta - 1 >= broj_igraca && broj_igraca != 0)
		{
			for (int i = 0; i < broj_igraca; i++)
			{
				ResumeThread(thread_array[i]);
			}

			for (int y = broj_igraca + 1; y < redniBrojKlijenta; y++)
			{
				SendMsg(ElementAt(head, y), poruka, strlen(poruka));
				Sleep(3000);
				int iResult = shutdown(ElementAt(head, y), SD_SEND);
				if (iResult == SOCKET_ERROR)
				{
					printf("shutdown failed with error: %d\n", WSAGetLastError());
					closesocket(ElementAt(head, y));
					WSACleanup();
					return 1;
				}
				closesocket(ElementAt(head, y));
			}

			igra_pocela = true;
			nemaMesta = true;
			break;
		}
		Sleep(2000);
	}
	return 1;
}

int  main(void)
{
	// Socket used for listening for new clients 
	SOCKET listenSocket = INVALID_SOCKET;
	// Socket used for communication with client
	SOCKET acceptedSocket = INVALID_SOCKET;
	// variable used to store function return value
	int iResult = 1;
	// Buffer used for storing incoming data
	char recvbuf[DEFAULT_BUFLEN];



	if (InitializeWindowsSockets() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
	}

	// Prepare address information structures
	addrinfo *resultingAddress = NULL;
	addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4 address
	hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
	hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
	hints.ai_flags = AI_PASSIVE;     // 

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	listenSocket = socket(AF_INET,      // IPv4 address famly
		SOCK_STREAM,  // stream socket
		IPPROTO_TCP); // TCP

	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket - bind port number and local address 
	// to socket
	iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// Since we don't need resultingAddress any more, free it
	freeaddrinfo(resultingAddress);

	// Set listenSocket in listening mode
	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("Server initialized, waiting for clients.\n");
	DWORD threadId;
	HANDLE control_thread = CreateThread(NULL, 0, &ContolThreadFunction, (LPVOID)0, 0, &threadId);
	HANDLE thread;


	InitList(&head);

	do
	{
		acceptedSocket = accept(listenSocket, NULL, NULL);

		if (acceptedSocket == INVALID_SOCKET)
		{
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		unsigned long int nonBlockingMode = 1;
		iResult = ioctlsocket(acceptedSocket, FIONBIO, &nonBlockingMode);
		printf("Client is connected");

		if (nemaMesta)
		{
			char poruka[] = "Igra je pocela. Ne mozete pristupiti.";
			SendMsg(acceptedSocket, poruka, strlen(poruka));
			Sleep(3000);
			iResult = shutdown(acceptedSocket, SD_SEND);
			if (iResult == SOCKET_ERROR)
			{
				printf("shutdown failed with error: %d\n", WSAGetLastError());
				closesocket(acceptedSocket);
				WSACleanup();
				return 1;
			}
			closesocket(acceptedSocket);
			continue;
		}

		AddToList(&head, acceptedSocket);

		DWORD threadId;

		if (redniBrojKlijenta == 0)
		{
			thread = CreateThread(NULL, 0, &ThreadFunction, (LPVOID)redniBrojKlijenta, 0, &threadId);
		}
		else
		{
			//if (redniBrojKlijenta == 1)
			//{
			//	thread_array = (HANDLE*)malloc(sizeof(HANDLE));
			//}
			thread_array = (HANDLE*)realloc(thread_array, sizeof(HANDLE) * redniBrojKlijenta);

			thread_array[redniBrojKlijenta - 1] = CreateThread(NULL, 0, &ThreadFunction, (LPVOID)redniBrojKlijenta, CREATE_SUSPENDED, &threadId);

			//memcpy(thread_array[redniBrojKlijenta - 1], t, sizeof(HANDLE));

		}

		redniBrojKlijenta++;

	} while (1);

	// shutdown the connection since we're done
	iResult = shutdown(acceptedSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(acceptedSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	free(poruka_gde_je_interval1);
	free(poruka_gde_je_interval2);
	CloseHandle(control_thread);
	CloseHandle(thread);
	closesocket(listenSocket);
	closesocket(acceptedSocket);
	WSACleanup();

	return 0;
}

bool InitializeWindowsSockets()
{
	WSADATA wsaData;
	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

