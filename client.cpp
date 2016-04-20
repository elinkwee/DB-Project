#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <netdb.h>
#include <cerrno>
#include "user.h"
 

using namespace std;

#define RECV_SHOUT 0
#define RECV_TELL 1
#define LOGIN 2

#define MAXONLINE 10
#define MAXBUFLEN 8192

int highestsock = 0;

int listener = -1;

fd_set master;
fd_set readfds;

int countrecv = 0; // count how many times the client recv from server

void read_from_socket();
vector<string> cmd_process(string cmd);
int client_run();

void read_from_socket()
{
	char sendbuf[MAXBUFLEN];
    char recvbuf[MAXBUFLEN];

	int n;

	if(FD_ISSET(fileno(stdin),&readfds))
	{
        string cmd;

        memset(sendbuf,'\0',MAXBUFLEN);
        fgets(sendbuf,MAXBUFLEN,stdin);
		if(sendbuf[strlen(sendbuf)-1]=='\n')
			sendbuf[strlen(sendbuf)-1] = '\0';

		cmd = string(sendbuf);
        cmd = cmd + "\n"+"\n";
        send(listener,(void*)cmd.c_str(),cmd.size(),0);
        fflush(stdin);
    }
	if(FD_ISSET(listener,&readfds))
	{
		n = recv(listener,recvbuf,MAXBUFLEN,0);
		if(n<=0)
		{
			if(n==0)
			{
				printf("Connection closed by foreign host.\n");
				close(listener);
				FD_CLR(listener,&master);
				listener = -1;
				exit(0);
			}
			if(n<0)
			{
				printf("Receive error.\n");
				close(listener);
				FD_CLR(listener,&master);
				listener = -1;
				exit(-1);
			}
		}
		else
		{
			string print;
			string str = string(recvbuf);
			int found = str.find("|");
			if(found>0)
			{
				//string label = str.substr(0,found);
			}
			else
			{
				print = str.substr(0,n);
				cout<<print;
			}
            memset(recvbuf,'\0',MAXBUFLEN);
            fflush(stdout);
		}
	}
}

int client_run(char * SERVERIP, char * SERVERPORT)
{

	//--------------------connect to server--------------------
	struct addrinfo hints, *res, *rp;
	socklen_t addr_len = sizeof(struct addrinfo);
	memset(&hints, 0, addr_len);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;


	// get possible ports
	if (getaddrinfo(SERVERIP, SERVERPORT, &hints, &res) < 0){
		const int rc = errno;
		cerr << "Error: getaddrinfo failed: " << strerror(rc) << endl;
		return -1;
		// continue;
	}

	// try to bind with one interface
	for(rp = res; rp; rp = rp -> ai_next){
		cout << "Trying " << inet_ntoa(((struct sockaddr_in *) (rp -> ai_addr)) -> sin_addr) << ":" << ntohs(((struct sockaddr_in *) (rp -> ai_addr)) -> sin_port) << endl;

		if ((listener = socket(rp -> ai_family, rp -> ai_socktype, rp -> ai_protocol)) < 0){
			continue;
		}

		if (!connect(listener, rp -> ai_addr, rp -> ai_addrlen)){
			cout << "Connected to " << inet_ntoa(((struct sockaddr_in *) rp -> ai_addr) -> sin_addr) << ":" << ntohs(((struct sockaddr_in *) rp -> ai_addr) -> sin_port) << " (" << listener << ")" << endl;
			break;
		}
		close(listener);
	}

    freeaddrinfo(res);
	if (!rp)  // try all aliases and fail
	{
		return -1;
	}


	FD_ZERO(&master);
	FD_ZERO(&readfds);
	//--------------------connect to server--------------------	

	FD_SET(listener,&master);
	if(listener>highestsock)
	{
		highestsock = listener;
	}

	FD_SET(fileno(stdin),&master);
	if(fileno(stdin)>highestsock)
	{
		highestsock = fileno(stdin);
	}

	while(1)  // keep listen the connection request
	{
		readfds = master;
		if(select(highestsock+1,&readfds,NULL,NULL,NULL) == -1)
		{
			perror("Fail to select.\n");
			exit(-1);
		}
		read_from_socket();
	}
	close(listener);
}

int main(int argc, char **argv)
{
	if(argc<2)
	{
		perror("Insufficient parameter.\n");
		return 1;
	}
	// SERVERIP = argv[1];
	// SERVERPORT = argv[2];
	// memcpy(SERVERIP,argv[1], strlen(argv[1])); //get server IP
	// SERVERPORT = atoi(argv[2]); //get server port


	client_run(argv[1], argv[2]);

	return 0;
}