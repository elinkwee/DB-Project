#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<cstdlib>

using namespace std;

vector<int>& 
FindIndexTwo(vector<int> &mile, int med, string sign, vector<int> &temp)
{
	// temp 是仅初始化但大小不定的vector 初始为空
//after executing, temp with SELECT index from mile inside!
	long x=0;
	if( sign == "=" )
	{
		for(x=0; x<(int)mile.size(); ++x)
			if( mile[x] == med )
			{
				temp.push_back(x);
			}
		return temp;
	}
	else if( sign == "!=" )
	{
		for(x=0; x<(int)mile.size(); ++x)
			if( mile[x] != med )
			{
				temp.push_back(x);
			}
		return temp;
	}
	else if( sign == "<" )
	{
		for(x=0; x<(int)mile.size(); ++x)
			if( mile[x] < med )
			{
				temp.push_back(x);
			}
		return temp;
	}
	else if( sign == "<=" )
	{
		for(x=0; x<(int)mile.size(); ++x)
			if( mile[x] <= med )
			{
				temp.push_back(x);
			}
		return temp;
	}
	else if( sign == ">" )
	{
		for(x=0; x<(int)mile.size(); ++x)
			if( mile[x] > med )
			{
				temp.push_back(x);
			}
		return temp;
	}
	else	// sign == ">="
	{
		for(x=0; x<(int)mile.size(); ++x)
			if( mile[x] >= med )
			{
				temp.push_back(x);
			}
		return temp;
	}
}

vector<int>& 
FindIndexThree(vector<int> &mile, 
			int low, int high, string sign1, string sign2, 
				vector<int> &temp)
{
	vector<int> bucket( (int)mile.size(), 0);
	long x1 = 0, 
		x2 = 0;
	if( sign1 == "=" )
	{
//		cout << "a ";
		for(x1=0; x1 < (int)mile.size(); ++x1)
			if( mile[x1] == low )
			{
				bucket[x1]++;
			}
	}
	else if( sign1 == "!=" )
	{
//		cout << "b ";
		for(x1=0; x1 < (int)mile.size(); ++x1)
			if( mile[x1] != low )
			{
				bucket[x1]++;
			}
	}
	else if( sign1 == "<" )
	{
//		cout << "c ";
		for(x1=0; x1 < (int)mile.size(); ++x1)
			if( mile[x1] < low )
			{
				bucket[x1]++;
			}
	}
	else if( sign1 == "<=" )
	{
//		cout << "d ";
		for(x1=0; x1 < (int)mile.size(); ++x1)
			if( mile[x1] <= low )
			{
				bucket[x1]++;
			}
	}
	else if( sign1 == ">" )
	{
//		cout << "e ";
		for(x1=0; x1 < (int)mile.size(); ++x1)
			if( mile[x1] > low )
			{
				bucket[x1]++;
			}
	}
	else // sign1 == ">="
	{
//		cout << "f ";
		for(x1=0; x1 < (int)mile.size(); ++x1)
			if( mile[x1] >= low )
			{
				bucket[x1]++;
			}
	}
	
	if(sign2 == "=" )
	{
//		cout << "A ";
		for(x2=0; x2 < (int)mile.size(); ++x2)
			if( mile[x2] == high )
			{
				bucket[x2]++;
			}
	}
	else if(sign2 == "!=" )
	{
//		cout << "B ";
		for(x2=0; x2 < (int)mile.size(); ++x2)
			if( mile[x2] != high )
			{
				bucket[x2]++;
			}
	}
	else if(sign2 == "<" )
	{
//		cout << "C ";
		for(x2=0; x2 < (int)mile.size(); ++x2)
			if( mile[x2] < high )
			{
				bucket[x2]++;
			}
	}
	else if(sign2 == "<=" )
	{	
//		cout << "D ";
		for(x2=0; x2 < (int)mile.size(); ++x2)
			if( mile[x2] <= high )
			{
				bucket[x2]++;
			}
	}
	else if(sign2 == ">" )
	{
//		cout << "E ";
		for(x2=0; x2 < (int)mile.size(); ++x2)
			if( mile[x2] > high )
			{
				bucket[x2]++;
			}
	}
	else // sign2 == ">=" )
	{
//		cout << "F ";
		for(x2=0; x2 < (int)mile.size(); ++x2)
			if( mile[x2] >= high )
			{
				bucket[x2]++;
			}
	}
	
//	for(long k=0; k < bucket.size(); ++k)
//		cout << bucket[k] << " ";
	
	for(long k=0; k < (int)mile.size(); ++k)
		if(bucket[k] == 2)
			temp.push_back( k );
		
	return temp;
}




