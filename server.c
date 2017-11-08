#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "user.h"

#define LISTENQ 10
#define HELLO_TO_EXISTED "[Server] Someone is coming!\n"
#define HELLO_TO_NEW "[Server] Hello, anonymous! From: %s/%u\n"

int MAX(int x,int y){
    return x > y ? x : y;
}

int* set_fdset(user_t *curNode,fd_set *rfds,int *maxfdp){
    while(curNode != NULL){
        FD_SET(curNode->fd,rfds);
        *maxfdp = MAX(*maxfdp,curNode->fd);
        curNode = curNode->next;
    }
}

void clearBuf(char *buffer,int len){
    memset(buffer,0,len);
}

int send_private_message(user_t *root,user_t *sender,char *param){
    char *spacePtr;
    char *receiverName,*msg;
    char *outputMsg[2];
    char buf[4200];
    int i,success;
    user_t *cur,*receiver;

    success = 0;
    spacePtr = strchr(param,' ');
    for(i=0;i<2;i++){
        outputMsg[i] = NULL;
    }
    if(spacePtr == NULL){
        receiverName = strdup(param);
        msg = strdup(" ");
    }else{
        receiverName = strndup(param,spacePtr-param);
        msg = strndup(spacePtr+1,strlen(param) - (spacePtr-param));
    }

    if(strcmp(sender->name,"anonymous") == 0){
        *outputMsg = strdup("[Server] ERROR: You are anonymous.\n");
    }else{
        cur = root;
        //Find receiver
        receiver = NULL;
        while(cur != NULL){
            if(strcmp(cur->name,receiverName) == 0){
                receiver = cur;
                break;
            }
            cur = cur->next;
        }
        if(strcmp(receiver->name,"anonymous") == 0){
            *outputMsg = strdup("[Server] ERROR: The client to which you sent is anonymous.\n");
        }else if(receiver == NULL){
            *outputMsg = strdup("[Server] ERROR: The receiver doesn't exist.\n");
        }else{
            success = 1;
            *outputMsg = strdup("[Server] SUCCESS: Your message has been sent.\n");
            sprintf(buf,"[Server] %s tell you %s\n",sender->name,msg);
            *(outputMsg+1) = strdup(buf);
            write(receiver->fd,outputMsg[1],strlen(outputMsg[1]));
        }
    }
    write(sender->fd,outputMsg[0],strlen(outputMsg[0]));
    for(i=0;i<2;i++){
        if(outputMsg[i] != NULL){
            free(outputMsg[i]);
        }
    }
    free(receiverName);
    free(msg);
    return success;
}

void broadcast_message(user_t *root,user_t *sender,char *msg){
    char buf[4200];
    user_t *cur;
    sprintf(buf,"[Server] %s yell %s\n",sender->name,msg);
    cur = root;
    while(cur != NULL){
        write(cur->fd,buf,strlen(buf));
        cur = cur->next;
    }
}

int main(){
    int listenFd;
    struct sockaddr_in serverAddr;
    socklen_t serverAddrLen;
    char helloMsg[60];
    user_t *root=NULL,*newUser,*cur=NULL;
    int connfd;
    struct sockaddr_in clientAddr;
    fd_set rfds,allset;
    char recBuf[4096];
    size_t recLen=4096;
    const size_t bufferSize=4096;
    int maxfdp=0;

    FD_ZERO(&rfds);

    listenFd = socket(AF_INET,SOCK_STREAM,0);
    serverAddrLen = sizeof(serverAddr);
    memset(&serverAddr,0,serverAddrLen);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(0);

    bind(listenFd,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    if(getsockname(listenFd,(struct sockaddr*)&serverAddr,(socklen_t*)&serverAddrLen) == 0){
        printf("Binding on: %s:%u\n",inet_ntoa(serverAddr.sin_addr),ntohs(serverAddr.sin_port));
    }else{
        fprintf(stderr,"Binding error: %d\n",errno);
    }

    listen(listenFd,LISTENQ);
    maxfdp = listenFd;
    FD_ZERO(&allset);
    FD_SET(listenFd,&allset);

    while(1){
        rfds = allset;
        select(maxfdp+1,&rfds,NULL,NULL,NULL);
        clearBuf(recBuf,4096);
        if(FD_ISSET(listenFd,&rfds)){
            socklen_t clientAddrLen=sizeof(clientAddr);
            if( (connfd = accept(listenFd,(struct sockaddr*)NULL,NULL)) != -1 ){
                if(getpeername(connfd,(struct sockaddr*)&clientAddr,&clientAddrLen) == 0){
                    newUser = NewUser(connfd,inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port),"anonymous");
                    FD_SET(connfd,&allset);
                    if(connfd > maxfdp) maxfdp = connfd;
                    if(newUser != NULL){
                        sprintf(helloMsg,HELLO_TO_NEW,newUser->addr,newUser->port);
                        cur = root;
                        while(cur != NULL){
                            write(cur->fd,HELLO_TO_EXISTED,strlen(HELLO_TO_EXISTED));
                            cur = cur->next;
                        }
                        root = AddUserToList(root,newUser);
                        write(connfd,helloMsg,strlen(helloMsg));
                        continue;
                    }
                }
            }
            printf("[Error] create new user failed\n");
        }else{
            cur = root;
            while(cur != NULL){
                if(FD_ISSET(cur->fd,&rfds)){
                    if((recLen = read(cur->fd,recBuf,bufferSize)) == 0){
                        user_t *temp = cur;
                        sprintf(recBuf,"[Server] %s is offline.\n",cur->name);
                        FD_CLR(cur->fd,&allset);
                        close(cur->fd);
                        root = RemoveUserFromList(root,cur);
                        {// notify online user someone is offline
                            user_t *temp=root;
                            while(temp != NULL){
                                write(temp->fd,recBuf,strlen(recBuf));
                                temp = temp->next;
                            }
                        }
                        cur = cur->next;
                    }else{
                        char *spacePtr,*cmd,*param;
                        recBuf[strlen(recBuf)-1] = '\0';
                        if(recBuf[strlen(recBuf) -1 ] == '\r'){
                            recBuf[strlen(recBuf) -1] = '\0';
                        }
                        if(strlen(recBuf) == 0){
                            continue;
                        }
                        printf("%s\n",recBuf);
                        if(strcmp(recBuf,"who") == 0){
                            user_t *curNode_temp=root;
                            while(curNode_temp != NULL){
                                sprintf(recBuf,"[Server] %s %s/%u\n",curNode_temp->name,curNode_temp->addr,curNode_temp->port);
                                if(UserEqual(curNode_temp,cur)){
                                    recBuf[strlen(recBuf)-1] = '\0';
                                    strcat(recBuf," ->me\n");
                                }
                                write(cur->fd,recBuf,strlen(recBuf));
                                curNode_temp = curNode_temp->next;
                            }
                        }else{
                            spacePtr = strchr(recBuf,' ');
                            if(spacePtr != NULL){
                                cmd = strndup(recBuf,spacePtr-recBuf);
                                param = strndup(spacePtr+1,strlen(recBuf) - (spacePtr-recBuf));
                                if(strcmp(cmd,"name") == 0){
                                    char *outputMsg[2];
                                    outputMsg[0] = NULL;
                                    outputMsg[1] = NULL;
                                    if(ChangeUserName(root,cur,param,outputMsg)){
                                        user_t *curNode_temp=root;
                                        while(curNode_temp != NULL){
                                            if(!UserEqual(curNode_temp,cur)){
                                                write(curNode_temp->fd,outputMsg[1],strlen(outputMsg[1]));
                                            }
                                            curNode_temp = curNode_temp->next;
                                        }
                                    }
                                    write(cur->fd,outputMsg[0],strlen(outputMsg[0]));
                                    {
                                        int i;
                                        for(i=0;i<2;i++){
                                            if(outputMsg[i] != NULL){
                                                free(outputMsg[i]);
                                            }
                                        }
                                    }
                                }else if(strcmp(cmd,"tell") == 0){
                                    send_private_message(root,cur,param);
                                }else if(strcmp(cmd,"yell") == 0){
                                    broadcast_message(root,cur,param);
                                }else{
                                    clearBuf(recBuf,4096);
                                    strcpy(recBuf,"[Server] ERROR: Error command.\n");
                                    write(cur->fd,recBuf,strlen(recBuf));
                                }
                                free(cmd);
                                free(param);
                            }else{
                                clearBuf(recBuf,4096);
                                strcpy(recBuf,"[Server] ERROR: Error command.\n");
                                write(cur->fd,recBuf,strlen(recBuf));
                            }
                        }
                    }
                    break;
                }else{
                    cur = cur->next;
                }
            }
        }
    }
    close(listenFd);

    return 0;
}
