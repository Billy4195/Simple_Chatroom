#ifndef USER_H
#define USER_H

typedef struct User{
    int fd;
    char addr[16];
    unsigned short port;
    char name[13];
    struct User *next;
}user_t;

int UserEqual(user_t *user1,user_t *user2);
user_t* NewUser(int fd,char *addr,unsigned short port,char *name);
void AddUserToList(user_t *root,user_t *newUser);
user_t* RemoveUserFromList(user_t *root,user_t *user);
int CheckUserValid(user_t *root,char *name);

#endif
