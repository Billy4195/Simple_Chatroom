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

void test_UserEqual(void){
    user_t *user1 = NewUser(3,"192.168.0.1",3000,"anonymous");
    user_t *user2 = NewUser(4,"192.168.0.2",3000,"anonymous");
    assert(UserEqual(user1,user1) == 1);
    assert(UserEqual(user1,user2) == 0);
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

user_t* test_RemoveUserFromList(){
    int i;
    user_t *rootNode=NULL,*cur=NULL;
    user_t *userArr[4];
    userArr[0] = NewUser(3,"192.168.0.1",3000,"anonymous");
    userArr[1] = NewUser(4,"192.168.0.2",3000,"anonymous");
    userArr[2] = NewUser(5,"192.168.0.3",3000,"anonymous");
    userArr[3] = NewUser(6,"192.168.0.3",3000,"ForgetMyname");

    rootNode = userArr[0];
    for(i=1;i<3;i++){
        AddUserToList(rootNode,userArr[i]);
    }
    // Check user list is correct
    cur = rootNode;
    for(i=0;i<3;i++){
        assert(UserEqual(cur,userArr[i]) == 1);
        cur = cur->next;
    }
    assert(cur == NULL);
    // Remove user is not in list
    rootNode = RemoveUserFromList(rootNode,userArr[3]);
    cur = rootNode;
    for(i=0;i<3;i++){
        assert(UserEqual(cur,userArr[i]) == 1);
        cur = cur->next;
    }
    assert(cur == NULL);
    // Remove user is not root node
    rootNode = RemoveUserFromList(rootNode,userArr[1]);
    assert(UserEqual(rootNode,userArr[0]) == 1);
    assert(UserEqual(rootNode->next,userArr[2]) == 1);
    assert(rootNode->next->next == NULL);
    // Remove user is root node
    rootNode = RemoveUserFromList(rootNode,userArr[0]);
    assert(UserEqual(rootNode,userArr[2]) == 1);
    assert(rootNode->next == NULL);
}

void test_CheckUserNameValid(void){
    user_t *rootNode=NULL;
    user_t *user1 = NewUser(3,"192.168.0.1",3000,"anonymous");
    user_t *user2 = NewUser(4,"192.168.0.2",3000,"user");
    user_t *user3 = NewUser(5,"192.168.0.3",3000,"ForgetMyname");

    rootNode = user1;
    AddUserToList(rootNode,user2);
    AddUserToList(rootNode,user3);

    assert(CheckUserNameValid(rootNode,"John") == 1);
    assert(CheckUserNameValid(rootNode,"x") == -1);
    assert(CheckUserNameValid(rootNode,"DictionaryMan") == -1);
    assert(CheckUserNameValid(rootNode,"123") == -1);
    assert(CheckUserNameValid(rootNode,"anonymous") == -2);
    assert(CheckUserNameValid(rootNode,"ForgetMyname") == -3);
}

void test_ChangeUserName(void){
    int i;
    char *outputMsg[2];
    user_t *rootNode=NULL,*cur=NULL;
    user_t *userArr[4];
    userArr[0] = NewUser(3,"192.168.0.1",3000,"anonymous");
    userArr[1] = NewUser(4,"192.168.0.2",3000,"anonymous");
    userArr[2] = NewUser(5,"192.168.0.3",3000,"anonymous");
    userArr[3] = NewUser(6,"192.168.0.3",3000,"Justine");

    rootNode = userArr[0];
    for(i=1;i<3;i++){
        AddUserToList(rootNode,userArr[i]);
    }

    assert(ChangeUserName(rootNode,userArr[0],"Peter",outputMsg) == 1);
    assert(ChangeUserName(rootNode,userArr[0],"Peter",outputMsg) == 1);
    assert(ChangeUserName(rootNode,userArr[1],"anonymous",outputMsg) == 0);
    assert(ChangeUserName(rootNode,userArr[1],"Peter",outputMsg) == 0);
    assert(ChangeUserName(rootNode,userArr[1],"x",outputMsg) == 0);
    assert(ChangeUserName(rootNode,userArr[1],"DictionaryMan",outputMsg) == 0);
    assert(ChangeUserName(rootNode,userArr[1],"123",outputMsg) == 0);

    assert(strcmp(rootNode->name,"Peter") == 0);
}

int main(){
    test_UserEqual();
    test_NewUser();
    test_AddUserToList();
    test_RemoveUserFromList();
    test_CheckUserNameValid();
    test_ChangeUserName();
    return 0;
}
