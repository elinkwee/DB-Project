/*
	Our VIN:	L-letter, N-number
	1bit - LN
	2 - L
	3 - N
	4~5 - L
	6~9 - N
	10 - L
	11~17 - N 
*/

/*的确，定义全局变量就可以实现变量在文件中的共享，但定义静态全局变量还有以下好处：
       静态全局变量不能被其它文件所用；
       其它文件中可以定义相同名字的变量，不会发生冲突；
*/

/*
http://zhidao.baidu.com/question/303841146109528764.html?qbl=relate_question_2&word=c%2B%2B%20%CB%E6%BB%FA%CA%FD%CF%E0%CD%AC

追问：请问为什么srand()不可以加到diceroll的函数中？

追答：随机数种子初始化只需一次，反复初始化就会产生重复的数字。

*/
const long N = 500000;
#define MED 30000
#define LOW 50000
#define HIGH 100000

#include<iostream>
#include<vector>
#include<string>
#include<ctime>
#include<cstdlib>
#include<fstream>
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
#include "read_create.h"
#include "partition.h"

using namespace std;

static char NnL[] = 
{
	'0','1','2',
	'3','4','5',
	'6','7','8',
	'9','A','B',
	'C','D','E',
	'F','G','H',
	'I','J','K',
	'L','M','N',
	'O','P','Q',
	'R','S','T',
	'U','V','W',
	'X','Y','Z'
};

static string Manu[] = 
{
	"Toyota", "Honda", "Mazda", 
	"Nissan", "Ford", "Dodge", 
	"Chevrolet", "Chryler", "Jeep",
	"Volkswagen", "BMW", "Benz", 
	"Other"
};
	
static string Color[] = 
{
	"Red", "White", "Grey", 
	"Black", "Silver","Tan", 
	"Blue", "Green", "Purple", 
	"OtherColor"
};

// TABLE 
vector<string> VIN;
vector<int> mile;
vector<string> make;
vector<string> color;
vector<int> year;

vector<string> VINOrig;
vector<int> mileOrig;
vector<string> makeOrig;
vector<string> colorOrig;
vector<int> yearOrig;

vector<string> VINSort;
vector<int> mileSort;
vector<string> makeSort;
vector<string> colorSort;
vector<int> yearSort;

	
string 
gen_VIN()
{
	string VIN(17,'\0');
//	srand((unsigned)time(NULL));
	VIN[0] = NnL[rand() % 36]; 			//gen index 0~35
	VIN[1] = NnL[(rand() % 26) + 10];	// gen index 10~35
	VIN[2] = NnL[rand() % 10];			//gen index 0~9
	VIN[3] = NnL[(rand() % 26) + 10];
	VIN[4] = NnL[(rand() % 26) + 10];
	VIN[5] = NnL[rand() % 10];
	VIN[6] = NnL[rand() % 10];
	VIN[7] = NnL[rand() % 10];
	VIN[8] = NnL[rand() % 10];
	VIN[9] = NnL[(rand() % 26) + 10];
	VIN[10] = NnL[rand() % 10];
	VIN[11] = NnL[rand() % 10];
	VIN[12] = NnL[rand() % 10];
	VIN[13] = NnL[rand() % 10];
	VIN[14] = NnL[rand() % 10];
	VIN[15] = NnL[rand() % 10];
	VIN[16] = NnL[rand() % 10];
	
	return VIN;
}

int
gen_mile()		//gen mileage
{
//	srand((unsigned)time(NULL));
	
	return rand() % 150001; // [0, 150000]
}

string
gen_Manu()
{
//	srand((unsigned)time(NULL));
	
	return Manu[rand() % 13];
}

string
gen_color()
{
//	srand((unsigned)time(NULL));
	
	return Color[rand() % 10];
}

int
gen_year()
{
//	srand((unsigned)time(NULL));
	
	return (rand() % 27 + 1990); // [1990, 2016]
}
	
void 
gen_data(long Num)	// N <= 100,000,000. Records number
{
	ofstream output;
	output.clear();
	string ofilename = "CarData";
	output.open(ofilename.data());
	if(!output)
	{
		cout << "Error creating output datafile!\n";
	}
	
	srand((unsigned)time(NULL));
	for(long k=0; k<Num; ++k)
	{
		output << gen_VIN() << " "
			<< gen_mile() << " "
			<< gen_Manu() << " "
			<< gen_color() << " "
			<< gen_year() << endl;
	}
	
	
	output.close();
	
}

