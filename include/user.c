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
    while(cur->next != NULL){
        cur = cur->next;
    }
    cur->next = newUser;
}