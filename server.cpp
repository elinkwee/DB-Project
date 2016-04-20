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
#include <cerrno>
#include "user.h"
#include "gendata.h"
#include "expdata.h"

using namespace std;

extern const long N;

extern vector<string> VIN;
extern vector<int> mile;
extern vector<string> make;
extern vector<string> color;
extern vector<int> year;

extern vector<string> VINOrig;
extern vector<int> mileOrig;
extern vector<string> makeOrig;
extern vector<string> colorOrig;
extern vector<int> yearOrig;

extern vector<string> VINSort;
extern vector<int> mileSort;
extern vector<string> makeSort;
extern vector<string> colorSort;
extern vector<int> yearSort;

//extern vector<int> mileSort;

#define GUEST 0
#define USER 1
#define LOGIN 2
#define HASDATA 3

#define MAXONLINE 10
#define MAXBUFLEN 8192
#define THREADNUM 1
#define THREADNUM2 1

int highestsock = 0;
int SERVERPORT = 5000;
fd_set master;
fd_set readfds;
int listener = -1; // listen the connection request from client
list<pthread_t> tids;

//vector<int>& mile, long posL, long posH, int med

   
vector<struct User> users;

const char* authorized_info = "                       -=-= AUTHORIZED USERS ONLY =-=-\n\
You are attempting to log into Database Cracking Server.\n\
Please be advised by continuing that you agree to the terms of the\n\
Computer Access and Usage Policy of Database Cracking Server.\n\n\n";

const char* cmd_list = "Commands supported:\n\
  register <username> <password>  # register a new user in the system\n\
  gendata <number>                # generate <number> table data\n\
                                  <number> size is better be no more than 1,000,000\n\
  readdata                        # read table data into memory\n\
  select <attribute> from <table> where [predictate 1] and [predicate 2]\n\
                                  # query data in SQL format\n\
								  # Notice : in [predicate] there must be spaces between\n\
								  # operands and the operator\n\
  printitem <number>              # print out the top <number> items in table\n\
                                    <number> size should be between [0,100]\n\
  sorting                         # Use merge sort to sort the whole data and record time\n\
  exit                            # quit the system\n\
  quit                            # quit the system\n\
  help                            # print this message\n\
  ?                               # print this message\n";
void server_run();
void *handler(void *arg);
vector<string> cmd_process(string cmd,int level,vector<struct User>::iterator uit);


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
            if (!(ss>>para))
			{
				printf("Lack of parameters\n");
			}
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
		else if(inst.compare("select")==0)  // dont have to do detailed parse for select in LOGIN level
		{
			order.push_back(inst);
		}
		else if(inst.compare("gendata")==0)
		{
			order.push_back(inst);
			string num;
			ss >> num;
			if(num!="")
			{
				order.push_back(num);
			}
			else
			{
				order.clear();
			}			
		}
		else if(inst.compare("readdata")==0)
		{
			order.push_back(inst);
		}
		else if(inst.compare("printitem")==0)
		{
			order.push_back(inst);
			string num;
			ss >> num;
			if(num!="")
			{
				order.push_back(num);
			}
		}
		else if(inst.compare("sorting")==0)
		{
			order.push_back(inst);
		}
	}
	else if(level==HASDATA)
	{
		uit->state |= ONLINE;
		if((inst.compare("exit")) == 0 || (inst.compare("quit")) == 0 ||
				(inst.compare("help")) == 0 || (inst.compare("?")) == 0)
			order.push_back(inst);

		else if(inst.compare("select")==0) // select [attribute] from [table] where [pred1] and [pred2]
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
		else if(inst.compare("gendata")==0)
		{
			order.push_back(inst);
			string num;
			ss >> num;
			if(num!="")
			{
				order.push_back(num);
			}				
		}
		else if(inst.compare("readdata")==0)
		{
			order.push_back(inst);
		}
		else if(inst.compare("printitem")==0)
		{
			order.push_back(inst);
			string num;
			ss >> num;
			if(num!="")
			{
				order.push_back(num);
			}
			else
			{
				order.clear();
			}
		}
		else if(inst.compare("sorting")==0)
		{
			order.push_back(inst);
		}	
	}
	return order;
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
	
	int pivot[2] = {-1};
	string signs[2] = {""};
	int type = 0;
	long int datanum = -1;

	// inform client connected
	sprintf(sendbuf,"%s", authorized_info);
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
						sprintf(sendbuf,"%s", cmd_list);
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
                        sprintf(sendbuf,"%s", cmd_list);
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
						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
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
							sprintf(sendbuf,"%s", cmd_list);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							order.clear();
							
                        }
						else if(order[0].compare("select")==0)
						{
                            memset(sendbuf,'\0',MAXBUFLEN);
                            sprintf(sendbuf,"You cannot do query now, please do gendata and readdata first.\n");
                            send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							
						}
						else if(order[0].compare("gendata")==0)
						{
							
							if(order.size()==1)
							{
								datanum = N;
								memset(sendbuf,'\0',MAXBUFLEN);
								sprintf(sendbuf,"Now generating %ld table items...\n",datanum);
								send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);								
								gen_data(N);
							}
							else if(order.size()==2)
							{
								stringstream ss;
								datanum = -1;
								ss << order[1];
								ss >> datanum;
								if(datanum>0)
								{
									memset(sendbuf,'\0',MAXBUFLEN);
									sprintf(sendbuf,"Now generating %ld table items...\n",datanum);
									send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);									
									gen_data(datanum);
								}
								else
								{
									memset(sendbuf,'\0',MAXBUFLEN);
									sprintf(sendbuf,"Invalid command. Data number is invalid.\n");
									send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);									
								}								
							}

							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Finish generating %ld table items.\n",datanum);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							order.clear();

						}
						else if(order[0].compare("readdata")==0)
						{
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Now reading %ld table items into memory...\n",datanum);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);

							read(VIN, mile, make, color, year);
							read(VINOrig, mileOrig, makeOrig, colorOrig, yearOrig);

							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Finish reading %ld table itemsinto memory.\n",datanum);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							
							level = HASDATA;
							
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							order.clear();
						}						
						else if(order[0].compare("sorting")==0)
						{
							read(VINSort, mileSort, makeSort, colorSort, yearSort);
							double time  =MergeSort(mileSort);
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Sorting execution time %f second.\n", time);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);							
						}
					}
				}
			}
			else if(level==HASDATA)
            {
                if(cmd.size()==0) //newline
                {
                    memset(sendbuf,'\0',MAXBUFLEN);
                    sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
                    send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
                }
				else
				{
					order = cmd_process(cmd,HASDATA,uit); //check the command and format
                    if(order.empty())  // the command is invalid
                    {
						memset(sendbuf,'\0',MAXBUFLEN);
                        sprintf(sendbuf,"Invalid command!\n");
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
                        printf("Invalid command!\n");
						memset(sendbuf,'\0',MAXBUFLEN);
						sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
						send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
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
							sprintf(sendbuf,"%s", cmd_list);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
                        }
						else if(order[0].compare("select")==0) // select [attribute] from [table] where [pred1] and [pred2]
						{
							string attr = order[1];
							string table = order[2];
							string lop1,lop2, rop1, rop2, sign1, sign2;
							crack2para cp2;
							crack3para cp3;
							if(order.size()==3) // type 0 no predicates: select [attr] from [table]
							{
								type = 0;
								//print all asked column data here
							}
							else if(order.size()==6) // type 1 only pred1: select [attr] from [table] where
							{
								type = 1;
								lop1 = order[3];
								sign1 = order[4];
								rop1 = order[5];
								// calculate para
								//cp2.mile = &mile;
								stringstream ss;
								ss << rop1;
								ss >> cp2.med;
								cp2.sign = sign1;
								
								pivot[0] = cp2.med;
								signs[0] = cp2.sign;
							}
							else if(order.size()==9) // type 2 pred1 and pred2
							{
								type = 2;
								lop1 = order[3];
								sign1 = order[4];
								rop1 = order[5];

								lop2 = order[6];
								sign2 = order[7];
								rop2 = order[8];
								// calculate para
								//cp3.mile = &mile;
								stringstream ss;
								ss << rop1;
								ss >> cp3.low;
								ss.clear();
								ss.str("");
								ss << rop2;
								ss >> cp3.high;
								cp3.sign1 = sign1;
								cp3.sign2 = sign2;
								
								pivot[0] = cp3.low;
								pivot[1] = cp3.high;
								signs[0] = cp3.sign1;
								signs[1] = cp3.sign2;
							}

							//create 6 threads
							pthread_t crackers[THREADNUM];
							int isSend = 0;
							int block = mile.size()/THREADNUM;
							//int endblock = mile.size() - (THREADNUM-1) * block;
							if(order.size()==6)
							{
								for(int i=0;i<THREADNUM;i++)
								{
									// calculate posL and posR
									if(i!=THREADNUM-1)
									{
										cp2.posL = i * block;
										cp2.posH = (i+1) * block - 1;
									}
									else // the last thread
									{
										cp2.posL = i * block;
										cp2.posH = mile.size() - 1;
									}
									if(pthread_create(&crackers[i], NULL, CrackInTwo, (void *)&cp2))
									{
										break;
									}
									isSend++;
									printf("thread %d created\n",i);
								}
							}
							else if(order.size()==9)
							{
								for(int i=0;i<THREADNUM2;i++)
								{
									if(i!=THREADNUM2-1)
									{
										cp3.posL = i * block;
										cp3.posH = (i+1) * block - 1;
									}
									else // the last thread
									{
										cp3.posL = i * block;
										cp3.posH = mile.size() - 1;
									}
									// calculate posL and posR
									// cout<<cp3.posL<<" "<<cp3.posH<<endl;
									// cout <<cp3.low<<" "<<cp3.high<<endl;
									if(pthread_create(&crackers[i], NULL, CrackInThree, (void *)&cp3))
									{
										int err = errno;
										std::cout << strerror(err) << std::endl;
										break;
									}
									isSend++;
								}
							}
							// join
							void* rtn;
							double exetime = -1;
							for(int i=0;i<isSend;i++)
							{
								pthread_join(crackers[i], &rtn);
								double time = *(double *)rtn;
								delete (double *)rtn;
								if(time > exetime)
								{
									exetime = time;
								}
							}
							if(type==1)
							{
								if (isSend!=THREADNUM){
									memset(sendbuf,'\0',MAXBUFLEN);
									sprintf(sendbuf,"Execute query fails: fail to create threads.\n");
									send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
									continue;
								}
							}
							else if(type==2)
							{
								if (isSend!=THREADNUM2){
									memset(sendbuf,'\0',MAXBUFLEN);
									sprintf(sendbuf,"Execute query fails: fail to create threads.\n");
									send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
									continue;
								}																
							}

							// merge();
							if(order.size()==6)
							{
								exetime += MergeInTwo(cp2.med);
							}
							else if(order.size()==6)
							{
								exetime += MergeInThree(cp3.low, cp3.high);
							}

							// send to client query execute time
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Execute time of the query: %f\n",exetime);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);

							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);

							// for(int i=0;i<(int)queryres.size();i++)
							// {
								// memset(sendbuf,'\0',MAXBUFLEN);
								// sprintf(sendbuf,"%d\n",queryres[i]);
								// send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							// }
						}
						else if(order[0].compare("gendata")==0)
						{
							if(order.size()==1)
							{
								datanum = N;
								memset(sendbuf,'\0',MAXBUFLEN);
								sprintf(sendbuf,"Now generating %ld table items...\n",datanum);
								send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);								
								gen_data(N);
							}
							else if(order.size()==2)
							{
								stringstream ss;
								datanum = -1;
								ss << order[1];
								ss >> datanum;
								if(datanum>0)
								{
									memset(sendbuf,'\0',MAXBUFLEN);
									sprintf(sendbuf,"Now generating %ld table items...\n",datanum);
									send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);									
									gen_data(datanum);
								}
								else
								{
									memset(sendbuf,'\0',MAXBUFLEN);
									sprintf(sendbuf,"Invalid command. Data number is invalid.\n");
									send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);									
								}								
							}
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Finish generating %ld table items.\n",datanum);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
                            
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);                            
						}
						else if(order[0].compare("readdata")==0)
						{
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Now reading %ld table items into memory...\n",datanum);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);

							read(VIN, mile, make, color, year);
							read(VINOrig, mileOrig, makeOrig, colorOrig, yearOrig);

							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Finish reading %ld table itemsinto memory.\n",datanum);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
							order.clear();

						}
						else if(order[0].compare("printitem")==0)
						{							
							if(type == 1)
							{							
								stringstream ss;
								int num;
								ss << order[1];
								ss >> num;
								if(num>0 and num <=100)
								{							
									vector<int> resindex;
									FindIndexTwo(mileOrig, pivot[0], signs[0], resindex);
									int pnum;
									if((int)resindex.size()>=num)
									{
										pnum = num;
									}
									else 
									{
										pnum = resindex.size();
									}
									for(int i=0;i<pnum;i++)
									{
										memset(sendbuf,'\0',MAXBUFLEN);
										sprintf(sendbuf,"[%d] %s %d %s %s %d\n", i+1, 
										VINOrig[ resindex[i] ].c_str(),
										mileOrig[ resindex[i] ] ,
										makeOrig[ resindex[i] ].c_str(),
										colorOrig[ resindex[i] ].c_str(),
										yearOrig[ resindex[i] ]);
										send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
										
										cout <<"[" << i+1 <<"] "
											<< VINOrig[ resindex[i] ] << " "
											<< mileOrig[ resindex[i] ] << " "
											<< makeOrig[ resindex[i] ] << " "
											<< colorOrig[ resindex[i] ] << " "
											<< yearOrig[ resindex[i] ] << endl;															
									}
								}
								else
								{
									memset(sendbuf,'\0',MAXBUFLEN);
									sprintf(sendbuf,"Number out of range. The supported print number is between 0 and 100.\n");
									send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);										
								}
							}
							else if(type == 2)
							{							
								stringstream ss;
								int num;
								ss << order[1];
								ss >> num;
								if(num>0 and num <=100)
								{							
									vector<int> resindex;
									FindIndexThree(mileOrig, pivot[0], pivot[1], signs[0], signs[1], resindex);
									int pnum;
									if((int)resindex.size()>=num)
									{
										pnum = num;
									}
									else 
									{
										pnum = resindex.size();
									}									
									for(int i=0;i<pnum;i++)
									{
										memset(sendbuf,'\0',MAXBUFLEN);
										sprintf(sendbuf,"[%d] %s %d %s %s %d\n", i+1, 
										VINOrig[ resindex[i] ].c_str(),
										mileOrig[ resindex[i] ] ,
										makeOrig[ resindex[i] ].c_str(),
										colorOrig[ resindex[i] ].c_str(),
										yearOrig[ resindex[i] ]);
										send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);
										
										cout <<"[" << i+1 <<"] "
											<< VINOrig[ resindex[i] ] << " "
											<< mileOrig[ resindex[i] ] << " "
											<< makeOrig[ resindex[i] ] << " "
											<< colorOrig[ resindex[i] ] << " "
											<< yearOrig[ resindex[i] ] << endl;															
									}
								}
								else
								{
									memset(sendbuf,'\0',MAXBUFLEN);
									sprintf(sendbuf,"Number out of range. The supported print number is between 0 and 100.\n");
									send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);										
								}
							}							
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"<%s: %d>",uit->name.c_str(),++countuser);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);							
						}
						else if(order[0].compare("sorting")==0)
						{
							read(VINSort, mileSort, makeSort, colorSort, yearSort);
							double time  =MergeSort(mileSort);
							memset(sendbuf,'\0',MAXBUFLEN);
							sprintf(sendbuf,"Sorting execution time %f second.\n", time);
							send(sockfd,(void*)(sendbuf),sizeof(sendbuf),0);							
						}						
						else if(order.empty())
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
