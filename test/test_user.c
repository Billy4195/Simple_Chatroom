#include "user.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>


void checkUserInfo(user_t *user,int fd,char *addr,unsigned short port,char *name){
    assert(user->fd == fd);
    assert(strcmp(user->addr,addr) == 0);
    assert(user->port == port);
    assert(strcmp(user->name,name) == 0);
}


void test_NewUser(void){
    user_t *user = NewUser(3,"192.168.0.1",3000,"anonymous");
    checkUserInfo(user,3,"192.168.0.1",3000,"anonymous");
}

void test_AddUserToList(void){
    user_t *rootNode=NULL,*cur=NULL;
    user_t *user1 = NewUser(3,"192.168.0.1",3000,"anonymous");
    user_t *user2 = NewUser(4,"192.168.0.2",3000,"anonymous");
    user_t *user3 = NewUser(5,"192.168.0.3",3000,"anonymous");

    rootNode = user1;
    AddUserToList(rootNode,user2);
    AddUserToList(rootNode,user3);

    cur = rootNode;
    checkUserInfo(cur,3,"192.168.0.1",3000,"anonymous");
    cur = cur->next;
    checkUserInfo(cur,4,"192.168.0.2",3000,"anonymous");
    cur = cur->next;
    checkUserInfo(cur,5,"192.168.0.3",3000,"anonymous");
}

void test_CheckUserValid(void){
    user_t *rootNode=NULL;
    user_t *user1 = NewUser(3,"192.168.0.1",3000,"anonymous");
    user_t *user2 = NewUser(4,"192.168.0.2",3000,"user2");
    user_t *user3 = NewUser(5,"192.168.0.3",3000,"user3");

    rootNode = user1;
    AddUserToList(rootNode,user2);
    AddUserToList(rootNode,user3);

    assert(CheckUserValid(rootNode,"user4") == 1);
    assert(CheckUserValid(rootNode,"x") == 0);
    assert(CheckUserValid(rootNode,"1234567890123") == 0);
    assert(CheckUserValid(rootNode,"anonymous") == 0);
}

int main(){
    test_NewUser();
    test_AddUserToList();
    test_CheckUserValid();
    return 0;
}
