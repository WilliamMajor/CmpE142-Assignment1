#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

void ls(char *input);
void cd(char *input);
char * lscd(char *input);

int
main(int argc, char **argv)
{
    char *line = NULL;
    size_t linesize = 0;
    size_t linelen;
    int rc;
    while ((linelen = getline(&line, &linesize, stdin)) != -1) //Want to figure out a way to pring out the shell name before this each time
    {
        if (strncmp("exit", line, 4) == 0) 
        {
            exit(0); // user wants to exit
        }

        else if (strncmp("ls", line, 2) == 0)
        {
            ls(line); // user wants to ls 
        }

        else if (strncmp("cd", line, 2) == 0)
        {
            cd(line); // user wants to change directory
        }
        else if (strncmp("", line, 0) == 0)
        {
            //nothing called
            //do nothing
        }
        else
        {
            line[strcspn(line, "\n")] = 0;
            printf("error command: %s not found", line);
        }
        

    }
    free(line);
    if (ferror(stdin))
        err(1, "getline");    
}


/********************************* Command Functions **********************************/
void ls(char *input)
{
    int rc;
    rc = fork();
            if (rc < 0)//error
            {
                fprintf(stderr, "fork failed\n");
                exit(1);
            }
            else if (rc == 0)//Child
            {
               if (access("/bin/ls",X_OK) == -1) // make sure the ls command is where we think it is
                {
                    if(access("/usr/bin/ls",X_OK) == -1) // if we dont see the ls command here either we have an error
                    {
                        printf("error no ls command found in /bin/ls or /user/bin/ls");
                        exit(-1);
                    } 
                } 
                else
                {   
                    char * position = strchr(input, '>');
                    char * position2 = strchr(input, ' ');
                    /* this still needs a lot of work -_- need to send everything between ls and > 
                    get into that directory then open and write to a file specified after > 
                    in there with the output of ls probably need to parse the file we also need to handle writing multiple files
                    like this test ls > output.9 > output.10 */
                    if(position)// we found a file need for an output redirect
                    {
                        if (strlen(position) == strlen(position2)-1) //check if we need to change directory or not
                        {   
                            char s[100];
                            char *nd;
                            memmove(input, input + 3, strlen(input)); //gets rid of ls and the space after it

                            //todo: make new file with specified file or open if existed and write result of ls to it
                            // nd = lscd(input);
                            // chdir(nd);
                            // free(nd);
                        }
                    }
                    else if (strlen(input) > 3)
                    {
                        memmove(input, input + 3, strlen(input));
                        input[strcspn(input, "\n")] = 0;
                        char *argm[] = {"ls", input, NULL};
                        execvp(argm[0], argm);
                        printf("wow errors");
                    }
                    else
                    {
                        char *argm[] = {"ls", NULL};
                        execvp(argm[0], argm);
                        printf("obviously there was an error here, damn");
                    }
                }
                
            }
            else //Parent
            {
                wait(NULL);
            }
}

void cd(char *input) //cd command Think this is pretty done...Nope found a bug not adding in initial / to path when the path has other /s...
{
        char s[100];
        char sep[50];
        char newinput[100];
        
        strcpy(sep,"/");
        memmove(input, input + 3, strlen(input));
        strcpy(newinput,input);
        if(strlen(newinput) == 0 || strlen(newinput) == 1)
        {
            chdir(getenv("HOME"));
        }

        
        else
        {
            getcwd(s, 100); 

            newinput[strcspn(newinput, "\n")] = 0;
            s[strcspn(s, "\n")] = 0;
            sep[strcspn(sep, "\n")] = 0;
            
            strcat(sep,newinput);
            s[strcspn(s, "\n")] = 0;
            strcat(s,sep);
        
            if(chdir(s) == -1)
            {
                printf("bash: cd: %s: No such file or directory\n", s);
            }
        }
}
// Thinking this is how im going to help accomplish part of the ls > commands... 
char * lscd(char *input) 
{
     
    char s[100];
    char sep[50];
    char newinput[100];
    char * returnvar;
    returnvar = (char *) malloc(100);
    
    strcpy(sep,"/");
    memmove(input, input + 3, strlen(input));
    strcpy(newinput,input);
    if(strlen(newinput) == 0)
    {
        chdir(getenv("HOME"));
    }
    else
    {
        getcwd(s, 100); 

        newinput[strcspn(newinput, "\n")] = 0;
        s[strcspn(s, "\n")] = 0;
        sep[strcspn(sep, "\n")] = 0;
        if (strchr(input, '/') == NULL)
        {
            strcat(sep,newinput);
            s[strcspn(s, "\n")] = 0;
            strcat(s,sep);
            strcpy(returnvar, s);
        }
        else
        {
            s[strcspn(s, "\n")] = 0;
            strcat(s,newinput);
            strcpy(returnvar, s);
        }
        if(chdir(s) == -1)
        {
            printf("bash: cd: %s: No such file or directory\n", s);
        }
        return returnvar;
    }


}