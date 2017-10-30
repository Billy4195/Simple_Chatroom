#include "user.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

void AddUserToList(user_t *root,user_t *newUser){
    user_t *cur = root;
    if(root == NULL){
        return;
    }
    while(cur->next != NULL){
        cur = cur->next;
    }
    cur->next = newUser;
}

int CheckUserValid(user_t *root,char *name){
    user_t *cur=root;
    int len = strlen(name);
    if(len < 2 || len > 12){
        return 0;
    }
    if(strcmp(name,"anonymous") == 0){
        return 0;
    }
    while(cur != NULL){
        if(strcmp(cur->name,name) == 0){
            return 0;
        }
        cur = cur->next;
    }
    return 1;
}
