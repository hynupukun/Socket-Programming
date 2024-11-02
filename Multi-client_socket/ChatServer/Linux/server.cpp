#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cmath>
#include <sstream>
#define buf_len 1024
using namespace std;

char request[buf_len], respond[buf_len];
int listenSocket, maxfd;
short port = 55555;

void SendAllChat(int sock, fd_set &master, string out)
{
    for (int i = maxfd; i > 0; i--)
    {
        int other = (master.fds_bits[0] >> i) & 1 ? i : 0;
        if (other && other != sock && other != listenSocket)
        {
            send(other, out.c_str(), out.length() + 1, 0);
        }
    }
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
            ostringstream ss;
            string out;
            if (sock == listenSocket)
            {
                int client = accept(listenSocket, nullptr, nullptr);
                if (client > maxfd)
                    maxfd = client;
                FD_SET(client, &master);
                send(client, welcome, sizeof(welcome), 0);
                ss << "Client #" << client << " join the server!" << endl;
                out = ss.str();
                cout << out;
                SendAllChat(client, master, out);
            }
            else
            {
                recv(sock, request, sizeof(request), 0);
                if (!strcmp(request, "q"))
                {
                    FD_CLR(sock, &master);
                    close(sock);
                    ss << "Client #" << sock << " leave the server!" << endl;
                    out = ss.str();
                    cout << out;
                }
                else
                {
                    ss << "Client #" << sock << " : " << request;
                    out = ss.str();
                }
                SendAllChat(sock, master, out);
            }
        }
    }

    system("pause");
    close(listenSocket);
    return 0;
}