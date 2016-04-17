#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <list>
#include "user.h"

using namespace std;

#define GUEST 0
#define USER 1
#define LOGIN 2

#define MAXONLINE 10
#define MAXBUFLEN 8192
#define THREADNUM 6

int highestsock = 0;
int SERVERPORT = 5000;
fd_set master;
fd_set readfds;
int listener = -1; // listen the connection request from client
list<pthread_t> tids;

//vector<int>& mile, long posL, long posH, int med

struct crackpara 
{
	int num;
	vector<int> * mile;
	long posL;
	long posH;
	int med;	
};

//vector<int>& mile, long posL, long posH, int low, int high
struct crack3para : public crackpara
{
	int low;
	int high;
};

vector<struct User> users;

const char* authorized_info = "                       -=-= AUTHORIZED USERS ONLY =-=-\n\
You are attempting to log into Database Cracking Server.\n\
Please be advised by continuing that you agree to the terms of the\n\
Computer Access and Usage Policy of Database Cracking Server.\n\n\n";

const char* cmd_list = "Commands supported:\n\
  select [attribute] from [table] where [predictate 1] and [predicate 2]\n\
													# query data in SQL format\n\
  exit                                       	    # quit the system\n\
  quit                                       	    # quit the system\n\
  help                                       	    # print this message\n\
  ?                                          	    # print this message\n";

void server_run();
void *handler(void *arg);
void *crack(void *arg);
vector<string> cmd_process(string cmd,int level,vector<struct User>::iterator uit);


// #include <thread>

// std::thread f(int a, int b, char c){

// }

// std::thread t(f, a ,b ,c);
// t.join();

vector<string> cmd_process(string cmd,int level,vector<struct User>::iterator uit)
{
	vector<string> order;
	string inst,para,str;
	stringstream ss;

	ss<<cmd;
	ss>>inst;

    if(level==GUEST)
    {
		if(inst.compare("quit")==0 || inst.compare("exit")==0)
        {
            order.push_back(inst);
        }
        else if(inst.compare("register")==0)
        {
            int i;
            order.push_back(inst);
            i = cmd.find("register");
            cmd = cmd.erase(0,i+8);

            ss.clear();  // reset stringstream
            ss.str("");

            para = "";
            ss<<cmd;
            ss>>para;
            if(!para.empty()) // has paras
            {
                order.push_back(para); // get username

                i = cmd.find(para);
                cmd = cmd.erase(0,i+para.size());

                ss.clear();  // reset stringstream
                ss.str("");
                para = "";
                ss<<cmd;
                ss>>para;
                if(!para.empty())
                    order.push_back(para); // get password
            }
        }
    }
	else if(level==LOGIN)
	{
		uit->state |= ONLINE;
		if((inst.compare("exit")) == 0 || (inst.compare("quit")) == 0 ||
				(inst.compare("help")) == 0 || (inst.compare("?")) == 0)
			order.push_back(inst);

		if(inst.compare("select")==0) // select [attribute] from [table] where [pred1] and [pred2]
		{
			string attr, table, pred1, pred2, keyword, next;
			if (!(ss >> attr >> keyword >> table)){
				order.clear();
			}

			if(keyword.compare("from")==0) // make sure the command format: has "from"
			{
				order.push_back(inst);
				order.push_back(attr);
				order.push_back(table);

				ss >> next;
				if(next.compare("where")==0)  // handle predicates
				{
					size_t  found = cmd.find("where");
					string paras = cmd.substr(found+5);
					if(paras != "")  // paras contains all predicates
					{
						ss.clear();  // reset stringstream
						ss.str("");
						found = paras.find("and");
						if (found != string::npos)  // there are 2 predicates
						{
							// parse first predicate
							pred1 = paras.substr(0,found-1);
							ss << pred1;
							string p1op1, p1op2, p1sig;   // the first , second operand and of predicate1
							ss >> p1op1 >> p1op2 >> p1sig;
							if(p1op1 != "" && p1op2!= "" && p1sig!="")
							{
								order.push_back(p1op1);
								order.push_back(p1op2);
								order.push_back(p1sig);
							}
							else
							{
								order.clear();
							}
							// parse second predicate
							ss.clear();  // reset stringstream
							ss.str("");
							pred2 = paras.substr(found+3);
							ss << pred2;
							string p2op1, p2op2, p2sig;   // the first , second operand and of predicate1
							ss >> p2op1 >> p2op2 >> p2sig;
							if(p2op1 != "" && p2op2!= "" && p2sig!="")
							{
								order.push_back(p2op1);
								order.push_back(p2op2);
								order.push_back(p2sig);
							}
							else
							{
								order.clear();
							}
						}
						else  // only 1 predicates
						{
							// parse first predicate
							pred1 = paras.substr(0,found-1);
							ss << pred1;
							string p1op1, p1op2, p1sig;   // the first , second operand and of predicate1
							ss >> p1op1 >> p1op2 >> p1sig;
							if(p1op1 != "" && p1op2!= "" && p1sig!="")
							{
								order.push_back(p1op1);
								order.push_back(p1op2);
								order.push_back(p1sig);
							}
							else
							{
								order.clear();
							}
						}
					}
					else
					{
						order.clear();
					}
				}
			}
		}

	}
	return order;
}

void *crack(void *arg)
{
	return NULL;
}

void *handler(void *arg)
{
	pthread_detach(pthread_self()); // the main thread won't be blocked even if the child doesn't finish
    char sendbuf[MAXBUFLEN] ={0};
    char recvbuf[MAXBUFLEN] = {0};
    string cmd;
    int n;
	int sockfd = *(int *)arg;  	// get the socket of the user
	vector<string> order; 		// store the user command
	vector<struct User>::iterator uit; // the iterator for vector users
	User user;

	int countrecv = 0; // count how many times the server recv from client
	int countguest = 0; // count lines as a guest
    int countuser = -1;
	int level = USER;

	// inform client connected
	sprintf(sendbuf,authorized_info);
	send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
	memset(sendbuf,'\0',MAXBUFLEN);
	sprintf(sendbuf,"username (guest):");
	 //int a = sprintf(sendbuf,"username (guest):");   // better way
	// sendbuf[a] = 0;
	send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);

	while(1)
	{
		n = recv(sockfd,recvbuf,MAXBUFLEN,0);
		if(n<0)
		{
			printf("Fail to read.\n");
			close(sockfd);
			pthread_exit(NULL);

		}
		else if(n==0)
		{
			printf("User quit.\n");  // ? STILL HAS QUESTION
			close(sockfd);
			pthread_exit(NULL);
		}
		else
		{
			countrecv++;
            cmd = string(recvbuf);
            cmd = cmd.substr(0,n-2); // get the command and remove the newline
			printf("command:%s\n",cmd.c_str());
			if(countrecv==1 && cmd.size()==0)
				level = GUEST;

			//---------------------------------Guest--------------------------
			if(level==GUEST)
			{
				if(cmd.size()==0) // get cmd: newline
				{
					if(countrecv==1) // should show Commands
					{
						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,cmd_list);
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);

						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"You log in as a guest. The only command you can use is\n'register username password'\n\n");
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);

						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"<guest: %d>",countguest);
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
					}
					else
					{
						sprintf(sendbuf,"You are not supposed to do this.\nYou can only use 'register username password' as a guest.\n");
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
						memset(sendbuf,'\0',MAXBUFLEN);

						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"<guest: %d>",++countguest);
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
					}
				}
				else // get cmd: register (have to check cmd)
				{
					order = cmd_process(cmd,GUEST,uit);	//check the command's format
					if(!order.empty())
					{
						//----guest register------
						if(order[0].compare("register")==0)  // new user register
						{
							bool registered = false;
							if(order.size()<3)
							{
								memset(sendbuf,'\0',MAXBUFLEN);
								sprintf(sendbuf,"Parameters are insufficient! Please give out username and password.\n");
								send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							}
							else
							{
								for(uit=users.begin();uit!=users.end();uit++)
								{
									if(order[1].compare(uit->name)==0)
									{
										registered = true;
										break;
									}
								}
								if(registered) // the name has already be registered
								{
									memset(sendbuf,'\0',MAXBUFLEN);
									sprintf(sendbuf,"%s has already been registered! Please change a username.\n",order[1].c_str());
									send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
								}
								else
								{
									user.name = order[1];
									user.pwd = order[2];
									user.sockfd = -1;
									users.push_back(user);
									//----------------------inform user register succeed-----------------------
									memset(sendbuf,'\0',MAXBUFLEN);
									sprintf(sendbuf,"User %s registered!\n",order[1].c_str());
									send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
									printf("User %s registered!\n",order[1].c_str());
								}
							}
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"<guest: %d>",++countguest);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
						}
						//----guest quit------
						else if(order[0].compare("quit")==0 || order[0].compare("exit")==0)
						{
							printf("Guest quit.\n");
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Thank you for using Database Cracking Server.\nSee you next time.\n\n");
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							close(sockfd);
							pthread_exit(NULL);
						}
					}
					else
					{
						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"You are not supposed to do this.\nYou can only use 'register username password' as a guest.\n");
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
						printf("Invalid command!\n");
						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"<guest: %d>",++countguest);
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
					}
					order.clear();
				}
			}
			//---------------------------------User--------------------------
			else if(level==USER)
			{
				//-----------------------Try Login---------------------
				if(countrecv==1)
				{
					bool isauser = false;
					for(uit = users.begin();uit!=users.end();uit++)
					{
						if(uit->name.compare(cmd)==0)
						{
							isauser = true;
							break;
						}
					}
					if(isauser==false)  // can't find the user
					{
						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"The user doesn't exit!\nThank you for using Database Cracking Server.\nSee you next time.\n");
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
						close(sockfd);
						pthread_exit(NULL);
					}
					else // found the user
					{
						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"password:");
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
					}
				}
                else if(countrecv==2) // check the password
                {
                    if(uit->pwd.compare(cmd)==0) //password match
                    {
                        if(uit->state & ONLINE) // if the user is already login somewhere
                        {
                            int pastsockfd = uit->sockfd; //record the past sockfd
                            uit->state ^= ONLINE;
                            memset(sendbuf,'\0',MAXBUFLEN);
                            sprintf(sendbuf,"You are not in the room.\nYou login from another place.\nThank you for using Database Cracking Server.\nSee you next time.\n\n");
                            send(pastsockfd,(void*)(sendbuf),sizeof(sendbuf),0);
                            close(pastsockfd);
                        }
                        uit->sockfd = sockfd;
                        uit->state |= ONLINE;
                        level = LOGIN;
                        memset(sendbuf,'\0',MAXBUFLEN);
                        sprintf(sendbuf,"%s now login!\n",uit->name.c_str());
                        send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);

                        memset(sendbuf,'\0',MAXBUFLEN);
                        sprintf(sendbuf,cmd_list);
                        send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);

                        memset(sendbuf,'\0',MAXBUFLEN);
                        sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
                        send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);

                    }
                    else //password doesn't match
                    {
						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"Login failed!\nThank you for using Database Cracking Server.\nSee you next time.\n");
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
						close(sockfd);
						pthread_exit(NULL);
                    }
                }
			}
            else if(level==LOGIN)
            {
                if(cmd.size()==0) //newline
                {
                    memset(sendbuf,'\0',MAXBUFLEN);
                    sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
                    send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
                }
				else
				{
					order = cmd_process(cmd,LOGIN,uit); //check the command and format
                    if(order.empty())  // the command is invalid
                    {
						memset(sendbuf,'\0',MAXBUFLEN);
                        sprintf(sendbuf,"Invalid command!\n");
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
                        printf("Invalid command!\n");
                    }
					// supported commands
					else
					{
                        if(order[0].compare("quit")==0||order[0].compare("exit")==0)
                        {
                            uit->state ^= ONLINE;
                            uit->sockfd = -1;
                            printf("User %s quit.\n",uit->name.c_str());
                            memset(sendbuf,'\0',MAXBUFLEN);
                            sprintf(sendbuf,"Thank you for using Database Cracking Server.\nSee you next time.\n\n");
                            send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
                            close(sockfd);
                            pthread_exit(NULL);
                        }
                        else if(order[0].compare("help")==0||order[0].compare("?")==0)
                        {
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,cmd_list);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
                        }
						else if(order[0].compare("select")==0) // select [attribute] from [table] where [pred1] and [pred2]
						{
							string attr = order[1];
							string table = order[2];
							string lop1,lop2, rop1, rop2, sign1, sign2;
							crackpara cp;
							if(order.size()==3) // no predicates: select [attr] from [table] 
							{
								
							}
							else if(order.size()==6) // only pred1: select [attr] from [table] where 
							{
								lop1 = order[3];
								sign1 = order[4];
								rop1 = order[5];	
								// calculate para
								cp.num = 2;
								
															
							}
							else if(order.size()==9) // pred1 and pred2
							{
								lop1 = order[3];
								sign1 = order[4];
								rop1 = order[5];
								
								lop2 = order[6];
								sign2 = order[7];
								rop2 = order[8];	
								// calculate para
								cp.num  = 3;
							}
							
							//create 6 threads
							// int rank[THREADNUM];
							pthread_t crackers[THREADNUM];
							int isSend = 0;
							for(int i=0;i<THREADNUM;i++)
							{
								// rank[i] = i;
								if(pthread_create(&crackers[i], NULL, crack, (void *)&cp))
								{																			
										break;																	
								}
								isSend++;
							}
							
							// join
							for(int i=0;i<isSend;i++)
							{
								pthread_join(crackers[i], NULL);					
							}				
							if (isSend!=THREADNUM){
								memset(sendbuf,'\0',MAXBUFLEN);
								sprintf(sendbuf,"Execute query fails: fail to create threads.\n");
								send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);		
								continue;
							}
							// merge();
							vector<int> queryres;// = //merge
							// send to client	
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Execute query result:\n");
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);							
							for(int i=0;i<(int)queryres.size();i++)
							{
								memset(sendbuf,'\0',MAXBUFLEN);
								sprintf(sendbuf,"%d\n",queryres[i]);
								send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);									
							}
						}
						if(order.empty())
						{
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
						}
					}
				}
				order.clear();
			}
		}
	}
}

void read_from_socket()
{
	struct sockaddr_in client_addr;
	int newfd;
	socklen_t addrlen;
	pthread_t tid;
	char cmd[MAXBUFLEN];

	if(FD_ISSET(listener,&readfds))   // if receive connection request
	{
		addrlen = sizeof(client_addr);
		if((newfd = accept(listener,(struct sockaddr*)&client_addr,&addrlen))==-1)
		{
			printf("Fail to accept a new connection.\n");
		}
		else
		{
			if(pthread_create(&tid,NULL,&handler,&newfd)!=0)
				printf("Fail to create new a thread.\n");
			}
			tids.push_back(tid);

	}
	if(FD_ISSET(fileno(stdin),&readfds)) // if receive quit from stdin
	{
		fgets(cmd,sizeof(cmd),stdin);
		if(cmd[strlen(cmd)-1]=='\n')
			cmd[strlen(cmd)-1] = '\0';
		string comm = string(cmd);
		if(comm.compare("quit")==0)
		{
			cout<<"Server quit."<<endl;
			exit(0);
		}
	}
}


void server_run()
{
	struct sockaddr_in server_addr;

	FD_ZERO(&master);
	FD_ZERO(&readfds);

	//create listener for the connection request
	if((listener = socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("Fail to create listener socket.\n");
		exit(-1);
	}

	// set listener address to be reused immediately after release
	int a=1;
	if(setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&a,sizeof(int))==-1)
	{
		perror("Fail to set socket option.\n");
		exit(-1);
	}

	//bind to the port
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVERPORT);
	memset(&(server_addr.sin_zero),'\0',8);
	if(bind(listener,(struct sockaddr *)&server_addr,sizeof(server_addr))==-1)
	{
		perror("Fail to bind to listener.\n");
		exit(-1);
	}

	if(listen(listener,MAXONLINE)<0)
	{
		perror("Fail to listen.\n");
		exit(-1);
	}

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
	else
	{
		SERVERPORT = atoi(argv[1]);  // get the server port
	}

	server_run();
	return 0;
}
