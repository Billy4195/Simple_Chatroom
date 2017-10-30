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

int ChangeUserName(user_t *root,user_t *user,char *newName){
    if(strcmp(user->name,newName) == 0){
        return 1;
    }
    if(CheckUserNameValid(root,newName)){
        strcpy(user->name,newName);
        return 1;
    }else{
        return 0;
    }
}
