#ifndef USER_H
#define USER_H

typedef struct User{
    int fd;
    char addr[16];
    unsigned short port;
    char name[13];
    struct User *next;
}user_t;

user_t* NewUser(int fd,char *addr,unsigned short port,char *name);
void AddUserToList(user_t *root,user_t *newUser);

#endif
