

#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<cstdlib>


using namespace std;

void
read(vector<string> &VIN, 
	vector<int> &mile,
	vector<string> &make,
	vector<string> &color,
	vector<int> &year )
{
	ifstream input;
	string ifilename = "CarData";
	input.clear();
	input.open(ifilename.c_str());
	if(!input)
	{
		cout << "Sorry bad file! Cannot read!\n";
		exit(1);
	}
	else
		cout << "Data file loading...\n";
	
	if(input.peek() == EOF)
	{
		cout << "File is empty!\n";
		exit(1);
	}
	
	while(!input.eof())
	{
		string tVIN;
		int tmile;
		string tmake;
		string tcolor;
		int tyear;
		
		input >> tVIN
			>> tmile
			>> tmake
			>> tcolor
			>> tyear;
			
		VIN.push_back(tVIN);
		mile.push_back(tmile);
		make.push_back(tmake);
		color.push_back(tcolor);
		year.push_back(tyear);
		
		input.get();	//read last "endl"
		if(input.peek() == '\n') break;
	}
	
	cout << "Data loaded into Database.\n";
}

