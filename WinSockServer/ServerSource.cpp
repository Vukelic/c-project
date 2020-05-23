#include"ServerHeader.h"

int Receive(SOCKET acceptedSocket, char* recvbuf, int size, Queue **headd, Queue **tail, int* broj_poruka, int socket_index, int* broj_igraca, char* poruka_poredjenja)
{
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

		int poruka = atoi(recvbuf);

		if (res > 0)
		{
			if (*broj_poruka == 0) {

				printf("\nKlijent: %s.\n", recvbuf); //username

			}
			else {

				if (socket_index == 0 && *broj_poruka == 1) { //broj igraca

					printf("\nKijent ocekuje %s igraca.", recvbuf);
					int e = atoi(recvbuf);
					*broj_igraca = atoi(recvbuf);
				}
				else if (socket_index == 0 && *broj_poruka <= 3) {

					printf("\nInterval je %s.", recvbuf);
				}
				else if (poruka != 0) {

					Enqueue(headd, tail, poruka, socket_index);
					printQueue(*headd);
				}
				else {

					printf("\nKlijent: %s.\n", recvbuf);
					strcpy(poruka_poredjenja, recvbuf);
				}
			}
			*broj_poruka = *broj_poruka + 1;
		}
	}

	return res;
}
void initQueue(Queue **head, Queue **tail)
{
	*head = NULL;
	*tail = NULL;
}
void Enqueue(Queue **head, Queue **tail, int number, int socket) //stavlja br na kraj reda
{
	Queue *newElement = (Queue*)malloc(sizeof(Queue));
	newElement->value = number;
	newElement->socket = socket;
	newElement->next = NULL;

	if (*head == NULL)
	{
		*head = *tail = newElement;
	}
	else
	{
		(*tail)->next = newElement;
		*tail = newElement;
	}
}
int TakeLast(Queue **head)
{
	if (*head == NULL)
	{
		printf("\nQUEUE IS EMPTY!");
		return NULL;
	}
	else
	{
		Queue *take_last = *head;
		return take_last->value;
	}
}
void printQueue(Queue *head)
{
	Queue *current = head;
	printf("\nQueue elements: ");
	if (current == NULL)
	{
		printf("Empty queue\n");
		return;
	}

	while (current != NULL)
	{
		printf("%d ", current->value);
		printf("%d ", current->socket);
		current = current->next;
	}
	printf("\n");
}
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
void InitList(ListaSocket **head)
{
	*head = NULL;
}
void AddToList(ListaSocket **head, SOCKET s) {

	if (*head == NULL) {

		*head = (ListaSocket*)malloc(sizeof(ListaSocket));
		(*head)->socket = s;
		(*head)->next = NULL;
	}
	else {

		ListaSocket *current = *head;
		while (current->next != NULL) {

			current = current->next;
		}

		current->next = (ListaSocket*)malloc(sizeof(ListaSocket));
		current->next->socket = s;
		current->next->next = NULL;
	}
}
SOCKET ElementAt(ListaSocket *head, int index) {

	ListaSocket *currentNode = head;
	int brojac = 0;

	while (currentNode != NULL) {

		if (brojac == index) {

			return currentNode->socket;
		}
		brojac++;
		currentNode = currentNode->next;
	}
	return -1;
}
int Dequeue(Queue **head)
{
	int val = 0;

	if (*head == NULL)
	{
		printf("\nQUEUE IS EMPTY!");
		return -1;
	}
	else
	{
		Queue *deleteElement = *head;
		*head = (*head)->next;
		val = deleteElement->socket;
		free(deleteElement);
		return val;
	}
}
