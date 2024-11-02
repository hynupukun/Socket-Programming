#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#define buf_len 1024
using namespace std;

char request[buf_len],respond[buf_len];

void procsReqs(int s){
    if(!strcmp(request,"what year is now?")) strcpy(respond,"2024");
    else if(!strcmp(request,"how old are you?")) strcpy(respond,"15");
    else if(!strcmp(request,"you are slow")) strcpy(respond,"sorry, my bad :(");
    else if(!strcmp(request,"hello")) strcpy(respond,"Welcome to this server");
    else strcpy(respond,"Invalid Request");

    send(s,respond,sizeof(respond),0);
    memset(request,0,sizeof(request));
    memset(respond,0,sizeof(respond));
}

int main(int argc,char **argv){
    int serverSocket,acceptSocket;
    short port=55555;

    serverSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(!serverSocket){
        cout<<"Error in creating socket"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"Socket was created"<<endl;

    sockaddr_in serverAddr;
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serverAddr.sin_port=htons(port);

    if(bind(serverSocket,(sockaddr*)&serverAddr,sizeof(serverAddr))){
        cout<<"Bind failed"<<endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    cout<<"Bind succeeded"<<endl;

    if(listen(serverSocket,5)){
        cout<<"Listening failed on socket"<<endl;
    }
    cout<<"Listening succeeded\nWaitting for connection..."<<endl;

    acceptSocket=accept(serverSocket,NULL,NULL);
    if(!acceptSocket){
        cout<<"Accept failed"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"Accept connection"<<endl;

    while(1){
        recv(acceptSocket,request,sizeof(request),0);
        cout<<request<<endl;
        procsReqs(acceptSocket);
    }

    system("pause");
    close(serverSocket);
    close(acceptSocket);
    return 0;
}