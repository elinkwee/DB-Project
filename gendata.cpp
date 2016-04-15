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

/*��ȷ������ȫ�ֱ����Ϳ���ʵ�ֱ������ļ��еĹ����������徲̬ȫ�ֱ����������ºô���
       ��̬ȫ�ֱ������ܱ������ļ����ã�
       �����ļ��п��Զ�����ͬ���ֵı��������ᷢ����ͻ��
*/

/*
http://zhidao.baidu.com/question/303841146109528764.html?qbl=relate_question_2&word=c%2B%2B%20%CB%E6%BB%FA%CA%FD%CF%E0%CD%AC

׷�ʣ�����Ϊʲôsrand()�����Լӵ�diceroll�ĺ����У�

׷����������ӳ�ʼ��ֻ��һ�Σ�������ʼ���ͻ�����ظ������֡�

*/

#include<iostream>
#include<vector>
#include<string>
#include<ctime>
#include<cstdlib>
#include<fstream>

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
gen_data(int N)	// N <= 100,000,000. Records number
{
	ofstream output;
	output.clear();
	string ofilename = "CarData";
	output.open(ofilename.data());
	if(!output)
		cout << "Error creating output datafile!\n";
	
	srand((unsigned)time(NULL));
	for(int k=0; k<N; ++k){
		output << gen_VIN() << " "
			<< gen_mile() << " "
			<< gen_Manu() << " "
			<< gen_color() << " "
			<< gen_year() << endl;
	}
	
	
	output.close();
	
}

int main(){
	gen_data(50);
		
	return 0;
}