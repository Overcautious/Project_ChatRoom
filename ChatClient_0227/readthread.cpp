#include "readthread.h"
#include "chatclient.h"
ReadThread::ReadThread()
{

}

void ReadThread::run(){
    ChatClient* chatClient = ChatClient::getTcpSocketClient();
    while(true){
        if(chatClient->getTcpClient()->waitForReadyRead(-1)){
            //chatClient->readData();
        }
    }
}
