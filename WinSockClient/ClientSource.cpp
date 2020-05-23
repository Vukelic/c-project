#include"ClientHeader.h"

int moj_broj = 6;
void SendMsg(SOCKET socket, char *msg, int msg_len) {

	int len = msg_len;

	// Initialize select parameters
	FD_SET set;
	timeval timeVal;
	FD_ZERO(&set);
	// Add socket we will wait to read from
	FD_SET(socket, &set);
	// Set timeouts to zero since we want select to return
	// instantaneously
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;

	FD_SET(socket, &set);
	int iResult = select(0 /* ignored */, NULL, &set, NULL, &timeVal);

	if (iResult > 0) {
		// Send an prepared message with null terminator included

		iResult = send(socket, msg, (int)strlen(msg) + 1, 0);

		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(socket);
			WSACleanup();
		}

	}

}
int Receive(SOCKET acceptedSocket, char* recvbuf)
{
	char poruka[] = "";
	FD_SET set;
	timeval timeVal;
	FD_ZERO(&set);
	// Add socket we will wait to read from
	FD_SET(acceptedSocket, &set);
	int ready = select(0 /* ignored */, &set, NULL, NULL, &timeVal);
	if (ready < 0)
	{
		printf("Select failed!\n");
	}

	int res = 0;

	if (ready > 0) {

		res = recv(acceptedSocket, recvbuf, DEFAULT_BUFLEN, 0);

		if (res > 0)
		{
			printf("\nServer: %s.\n", recvbuf);

		}
	}

	return res;
}
char* ProveraBroja(int niziOpseg, int visiOpseg, int recvbuf) {


	if (recvbuf > moj_broj) {
		return "Vece, pokusaj ponovo";
	}
	else if (recvbuf < moj_broj) {
		return "Manje, pokusaj ponovo";
	}
	else {
		return "Tacno, igra je zavrsena";
	}


}