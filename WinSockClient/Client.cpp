#include"ClientHeader.h"

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

int __cdecl main(int argc, char **argv)
{
	bool dozvola = false;
	int niziOpseg = 0;
	int visiOpseg = 0;
	int brojac = 0;
	int moji_pokusaji = 0;
	int moguce_kombinacije = 0;

	// socket used to communicate with server
	SOCKET connectSocket = INVALID_SOCKET;
	// variable used to store function return value
	int iResult;
	// message to send
	char *messageToSend = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];

	// Validate the parameters
	if (argc != 2)
	{
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

	if (InitializeWindowsSockets() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
	}

	// create a socket
	connectSocket = socket(AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// create and initialize address structure
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
	serverAddress.sin_port = htons(DEFAULT_PORT);
	// connect to server specified in serverAddress and socket connectSocket
	if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		closesocket(connectSocket);
		WSACleanup();
	}

	unsigned long int nonBlockingMode = 1;
	iResult = ioctlsocket(connectSocket, FIONBIO, &nonBlockingMode);

	FD_SET set;
	timeval timeVal;
	FD_ZERO(&set);
	// Add socket we will wait to read from
	FD_SET(connectSocket, &set);
	// Set timeouts to zero since we want select to return
	// instantaneously
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	FD_SET(connectSocket, &set);

	char broj[DEFAULT_BUFLEN];
	char predlog[DEFAULT_BUFLEN];
	while (true)
	{
		iResult = Receive(connectSocket, recvbuf);

		if (strcmp(recvbuf, "Igra jos nije pocela, molimo sacekajte") == 0 || iResult <= 0)
		{
			continue;
		}
		if (strcmp(recvbuf, "Igra je pocela. Ne mozete pristupiti.") == 0 || strcmp(recvbuf, "Niste u igri. Jbg..") == 0)
		{
			printf("Press any key to exit..");
			break;
		}

		if (strcmp(recvbuf, "Igra je zavrsena") == 0)
		{
			printf("Press any key to exit..");
			break;
		}

		if (iResult > 0) {

			if ((atoi(recvbuf)) != 0) {

				if (!dozvola) {

					niziOpseg = atoi(recvbuf); //primili nizi opseg
					brojac++;
					dozvola = true;
					//printf("Primio nizi opseg\n");
				}
				else {

					if (brojac == 1) {
						if ((atoi(recvbuf)) != 0) {
							visiOpseg = atoi(recvbuf); //primili visi opseg
							brojac++;
							//printf("Primio visi opseg\n");
						}
					}
					else {
						//moji_pokusaji = (niziOpseg + visiOpseg + 1) / 2;
						printf("Posalje prvi broj\n");
						//moji_pokusaji = (rand() % (visiOpseg - niziOpseg + 1)) + niziOpseg;
						strcpy(broj, ProveraBroja(niziOpseg, visiOpseg, atoi(recvbuf))); //ovde mu saljem 0 jer je inicijalno 0
						SendMsg(connectSocket, broj, sizeof(broj));
					}
				}

			}
			else {
				if (strcmp(recvbuf, "Vece, pokusaj ponovo") == 0)
				{
					//printf("Broj je veci\n");
					visiOpseg = moji_pokusaji - 1;
					moji_pokusaji = (visiOpseg + niziOpseg) / 2;
					itoa(moji_pokusaji, broj, 10);
					printf("Pogadjam %d \n", moji_pokusaji);
					SendMsg(connectSocket, broj, sizeof(broj));
				}
				else if (strcmp(recvbuf, "Manje, pokusaj ponovo") == 0) {
				//	printf("Broj je manji\n");

					niziOpseg = moji_pokusaji + 1;
					moji_pokusaji = (visiOpseg + niziOpseg) / 2;
					itoa(moji_pokusaji, broj, 10);
					printf("Pogadjam %d \n", moji_pokusaji);
					SendMsg(connectSocket, broj, sizeof(broj));
				}
				else if (strcmp(recvbuf, "Tacno, igra je zavrsena") == 0) {
					printf("VI STE POBEDNIK! \n\tPress any key to exit..");
					break;
				}
				else if (strcmp(recvbuf, "Igra pocinje. Unesite broj") == 0) {
				//	printf("Trazi prv broj  \n");
					//gets_s(broj, DEFAULT_BUFLEN);
					moji_pokusaji = (visiOpseg + niziOpseg) / 2;
					//moji_pokusaji = atoi(broj);
					itoa(moji_pokusaji, broj, 10);
					printf("Pogadjam %d\n", moji_pokusaji);
					SendMsg(connectSocket, broj, sizeof(broj));
				}
				else {
					//printf("nije ni jena od onih %s \n", recvbuf);
					gets_s(broj, DEFAULT_BUFLEN);
					SendMsg(connectSocket, broj, sizeof(broj));
				}

			}
		}

		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}
	}

	_getch();
	// cleanup
	closesocket(connectSocket);
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
