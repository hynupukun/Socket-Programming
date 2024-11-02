#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cmath>
#include <thread>
#define buf_len 1024
using namespace std;

char request[buf_len], respond[buf_len];
int listenSocket, maxfd;
short port = 55555;

void procsReqs(int s)
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
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!listenSocket)
    {
        cout << "Error in creating socket" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Socket was created" << endl;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)))
    {
        cout << "Bind failed" << endl;
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
    cout << "Bind succeeded" << endl;

    if (listen(listenSocket, 5))
    {
        cout << "Listening failed on socket" << endl;
    }
    cout << "Listening succeeded\nWaitting for connection...\n";

    fd_set master;
    maxfd = listenSocket;
    FD_SET(listenSocket, &master);
    char welcome[] = "Welcome to this server!";

    while (1)
    {
        fd_set tmp = master;
        int sockcnt = select(maxfd + 1, &tmp, nullptr, nullptr, nullptr);
        for (int i = 0; i < sockcnt; i++)
        {
            int sock = log2(tmp.fds_bits[i]);
            if (sock == listenSocket)
            {
                int client = accept(listenSocket, nullptr, nullptr);
                if (client > maxfd)
                    maxfd = client;
                FD_SET(client, &master);
                send(client, welcome, sizeof(welcome), 0);
                cout << "Client #" << client << " join the server!" << endl;
            }
            else
            {
                recv(sock, request, sizeof(request), 0);
                if (!strcmp(request, "q"))
                {
                    FD_CLR(sock, &master);
                    close(sock);
                    cout << "Client #" << sock << " leave the server!" << endl;
                }
                else
                {
                    procsReqs(sock);
                }
            }
        }
    }

    system("pause");
    close(listenSocket);
    return 0;
}