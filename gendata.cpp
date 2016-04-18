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
const long N = 250000;
#define MED 30000
#define LOW 50000
#define HIGH 100000

#include<iostream>
#include<vector>
#include<string>
#include<ctime>
#include<cstdlib>
#include<fstream>

#include "read_create.h"
#include "partition.h"

using namespace std;

static char NnL[] = {'0','1','2','3','4','5','6','7','8','9',
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
	'O','P','Q','R','S','T','U','V','W','X','Y','Z'};

static string Manu[] = {"Toyota", "Honda", "Mazda", "Nissan", 
	"Ford", "Dodge", "Chevrolet", "Chryler", "Jeep",
	"Volkswagen", "BMW", "Benz", "Other"};
	
static string color[] = {"Red", "White", "Grey", "Black", "Silver",
	"Tan", "Blue", "Green", "Purple", "OtherColor"};

	
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
	
	return color[rand() % 10];
}

int
gen_year()
{
//	srand((unsigned)time(NULL));
	
	return (rand() % 27 + 1990); // [1990, 2016]
};
	
void 
gen_data(long Num)	// N <= 100,000,000. Records number
{
	ofstream output;
	output.clear();
	string ofilename = "CarData";
	output.open(ofilename.data());
	if(!output)
		cout << "Error creating output datafile!\n";
	
	srand((unsigned)time(NULL));
	for(long k=0; k<Num; ++k){
		output << gen_VIN() << " "
			<< gen_mile() << " "
			<< gen_Manu() << " "
			<< gen_color() << " "
			<< gen_year() << endl;
	}
	
	
	output.close();
	
}

int main(){

	gen_data(N);
	
	vector<string> VIN;
	vector<int> mile;
	vector<string> make;
	vector<string> color;
	vector<int> year;
	
	double duration=0.0;
	
	read(VIN, mile, make, color, year);

//	for(int k=0; k<N; ++k)
//		cout << mile[k] << endl;
/*
	CrackInTwo(mile, 0, N-1, MED);
	cout << "Cracking Two completed\n\n";
*/
	cout << "\n\n";
	duration = MergeSort(mile);
//	for(int k=0; k<N; ++k)
//		cout << mile[k] << endl;
	cout << "MergeSort takes " << duration << " second.\n";
	
/*
	cout << "N = " << N << endl;
	duration = MergeInThree(mile, LOW, HIGH);
	cout << "MergeInThree takes " << duration << " second.\n";
	
	duration = MergeInTwo(mile, MED);
	cout << "MergeInTwo takes " << duration << " second.\n";

		for(int k=0; k<50; ++k)
		cout << mile[k] << endl;
	
	CrackInThree(mile, 0, N-1, LOW, HIGH);
//	cout << "Cracking Three1 completed\n\n";
	
	
	//CrackInThree(mile, 0, N-1, 20000, 69999);
	CrackInThree(mile, 0, N-1, LOW, 140000);
//	cout << "Cracking Three2 completed\n\n";
	
	CrackInThree(mile, 0, N-1, LOW, 140000);
	
	cout << "\n\n";
	
	for(int k=0; k<50; ++k)
	cout << mile[k] << endl;
	
	CrackInThree(mile, 0, N-1, -0, 120000);
	cout << "\n\n";
	for(int k=0; k<mile.size(); ++k)
	cout << mile[k] << endl;
	
	CrackInThree(mile, 0, N-1, -40000, 100000);
	cout << "\n\n";
	for(int k=0; k<mile.size(); ++k)
	{
	cout << mile[k] << endl;
	}

	CrackInThree(mile, 0, N-1, -70000, 130000);
	cout << "\n\n";
	for(int k=0; k<mile.size(); ++k)
	cout << mile[k] << endl;
*/	
	
	return 0;
}