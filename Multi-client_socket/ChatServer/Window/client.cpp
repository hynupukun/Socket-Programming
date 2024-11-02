#include<iostream>
#include<ws2tcpip.h>
#include<winsock2.h>
#include<thread>
#define buf_len 1024
using namespace std;

char request[buf_len],respond[buf_len];
bool disconnected=0;

void ReadInputReqs(SOCKET client){
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

void RecvMsg(SOCKET client){
    while(!disconnected){
        recv(client,respond,sizeof(respond),0);
        cout<<respond<<endl;
        memset(respond,0,sizeof(respond));
    }
}

int main(int argc,char **argv){
    SOCKET clientSocket;
    short port=55555;

    WSADATA wsaData;
    WORD wVersionRequest=MAKEWORD(2,2);
    if(WSAStartup(wVersionRequest,&wsaData)){
        perror("The Winsock dll not found");
        exit(EXIT_FAILURE);
    }
    cout<<"The Winsock dll found"<<endl;
    cout<<"Status: "<<wsaData.szSystemStatus<<endl;

    clientSocket=INVALID_SOCKET;
    clientSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(clientSocket==INVALID_SOCKET){
        cout<<"Error in creating socket: "<<WSAGetLastError()<<endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout<<"Socket was created"<<endl;

    sockaddr_in clientService;
    clientService.sin_family=AF_INET;
    clientService.sin_addr.s_addr=inet_addr("127.0.0.1");
    clientService.sin_port=htons(port);

    if(connect(clientSocket,(sockaddr*)&clientService,sizeof(clientService))){
        cout<<"Connect failed: "<<WSAGetLastError<<endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout<<"Connect succeeded\nWaitting for some request...\n";

    thread wInput(ReadInputReqs,clientSocket);
    RecvMsg(clientSocket);
    if(wInput.joinable()) wInput.join();

    system("pause");
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}