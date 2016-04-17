#include<iostream>
#include<vector>
#include<string>
#include<cstdlib>


using namespace std;

void
CrackInTwo(vector<int>& mile, long posL, long posH, int med) //med is mileage. < and >=
{
	// need to guarantee 0 <= posL <= posH <= N-1. Wrong num if posH > N
	//posL posH cannot be equal. At least by difference of 1
	// med 取任意值均可 没有限制。最多不排序。不需要修改Two了。
	
	if(mile.empty()){
		cout << "Empty mileage array!\n";
		exit(1);
	}
	
	if(posL < 0 || posH >= mile.size()){
		cout << "pos cross border!\n";
		exit(1);
	}
	
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
}

void
CrackInThree(vector<int>& mile, long posL, long posH, int low, int high)
// >=, >, <

{
	long x1 = posL;
	long x2 = posH;
//	cout << "before while1";
	while(mile[x2] >= high && x2 > x1)
	{
		x2--;
		cout << "a ";
	}
	long x3 = x2;
	cout << "before while2";
	while (mile[x3] > low && x3 > x1)
	{
		if(mile[x3] >= high)
		{
			int temp = mile[x2];
			mile[x2] = mile[x3];
			mile[x3] = temp;
			x2--;
		}
		cout << "b ";
		x3--;
	}
	// 
	if( x1 == 0 && x3 == 0) {
		if(mile[x1] >= high){
			int temp = mile[x1];
			mile[x1] = mile[x2];
			mile[x2] = temp;
		}
		return;
	}
	
//	cout << "Before x1<=x3\n";
	while(x1 <= x3)
	{
		cout << "c ";
		if(mile[x1] < low)
			x1++;
		else
		{
			int temp = mile[x1];
			mile[x1] = mile[x3];
			mile[x3] = temp;
			cout << "x1 = " << x1 << "  c[x1] = " << mile[x1] << endl;
			cout << " x2 = " << x2 << "  c[x2] = " << mile[x2] << endl;
			cout << " x3 = " << x3 << "  c[x3] = " << mile[x3] << endl;
			cout << "d ";
			while(mile[x3] >= low && x3 > x1)
			{
				cout << "e ";
				if(mile[x3] >= high)
				{
					int t1 = mile[x2];
					mile[x2] = mile[x3];
					mile[x3] = t1;
					x2--;
					cout << "f ";
				}
				x3--;
				cout << "g ";
			}
		}
	}
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

*/












