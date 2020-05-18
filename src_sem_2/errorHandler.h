#ifndef ErrorHanhler_H
#define ErrorHanhler_H
using namespace std;
#include<iostream>
#include<stdlib.h>
class err
{
	public:
		bool errCheck(int,char* argv[]);
};

bool err::errCheck(int argc, char* argv[])
{
	//Checks number of arguments  
	if(argc<4)
	{
		cout<<"Invalid no of arguments\n";
		return false;

	}
	else 
	{
		if(!atoi(argv[1]))
		{
			cout<<"Number of Nodes- Argument is not an integer";
			return false;
		}
		if(!atoi(argv[2]))
		{
			cout<<"Number of threads- Argument is not an integer";
			return false;
		}
		if(!atoi(argv[3]))
		{
			cout<<"case Type- Argument is not an integer";
			return false;
		}
		else {return true;}
	}
}

#endif
