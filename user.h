#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <sys/socket.h>
#include <sys/types.h>
#include <cctype>
using namespace std;

#define ONLINE 0x00000001                          //online
#define BEBLOCKED 0x00000002                 	   //blocked
#define QUIET 0x00000004                           //quite mode

struct User
{
	string name;
	string pwd;
	string info;
	int sockfd;   //present socket belonging to the user
	int state;
    int countline;        
	User():name(""), pwd(""), info("<none>"),sockfd(-1), state(0),countline(0){} 
};





