#include "user.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int UserEqual(user_t *user1,user_t *user2){
    if((user1->fd == user2->fd) && (strcmp(user1->addr,user2->addr) == 0)
            && (user1->port == user2->port) && (strcmp(user1->name,
            user2->name) == 0)){
        return 1;
    }else{
        return 0;
    }
}

user_t* NewUser(int fd,char *addr,unsigned short port,char *name){
    user_t *user = malloc(sizeof(user_t));
    if(user == NULL){
        goto ret;
    }

    if(fd < 0 || addr == NULL || name == NULL){
        free(user);
        user = NULL;
        goto ret;
    }

    user->fd = fd;
    strcpy(user->addr,addr);
    user->port = port;
    strcpy(user->name,name);
    user->next = NULL;

ret:
    return user;
}

user_t* AddUserToList(user_t *root,user_t *newUser){
    user_t *cur = root;
    if(root == NULL){
        return newUser;
    }
    while(cur->next != NULL){
        cur = cur->next;
    }
    cur->next = newUser;
    return root;
}

user_t* RemoveUserFromList(user_t *root,user_t *user){
    user_t *cur=root,*prev=NULL;
    while(cur != NULL){
        if(UserEqual(cur,user)){
            if(prev == NULL){ // root Node match
                if(cur->next != NULL){
                    cur = cur->next;
                    free(root);
                    return cur;
                }else{
                    return NULL;
                }
            }else{
                prev->next = cur->next;
                free(cur);
                return root;
            }
        }
        prev = cur;
        cur = cur->next;
    }
    return root;
}

int CheckUserNameValid(user_t *root,char *name){
    user_t *cur=root;
    int len = strlen(name);
    if(len < 2 || len > 12){
        return -1;
    }else{
        int i,len=strlen(name);
        for(i=0;i<len;i++){
            if(!(name[i] >= 'a' && name[i] <= 'z') && !(name[i] >= 'A' && name[i] <= 'Z')){
                return -1;
            }
        }
    }
    if(strcmp(name,"anonymous") == 0){
        return -2;
    }
    while(cur != NULL){
        if(strcmp(cur->name,name) == 0){
            return -3;
        }
        cur = cur->next;
    }
    return 1;
}

int ChangeUserName(user_t *root,user_t *user,char *newName,char **outputMsg){
    int ret;
    char buf[80];
    if(!(strcmp(newName,"anonymous") == 0) && (strcmp(user->name,newName) == 0)){
        return 1;
    }
    ;
    if((ret=CheckUserNameValid(root,newName)) == 1){
        sprintf(buf,"[Server] You're now known as %s.\n",newName);
        *outputMsg = strdup(buf);
        sprintf(buf,"[Server] %s is now known as %s.\n",user->name,newName);
        *(outputMsg+1) = strdup(buf);
        strcpy(user->name,newName);
        return 1;
    }else{
        switch(ret){
            case -1:
                *outputMsg = strdup("[Server] ERROR: Username can only consists of 2~12 English letters.\n");
                break;
            case -2:
                *outputMsg = strdup("[Server] ERROR: Username cannot be anonymous.\n");
                break;
            case -3:
                sprintf(buf,"[Server] ERROR: %s has been used by others.\n",newName);
                *outputMsg = strdup(buf);
                break;
        }
        return 0;
    }
}
