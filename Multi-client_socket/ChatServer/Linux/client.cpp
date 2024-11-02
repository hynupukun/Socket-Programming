#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include<thread>
#define buf_len 1024
using namespace std;

char request[buf_len], respond[buf_len];
bool disconnected=0;

void ReadInputReqs(int client){
    while(1){
        cin.getline(request,buf_len);
        send(client,request,sizeof(request),0);
        if(!strcmp(request,"q")){
            disconnected=1;
            break;
        } 
        memset(request,0,sizeof(request));
    }
}

void RecvMsg(int client){
    while(!disconnected){
        recv(client,respond,sizeof(respond),0);
        cout<<respond<<endl;
        memset(respond,0,sizeof(respond));
    }
}

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
    
    thread wInput(ReadInputReqs,clientSocket);
    RecvMsg(clientSocket);
    if(wInput.joinable()) wInput.join();

    system("pause");
    close(clientSocket);
    return 0;
}