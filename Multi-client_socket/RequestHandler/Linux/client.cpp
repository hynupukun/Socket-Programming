#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define buf_len 1024
using namespace std;

char request[buf_len], respond[buf_len];

int main(int argc, char **argv)
{
    int clientSocket;
    short port = 55555;

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!clientSocket)
    {
        cout << "Error in creating socket" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Socket was created" << endl;

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(port);

    if (connect(clientSocket, (sockaddr *)&clientService, sizeof(clientService)))
    {
        cout << "Connect failed:" << endl;
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    cout << "Connect succeeded\nWaitting for some request...\n";
    recv(clientSocket, respond, sizeof(respond), 0);
    cout << respond << endl;
    memset(respond, 0, sizeof(respond));

    while (1)
    {
        cin.getline(request, buf_len);
        send(clientSocket, request, sizeof(request), 0);
        if (!strcmp(request, "q")) break;
        recv(clientSocket, respond, sizeof(respond), 0);
        cout << respond << endl;
        memset(respond, 0, sizeof(respond));
        memset(request, 0, sizeof(request));
    }

    system("pause");
    close(clientSocket);
    return 0;
}