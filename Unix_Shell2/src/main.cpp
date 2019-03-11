//============================================================================
// Name        : UnixShell.cpp
// Author      : WilliamMajor
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include<sys/wait.h>
#include <ctype.h>


#include <experimental/filesystem>



using namespace std;

void 	ls(string input);
void 	cd(string input);
string 	charptostr(char* input);

int main(int argc, char *argv[])
{
	string line = "";

	while (getline(cin,line)){


		if (line.compare(0,4,"exit") == 0)
		{
			exit(0);
		}

		else if (line.compare(0,2,"ls") == 0)
		{
			// char * direct = new char[line.size() + 1]; // do we use this...
			// copy(line.begin(), line.end(), direct);
			// direct[line.size()] = '\0';
			// system(direct);
			ls(line);
		}

		else if(line.compare(0,2,"cd") == 0)
		{
			cd(line);
		}

		else if(line.compare(0,0,"") == 0)
		{
			
		}
		else
		{
			cout<<"error command not found";
		}
	}

	return 0;
}

void ls(string input)
{
	int rc;

	rc = fork();
	
	if (rc < 0)
	{
		cout << "fork failed." << endl;
		exit (1);
	}
	else if(rc == 0)//child
	{
		int pid = (int) getpid();
		char * direct = new char[input.size() + 1]; // do we use this...
		copy(input.begin(), input.end(), direct);
		direct[input.size()] = '\0';
		system(direct);
		kill(pid,SIGTERM);
	}
	else
	{
		wait(NULL);
	}
	

}

void cd(string input)
{
	int spaces = 0;
	char* cwd = getcwd(cwd,100);
	string directory, ncwd;

	for (int idx = 0; idx < input.length(); idx++)
	{
		int check = input[idx];
		if(isspace(check))
		{
			spaces++;
		}
	}
	if(input.length() - 2 == spaces )
	{
		chdir(getenv("HOME"));
	}
	
	else 
	{
		
		
		directory = input.substr(3,input.length()-2);
		ncwd = charptostr(cwd);
		directory = ncwd + "/" + directory;
		char const * c = directory.data();

		
		chdir(c);
		
		
	}
	
}

string charptostr(char* input)
{
	stringstream ss;
	string output;
	ss << input;
	ss >> output;

	return output;
}

