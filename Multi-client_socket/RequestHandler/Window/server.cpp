#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#define buf_len 1024
using namespace std;

char request[buf_len], respond[buf_len];
SOCKET listenSocket;
short port = 55555;

void procsReqs(SOCKET s)
{
    if (!strcmp(request, "what year is now?"))
        strcpy(respond, "2024");
    else if (!strcmp(request, "how old are you?"))
        strcpy(respond, "15");
    else if (!strcmp(request, "you are slow"))
        strcpy(respond, "sorry, my bad :(");
    else if (!strcmp(request, "hello"))
        strcpy(respond, "Welcome to this server");
    else
        strcpy(respond, "Invalid Request");

    send(s, respond, sizeof(respond), 0);
    memset(request, 0, sizeof(request));
    memset(respond, 0, sizeof(respond));
}

int main(int argc, char **argv)
{
    WSADATA wsaData;
    WORD wVersionRequest = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequest, &wsaData))
    {
        perror("The Winsock dll not found");
        exit(EXIT_FAILURE);
    }
    cout << "The Winsock dll found" << endl;
    cout << "Status: " << wsaData.szSystemStatus << endl;

    listenSocket = INVALID_SOCKET;
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        cout << "Error in creating socket: " << WSAGetLastError() << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Socket was created" << endl;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)))
    {
        cout << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Bind succeeded" << endl;

    if (listen(listenSocket, 5))
    {
        cout << "Listening failed on socket: " << WSAGetLastError() << endl;
    }
    cout << "Listening succeeded\nWaitting for connection..." << endl;

    fd_set master;
    FD_ZERO(&master);
    FD_SET(listenSocket, &master);
    char welcome[] = "Welcome to this server!";

    while (1)
    {
        fd_set tmp = master;
        int sockcnt = select(0, &tmp, nullptr, nullptr, nullptr);
        for (int i = 0; i < sockcnt; i++)
        {
            SOCKET sock = tmp.fd_array[i];
            if (sock == listenSocket)
            {
                SOCKET client = accept(listenSocket, nullptr, nullptr);
                FD_SET(client, &master);
                send(client, welcome, sizeof(welcome), 0);
                cout << "Client #" << client << " join the server!"<<endl;
            }
            else
            {
                recv(sock, request, sizeof(request), 0);
                if (!strcmp(request, "q"))
                {
                    FD_CLR(sock, &master);
                    closesocket(sock);
                    cout << "Client #" << sock << " leave the server!"<<endl;
                }
                else
                {
                    procsReqs(sock);
                }
            }
        }
    }

    system("pause");
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}