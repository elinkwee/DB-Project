#include<iostream>
#include<vector>
#include<string>
#include<cstdlib>
#include<ctime>
#include<sys/time.h>
#include<list>
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

const long N_ = 1000000;
extern const long N;
using namespace std;

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


struct crack2para 
{
	//vector<int> * mile;
	long posL;
	long posH;
	int med;
	string sign;
};

//vector<int>& mile, long posL, long posH, int low, int high
struct crack3para
{
	//vector<int> * mile;
	long posL;
	long posH;
	int low;
	int high;
	string sign1;
	string sign2;	
};

///////////////////time_substract/////////////////

int time_substract(struct timeval *result, struct timeval *begin,struct timeval *end)
{											// To calculate time diff between two timeval struct.
    if(begin->tv_sec > end->tv_sec)
		return -1;
    if((begin->tv_sec == end->tv_sec) && (begin->tv_usec > end->tv_usec))
		return -2;

    result->tv_sec = (end->tv_sec - begin->tv_sec);
    result->tv_usec = (end->tv_usec - begin->tv_usec);

    if(result->tv_usec < 0)
    {
        result->tv_sec--;
        result->tv_usec += 1000000;
    }
    return 0;
}

///////////////////////////////////////////////////

void * 
CrackInTwo(void *arg)
//CrackInTwo(vector<int>& mile, long posL, long posH, int med) //med is mileage. < and >=
{
	// receive one pivot "med"
	// need to guarantee 0 <= posL <= posH <= N-1. Wrong num if posH > N
	//posL posH cannot be equal. At least by difference of 1
	// med 取任意值均可 没有限制。最多不排序。不需要修改Two了。
	
	crack2para cp2 = *(crack2para *)arg;
	//vector<int>& mile = *(cp2.mile);
	long posL = cp2.posL;
	long posH = cp2.posH;
	int med = cp2.med;
	string sign = cp2.sign;
	
	if(mile.empty()){
		cout << "Empty mileage array!\n";
		exit(1);
	}
	
	if(posL < 0 || posH >= (int)mile.size()){
		cout << "pos cross border!\n";
		exit(1);
	}
	///begin counting time
	struct timeval start, finish, diff;
	gettimeofday(&start, 0);
	long x1 = posL;
	long x2 = posH;
	while(x1 < x2)
	{
		if(mile[x1] < med)
			x1 = x1 + 1;
		else
		{
			while(mile[x2] >= med && x2 > x1)
				x2 = x2 - 1;
			long temp  = mile[x1];
			mile[x1] = mile[x2];
			mile[x2] = temp;
			x1 = x1 + 1;
			x2 = x2 - 1;
		}
	}
	
	gettimeofday(&finish, 0);
	time_substract(&diff, &start, &finish);
	double * duration = new double( (double)diff.tv_sec + ((double)diff.tv_usec/1000000.0));
	////finish counting time
	return (void*)duration;

}

void *
CrackInThree(void *arg)
// CrackInThree(vector<int>& mile, 
			// long posL, 
			// long posH, int low, int high)
// >=, >, <
{
	
	crack3para cp3 = *(crack3para *)arg;
	//vector<int>& mile = *(cp3.mile);
	long posL = cp3.posL;
	long posH = cp3.posH;
	int low = cp3.low;
	int high = cp3.high;
	string sign1 = cp3.sign1;
	string sign2 = cp3.sign2;	
	
	if(low>high)
	{
		int temp = high;
		high = low;
		low  = temp;
		string ts = sign2;
		sign2 = sign1;
		sign1 = ts;		
	}
	
		// receive two pivot "low" "high"
		///begin counting time
	struct timeval start, finish, diff;
	gettimeofday(&start, 0);
	
	long x1 = posL;
	long x2 = posH;
//	cout << "before while1";
	while(mile[x2] >= high && 	//
		  x2 > x1)				//
	{
		x2--;
	//	cout << "a ";
	}
	long x3 = x2;
	//cout << "before while2";
	while ((mile[x3] > low) && (x3 > x1))
	{
		if(mile[x3] >= high)
		{
			int temp = mile[x2];
			mile[x2] = mile[x3];
			mile[x3] = temp;
			x2--;
		}
	//	cout << "b ";
		x3--;
	}
	// 
	if( x1 == 0 && x3 == 0) {
		if(mile[x1] >= high){
			int temp = mile[x1];
			mile[x1] = mile[x2];
			mile[x2] = temp;
		}
		return NULL;
	}
	
	//cout << "Before x1<=x3\n";
	while(x1 <= x3)
	{
	//	cout << "c ";
		if(mile[x1] <= low)
			x1++;
		else
		{
			int temp = mile[x1];
			mile[x1] = mile[x3];
			mile[x3] = temp;
		//	cout << "x1 = " << x1 << "  c[x1] = " << mile[x1] << endl;
		//	cout << " x2 = " << x2 << "  c[x2] = " << mile[x2] << endl;
		//	cout << " x3 = " << x3 << "  c[x3] = " << mile[x3] << endl;
		//	cout << "d ";
			while(mile[x3] >= low && x3 > x1)
			{
			//	cout << "e ";
				if(mile[x3] >= high)
				{
					int t1 = mile[x2];
					mile[x2] = mile[x3];
					mile[x3] = t1;
					x2--;
					//cout << "f ";
				}
				x3--;
				//cout << "ggggggggggggggggggggggggggggg\n ";
			}
		}
		//cout <<"3333\n";
	}
	//cout << posL << " "<<posH<<" "<<"222222\n";
	gettimeofday(&finish, 0);
	time_substract(&diff, &start, &finish);
	double * duration = new double((double)diff.tv_sec + ((double)diff.tv_usec/1000000.0));
	////finish counting time
	return (void*)duration;
}


double
MergeInTwo(int med)
//MergeInTwo(vector<int>& mile, int med) //med is mileage. < and >=
{
	//用于多线程第二阶段，其实和CrackInTwo一样只不过改了接口
	long posL = 0;
	long posH = mile.size() - 1;
	if(mile.empty()){
		cout << "Empty mileage array!\n";
		exit(1);
	}
	
	if(posL < 0 || posH >= (int)mile.size()){
		cout << "pos cross border!\n";
		exit(1);
	}
	
	///begin counting time
	struct timeval start, finish, diff;
	gettimeofday(&start, 0);
	
	long x1 = posL;
	long x2 = posH;
	while(x1 < x2)
	{
		if(mile[x1] < med)
			x1 = x1 + 1;
		else
		{
			while(mile[x2] >= med && x2 > x1)
				x2 = x2 - 1;
			long temp  = mile[x1];
			mile[x1] = mile[x2];
			mile[x2] = temp;
			x1 = x1 + 1;
			x2 = x2 - 1;
		}
	}
	gettimeofday(&finish, 0);
	time_substract(&diff, &start, &finish);
	double duration = (double)diff.tv_sec + ((double)diff.tv_usec/1000000.0);
	////finish counting time
	return duration;
}

double
MergeInThree(int low, int high)
// >=, >, <

{
	if(low>high)
	{
		int temp = high;
		high = low;
		low  = temp;
	}	
		///begin counting time
	struct timeval start, finish, diff;
	gettimeofday(&start, 0);
	
	long posL = 0;
	long posH = mile.size() - 1;
	long x1 = posL;
	long x2 = posH;
//	cout << "before while1";
	while(mile[x2] >= high && x2 > x1)
	{
		x2--;
	//	cout << "a ";
	}
	long x3 = x2;
	//cout << "before while2";
	while (mile[x3] > low && x3 > x1)
	{
		if(mile[x3] >= high)
		{
			int temp = mile[x2];
			mile[x2] = mile[x3];
			mile[x3] = temp;
			x2--;
		}
	//	cout << "b ";
		x3--;
	}
	// 
	if( x1 == 0 && x3 == 0) {
		if(mile[x1] >= high){
			int temp = mile[x1];
			mile[x1] = mile[x2];
			mile[x2] = temp;
		}
		return -1.0;
	}
	
//	cout << "Before x1<=x3\n";
	while(x1 <= x3)
	{
	//	cout << "c ";
		if(mile[x1] <= low)
			x1++;
		else
		{
			int temp = mile[x1];
			mile[x1] = mile[x3];
			mile[x3] = temp;
		//	cout << "d ";
			while(mile[x3] >= low && x3 > x1)
			{
			//	cout << "e ";
				if(mile[x3] >= high)
				{
					int t1 = mile[x2];
					mile[x2] = mile[x3];
					mile[x3] = t1;
					x2--;
				//	cout << "f ";
				}
				x3--;
				//cout << "g ";
			}
		}
	}
	
	gettimeofday(&finish, 0);
	time_substract(&diff, &start, &finish);
	double duration = (double)diff.tv_sec + ((double)diff.tv_usec/1000000.0);
	////finish counting time
	return duration;
}



///////////Insertion Sort//////////////////

double Insertion(vector< pair<long,string> >& v)
{
	struct timeval start, finish, diff;
	gettimeofday(&start, 0);
	for(long i=0; i<(int)v.size(); ++i)
	{
		if( v[i-1].first > v[i].first )
		{
			pair<long, string> temp = v[i];
			long j = i;
			while( j>0 && v[j-1].first > temp.first )
			{
				v[j] = v[j-1];
				j--;
			}
			v[j] = temp;
		}
	}
	gettimeofday(&finish, 0);
	time_substract(&diff, &start, &finish);
	double duration = (double)diff.tv_sec + ((double)diff.tv_usec/1000000.0);
	
	return duration;
	
}

///////////Merge Sort//////////////////

void mergehalves(vector< int>& v, 
				long first, long mid, long last, 
				vector< int >& temp)
{
	long i1 = first, i2 = mid+1,	// beginning pointers for two halves
		j1 = mid, j2 = last,		// end pointers
		k = 0;
		
	while( i1 <= j1 && i2 <= j2)	//smaller one from two halves each time.
	{								// move onto next on smaller half.
		if( v[i1] <= v[i2] )
		{
			temp[k] = v[i1];
			k++; i1++;
		}
		else{
			temp[k] = v[i2];
			k++; i2++;
		}
	}
	
	while( i1<=j1 )		//remaining ones on first half, if any!
	{
		temp[k] = v[i1];
		k++; i1++;
	}
	while( i2<=j2 )		//remaining ones on second half, if any!
	{
		temp[k] = v[i2];
		k++; i2++;
	}
	
	for(long m = 0; m<k; m++)	//copy sortend to original array
		v[first + m] = temp[m];
	
}

void merge(vector< int >& v, 
			long first, long last, 
			vector< int >& temp)
{
	if( first < last )
	{
		long mid = (first + last)/2;
		merge(v, first, mid, temp);			// first half
		merge(v, mid+1, last, temp);		// second half
		mergehalves(v, first, mid, last, temp);
	}
}

double MergeSort(vector<int>& v)	// v = mile
{
	vector<int > temp(N);	//initialize a vector with size N
	struct timeval start, finish, diff;
	gettimeofday(&start, 0);
	
	merge(v, 0, N-1, temp);
	
	gettimeofday(&finish, 0);
	time_substract(&diff, &start, &finish);
	double duration = (double)diff.tv_sec + ((double)diff.tv_usec/1000000.0);
	
	return duration;
}


///////////////Counting Sort//////////////////

double Counting(vector< pair<long,string> >& v, long K_)
{
	vector< list< pair<long, string> > > b(K_+1); // because [1,K_], K_+1 kinds num in total
												// 120014 line in 1000000.txt is 100000 ...
	struct timeval start, finish, diff;
	gettimeofday(&start, 0);
	
	for(long i=0; i<(int)v.size(); i++)
	{
		b[ v[i].first ].push_back( v[i]);
//		cout << i << "\t" << v[i].first << " | " << v[i].second << endl;
	}

	v.clear();
	list< pair<long, string> >::iterator itr;
	for(long i=0; i<K_+1; i++) // 0,1,2,3,4,5,6,7,8,9,10 in bucket
		if( !b[i].empty())
			for(itr = b[i].begin(); itr != b[i].end(); itr++)
				v.push_back(*itr);

	gettimeofday(&finish, 0);
	time_substract(&diff, &start, &finish);
	double duration = (double)diff.tv_sec + ((double)diff.tv_usec/1000000.0);
	
	return duration;
}


/*
4.16修复CrackInThree bug. 当low小于min(mile)时x1 和 x3都是0，然后
无限交换死循环。加上		
if( x1 == 0 && x3 == 0) {
		if(mile[x1] >= high){
			int temp = mile[x1];
			mile[x1] = mile[x2];
			mile[x2] = temp;
		}
		return;
	}
因为x1=x3=0 就是low比min还小的特殊case。

修复使用相同pivot加上vector里面如果有pivot的值时，无限循环的bug。
		if(mile[x1] < low)
			x1++;
改成<=

4.17 完成merge phase其实就是crack不知道这样的话是不是快点

*/












