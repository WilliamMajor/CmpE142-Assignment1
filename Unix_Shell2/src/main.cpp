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

void 	ls(string input, string cwd);
void 	cd(string input);
void	cat(string input);
void	rm(string input);
void	path(string input);
void	echo(string input);
void 	sleep(string input);
void 	multcmd(string input[], bool padstat);
void	shellex(string input, bool padstat);

string GlobalPath[100];
char const * GP;

int main(int argc, char *argv[])
{
	string line = "";
	string initialpath(getenv("HOME"));
	GlobalPath[0] = initialpath;
	bool badpath = false;
	
	

	while(1)
	{
		
		int idx = 0;
		 
		while(!GlobalPath[idx].empty())
		{
			GP = GlobalPath[idx].data();
			if(access(GP,X_OK) == -1)
			{
				cout << "warning one or more paths do not exist" << endl;
				badpath = true;
				idx++;
				
			}
			badpath = false;
			break;//this isn't the correct way to do this but it works for now... -_- I think
		}
		cout << "SuperShell > ";
		line.clear();
		getline(cin,line);
		char input[100];
		int spaces[100]; 
		int ands[100];
		int count = 0;
		int count2 = 0;
		int offset = 0;
		int location = 0;
		int length = 0;
		int gah;
		bool spcs = false;
		bool mult = false;
		string tmp;


		if(line.find("&") != -1)
		{
			string commandset[100];
			strcpy(input, line.c_str());

			for (int i = 0; i < line.length(); i++)
			{
				if(((input[i] == ' ') && (input[i+1] == '&')) || ((input[i] == ' ') && (input[i-1] == '&')))
				{
					spaces[count] = i;
					count++;
					spcs = true;
				}
			}
			if(spcs)
			{
				while(spaces[count2] != 0)
				{
					line.erase(spaces[count2] - offset , 1);
					count2++;
					offset++;
				}
			}

			strcpy(input, line.c_str());
			count = 0;
			for (int i = 0; i < line.length(); i++)
			{
				if(input[i] == '&')
				{
					ands[count] = i;
					count++;
					mult = true;
				}
			}
			if(mult)
			{
				tmp = "";
				for (int a = 0; a < ands[location]; a++)
				{
					tmp += input[a];
				}
				commandset[location] = tmp;

				while(ands[location + 1] != 0)//handles all but the last one
				{
					tmp = "";
					for (int a = ands[location] + 1; a < ands[location + 1]; a++)
					{		
							tmp += input[a];
					}
					commandset[location + 1] = tmp;
					location++;
				}

				tmp = "";
				for (int a = ands[location] + 1; a <= line.length(); a++)
				{
					tmp += input[a];
				}
				commandset[location + 1] = tmp;
				mult = false;
			}
			multcmd(commandset, badpath);
		}

		else
		{
			if (line.compare(0,4,"exit") == 0)
				exit(0);

			else if (line.compare(0,2,"ls") == 0 && !badpath)
			{
				char* cwd2 = getcwd(cwd2,100);
				stringstream ss;
				string adder;
				ss << cwd2;
				ss >> adder;
				ls(line, adder);
			}

			else if(line.compare(0,2,"cd") == 0)
				cd(line);

			else if(line.compare(0,3,"cat") == 0 && !badpath)
				cat(line);

			else if(line.compare(0, 2, "rm") == 0 && !badpath)
				rm(line);
			
			else if(line.compare(0, 4, "path") == 0)
				path(line);

			else if(line.compare(0, 4, "echo") == 0 && !badpath)
				echo(line);

			else if (line.compare(0,5, "sleep") == 0 && ! badpath)
				sleep(line);

			else if (line.find(".sh") != -1)
				shellex(line, badpath);

			else if(line.length() == 0)
			{
			}
			else
			{
				cout<<"error command not found" << endl;
			}
		}

	}

	return 0;
}

void multcmd(string input[], bool pathstat)
{
	int i = 0;
	while(!input[i].empty())
	{
		if (input[i].compare(0,4,"exit") == 0)
			exit(0);

		else if (input[i].compare(0,2,"ls") == 0 && !pathstat)
		{
			char* cwd2 = getcwd(cwd2,100);
			stringstream ss;
			string adder;
			ss << cwd2;
			ss >> adder;
			ls(input[i], adder);
		}

		else if(input[i].compare(0,2,"cd") == 0)
			cd(input[i]);

		else if(input[i].compare(0,3,"cat") == 0 && !pathstat)
			cat(input[i]);

		else if(input[i].compare(0, 2, "rm") == 0 && !pathstat)
			rm(input[i]);
		
		else if(input[i].compare(0, 4, "path") == 0)
			path(input[i]);

		else if(input[i].compare(0, 4, "echo") == 0 && !pathstat)
			echo(input[i]);

		else if (input[i].compare(0,5, "sleep") == 0 && !pathstat)
			sleep(input[i]);

		else if (input[i].find(".sh") != -1)
		{
			shellex(input[i], pathstat);
		}


		else if(input[i].length() == 0)
		{
		}
		else
		{
			cout<<"error command not found" << endl;
		}
		i++;
	}
	
}

void shellex(string input1, bool pathstat)
{
	char input[100];
	int spaces[100]; 
	int ands[100];
	int count = 0;
	int count2 = 0;
	int offset = 0;
	int location = 0;
	int length = 0;
	int fd, saveout, oldout;
	bool spcs = false;
	bool mult = false;
	char * cstr;
	string shellcmds;
	string tmp;

	if(input1.find("&") != -1)
	{
		string commandset[100];
		strcpy(input, input1.c_str());

		for (int i = 0; i < input1.length(); i++)
		{
			if(((input[i] == ' ') && (input[i+1] == '&')) || ((input[i] == ' ') && (input[i-1] == '&')))
			{
				spaces[count] = i;
				count++;
				spcs = true;
			}
		}
		if(spcs)
		{
			while(spaces[count2] != 0)
			{
				input1.erase(spaces[count2] - offset , 1);
				count2++;
				offset++;
			}
		}

		strcpy(input, input1.c_str());
		count = 0;
		for (int i = 0; i < input1.length(); i++)
		{
			if(input[i] == '&')
			{
				ands[count] = i;
				count++;
				mult = true;
			}
		}
		if(mult)
		{
			tmp = "";
			for (int a = 0; a < ands[location]; a++)
			{
				tmp += input[a];
			}
			commandset[location] = tmp;

			while(ands[location + 1] != 0)//handles all but the last one
			{
				tmp = "";
				for (int a = ands[location] + 1; a < ands[location + 1]; a++)
				{		
						tmp += input[a];
				}
				commandset[location + 1] = tmp;
				location++;
			}

			tmp = "";
			for (int a = ands[location] + 1; a <= input1.length(); a++)
			{
				tmp += input[a];
			}
			commandset[location + 1] = tmp;
			mult = false;
		}
		multcmd(commandset, pathstat);
	}

	else
	{
		if (input1.find(">") != -1)
		{
			
			string file_location = input1;
			file_location.erase(0,input1.find(">") + 2);
			input1.erase(input1.find(">") - 1, input1.length());
			cstr = new char[file_location.length()];
			strcpy(cstr,file_location.c_str());
			
						
			fd = open(cstr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			
			saveout = dup(fileno(stdout));
			oldout = dup(1);

			dup2(fd,fileno(stdout));

			delete[] cstr;
			
		}

		ifstream infile(input1);
		while(getline(infile, shellcmds))
		{

			if (shellcmds.compare(0,4,"exit") == 0)
				exit(0);

			else if (shellcmds.compare(0,2,"ls") == 0 && !pathstat)
			{
				char* cwd2 = getcwd(cwd2,100);
				stringstream ss;
				string adder;
				ss << cwd2;
				ss >> adder;
				ls(shellcmds, adder);
			}

			else if(shellcmds.compare(0,2,"cd") == 0)
				cd(shellcmds);

			else if(shellcmds.compare(0,3,"cat") == 0 && !pathstat)
				cat(shellcmds);

			else if(shellcmds.compare(0, 2, "rm") == 0 && !pathstat)
				rm(shellcmds);
			
			else if(shellcmds.compare(0, 4, "path") == 0)
				path(shellcmds);

			else if(shellcmds.compare(0, 4, "echo") == 0 && !pathstat)
				echo(shellcmds);

			else if (shellcmds.compare(0,5, "sleep") == 0 && ! pathstat)
				sleep(shellcmds);

			else if(shellcmds.length() == 0)
			{
			}
			else
			{
				cout<<"error command not found" << endl;
			}
		}
		
	}
	dup2(oldout, 1);
	close(fd);
	
	

}

void echo(string input)
{
	cout << input.erase(0,5)<<endl;
}

void sleep(string input)
{
	input.erase(0,6);
	int timer = stoi(input);
	sleep(timer);
}

void path(string input)
{
	int spaces[100];
	string tmp;
	bool fwdslash;
	int location = 0;
	int length = 0;
	char char_array[input.length() + 1];
	int count = 0;
	char const * c;

	strcpy(char_array, input.c_str());


	for (int i = 0; i < input.length(); i++)
	{
		
		if(char_array[i] == ' ')
		{
			spaces[count] = i;
			count++;
		}
	}
	if (input.length() == 4)
	{
		GlobalPath[0] = "0";
	}
	else
	{
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
			GlobalPath[location] = tmp;
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
		GlobalPath[location] = tmp;
	}

	// while(!files[location].empty())
	// {
	// 	c = files[location].data();
	// 	if(access(c,X_OK) == -1)
	// 	{
	// 		cout <<"error could not find file"<<endl;
	// 	}
	// 	location++;
	// }
}

void ls(string input, string cwd)
{
	int rc, found, found2, saveout, fd, fd2;
	string file_location, testfile, testfile2;
	string ls = "ls";
	
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
						if (input.find(" ") !=-1)
						{
							
							testfile = input.substr((found+2),(found2-(found+2)));
						}
						else
						{
							testfile = input.substr((found),(found2-(found)));
						}
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

						//string ls = "ls";
						char* argm[2];
						argm[0] = (char*)ls.c_str();
						argm[1] = NULL;
						execvp(argm[0], argm);
						cout << "error";
				}
				else
				{
					if (found >= 3)
					{	
						if(input.length() == 4)
						{
							cout << "bash: syntax error near unexpected token `newline'"<<endl;
							exit(1);
						}
						
						testfile = input.substr(found+1,input.length());
						char * cstr = new char[testfile.length()];
						strcpy(cstr,testfile.c_str());
						
						fd = open(cstr, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
						
						saveout = dup(fileno(stdout));

						dup2(fd,fileno(stdout));

						delete[] cstr;
						
						file_location = input.substr(3,found-4);
						cd(file_location);

						//string ls1 = "ls";
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

						//string ls2 = "ls";
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
					DIR *blah = NULL;
					struct dirent *DirEntry = NULL;
					unsigned char isFile =0x8;
					char const * filecmd; 
					//string ls = "ls";
					
					file_location = input.substr(3,input.length());
					cwd += "/";
					cwd += file_location;
					filecmd = cwd.data();
					blah = opendir(filecmd);
					if(blah == NULL) //we have a file
					{
						char* argm[3];
						argm[0] = (char*)ls.c_str();
						argm[1] = (char*)file_location.c_str();
						argm[2]	= NULL;
						execvp(argm[0], argm);
						cout << "error";
					}
					else
					{
						cd(file_location);
						char* argm[2];
						argm[0] = (char*)ls.c_str();
						argm[1] = NULL;
						execvp(argm[0], argm);
						cout << "error";
					}
				}
				else
				{
					//string ls = "ls";
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
		if(input.find("cd") != -1)
		{
		directory = input.substr(3,input.length()-2);
		}
		
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
	int count = 0;
	int spaces[100];
	string newinput;
	char const * c;
	char char_array[input.length() + 1];

	strcpy(char_array, input.c_str());
	
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
					cout << "error this is a diretory" << endl;
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
		else
		{
			char* cwd2 = getcwd(cwd2,100);
			stringstream ss;
			string adder;
			ss << cwd2;
			ss >> adder;
			found = input.find(" ");
			command = input;
			command.erase(0,found + 1);
			adder +="/";
			adder += command;
			char const * filecmd = command.data();
			blah = opendir(adder.data());
			if(blah == NULL)
			{
				if(remove(filecmd) != 0)
				{
					cout << "file does not exits"<< endl;
					exit(1);
				}
			}
			else
			{
				cout << "error this is a directory"<<endl;	
			}
			
			
		}
		

	}
}


