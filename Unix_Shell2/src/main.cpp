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
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>
#include <dirent.h>
#include <experimental/filesystem>

using namespace std;

void 	ls(string input);
void 	cd(string input);
void	cat(string input);
void	rm(string input);
void	path(string input);
void	echo(string input);

int main(int argc, char *argv[])
{
	string line = "";
	

	while(1)
	{
		cout << "SuperShell > ";
		getline(cin,line);
	
		if (line.compare(0,4,"exit") == 0)
			exit(0);

		else if (line.compare(0,2,"ls") == 0)
			ls(line);

		else if(line.compare(0,2,"cd") == 0)
			cd(line);

		else if(line.compare(0,3,"cat") == 0)
			cat(line);

		else if(line.compare(0, 2, "rm") == 0)
			rm(line);
		
		else if(line.compare(0, 4, "path") == 0)
			path(line);

		else if(line.compare(0, 4, "echo") == 0)
			echo(line);

		else if(line.length() == 0)
		{
		}
		else
		{
			cout<<"error command not found" << endl;
		}

	}

	return 0;
}

void echo(string input)
{
	cout << input.erase(0,5)<<endl;
}

void path(string input)
{

}

void ls(string input)
{
	int rc, found, found2, saveout, fd, fd2;
	string file_location, testfile, testfile2;
	
	rc = fork();
	
	if (rc < 0)
	{
		cout << "fork failed." << endl;
		exit (1);
	}
	else if(rc == 0)//child
	{
		int pid = (int) getpid();
		if(access("/bin/ls",X_OK) == -1)
		{
			if(access("/user/bin/ls",X_OK) == -1)
			{
				cout << "error could not find ls"<<endl;
				exit(-1);
			}
		}

		else
		{
			string output = ">";
			found = input.find(output);
			found2 = input.find(output,found+1);
			if(found > 0)
			{

				if(found2 > 0) // we found two > so we need to make one file and write to the second
				{
						testfile = input.substr((found+2),(found2-(found+2)));
						char * cstr = new char[testfile.length()];
						strcpy(cstr,testfile.c_str());

						testfile2 = input.substr(found2+2,input.length());
						char * cstr2 = new char[testfile2.length()];
						strcpy(cstr2,testfile2.c_str());
						
						fd = open(cstr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

						fd2 = open(cstr2, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
						
						saveout = dup(fileno(stdout));

						dup2(fd2,fileno(stdout));

						delete[] cstr,cstr2;
						
						file_location = input.substr(3,found-4);
						cd(file_location);

						string ls = "ls";
						char* argm[2];
						argm[0] = (char*)ls.c_str();
						argm[1] = NULL;
						execvp(argm[0], argm);
						cout << "error";
				}
				else
				{
					if (found > 3)
					{	
						
						testfile = input.substr(found+2,input.length());
						char * cstr = new char[testfile.length()];
						strcpy(cstr,testfile.c_str());
						
						fd = open(cstr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
						
						saveout = dup(fileno(stdout));

						dup2(fd,fileno(stdout));

						delete[] cstr;
						
						file_location = input.substr(3,found-4);
						cd(file_location);

						string ls = "ls";
						char* argm[2];
						argm[0] = (char*)ls.c_str();
						argm[1] = NULL;
						execvp(argm[0], argm);
						cout << "error";
					}
					else
					{
						testfile = input.substr(found+2,input.length());
						char * cstr = new char[testfile.length()];
						strcpy(cstr,testfile.c_str());
						
						fd = open(cstr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
						
						saveout = dup(fileno(stdout));

						dup2(fd,fileno(stdout));

						delete[] cstr;

						string ls = "ls";
						char* argm[2];
						argm[0] = (char*)ls.c_str();
						argm[1] = NULL;
						execvp(argm[0], argm);
						cout << "error";
					}
					
				}
				
			}
			else
			{	
				if (input.length() > 2)
				{
					file_location = input.substr(3,input.length());
					cd(file_location);

					string ls = "ls";
					char* argm[2];
					argm[0] = (char*)ls.c_str();
					argm[1] = NULL;
					execvp(argm[0], argm);
					cout << "error";
				}
				else
				{
					string ls = "ls";
					char* argm[2];
					argm[0] = (char*)ls.c_str();
					argm[1] = NULL;
					execvp(argm[0], argm);
					cout << "error";
				}
				
			}
			

		}
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
	string directory, ncwd, olddirectory;
	stringstream ss;

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
		
		ss << cwd;
		ss >> ncwd;
		olddirectory = directory;
		directory = ncwd + "/" + directory;
		char const * c = directory.data();
		if(chdir(c) != 0)
		{
			cout << "bash: cd: " << olddirectory << " No such file or directory"<<endl;
		}	
	}
	
}

void cat(string input)
{
	int rc, fr, t, found;
	int spaces[100];
	string newinput;
	char const * c;
	char char_array[input.length() + 1];

	strcpy(char_array, input.c_str());
	int count = 0;
	for (int i = 0; i < input.length(); i++)
	{
		
		if(char_array[i] == ' ')
		{
			spaces[count] = i;
			count++;
		}
	}
	
	
	if(input.length() == 3)
	{
		while(getline(cin,newinput))
		{
			cout << newinput << endl;
		}
	}
	//split all commands int individual parts of a string array and we will operate on them seperately
	else if(input.length() > 3)
	{
		string files[sizeof(spaces) + 1];
		string tmp;
		bool fwdslash;
		int location = 0;
		int length = 0;

		while(spaces[location + 1] != 0)//handles all but the last one
		{
			fwdslash = true;
			tmp = "";
			for (int a = spaces[location] + 1; a < spaces[location + 1]; a++)
			{
				if((char_array[a] == '/') && fwdslash && ( a == spaces[location] +1))
				{
					fwdslash = false;
				}
				else
				{
					tmp += char_array[a];
				}
			}
			files[location] = tmp;
			location++;
		}

		fwdslash = true;
		tmp = "";
		for(int a = spaces[location] + 1; a < sizeof(char_array); a++) // handle the last one
		{
			if((char_array[a] == '/') && fwdslash && ( a == spaces[location] +1))
			{
				fwdslash = false;
			}
			else
			{
				tmp += char_array[a];
			}
		}
		files[location] = tmp;


		while(!files[length].empty())
		{
			c = files[length].data();
			fr = open(c, O_RDONLY);

			if(fr < 0)
			{
				cout << "error opening file" <<endl;
				exit(1);
			}

			while(read(fr, &t, 1))
			{
				write(STDOUT_FILENO, &t, 1);
			}

			close(fr);
			length++;
		}
	}

	

	// c = input.data();
	// cout << c << endl;
	// fr = open(c, O_RDONLY);

	// if(fr < 0)
	// {
	// 	cout << "failed to open file" <<endl;
	// 	exit(-1);
	// }

	// while (read(fr, &t, 1))
	// {
	// 	write(STDOUT_FILENO, &t, 1);
	// }

	// close(fr);
	
}

void rm(string input)
{
	string arguments, command;
	int found, found2, status;
	DIR *blah = NULL;
	struct dirent *DirEntry = NULL;
	unsigned char isFile =0x8;
	found = input.find("-");
	if(found > 0)
	{
		input.erase(0, found+1);
		found = input.find(" ");
		arguments = input;
		command = input;
		arguments.erase(found, arguments.length());
		command.erase(0,found + 1);

		if (arguments == "r")
		{
			char const * removecmd = command.data();
			if(remove(removecmd) != 0)
			{
				cout << "rm: cannot remove '" << command <<"' : no such file or directory" << endl;
			}
		}
		else if(arguments == "f")
		{
			char const * filecmd = command.data();
			blah = opendir(".");
			if(blah == NULL)
			{
				cout << "does not exits" << endl;
				exit(8);
			}
			DirEntry = readdir(blah);
			switch (DirEntry -> d_type)
			{
				case DT_REG:
					remove(filecmd);
					break;
				case DT_DIR:
					cout << "this is a diretory" << endl;
					break;
				default:
					cout <<"Well this is awkward" << endl;
			}
		}
		else if(arguments == "rf")
		{
			char const * removecmd = command.data();
			remove(removecmd);
		}
		
	}
	else
	{
		if(input.length() == 2)
		{
			cout <<"rm: missing operand" << endl;
		}
	}
}


