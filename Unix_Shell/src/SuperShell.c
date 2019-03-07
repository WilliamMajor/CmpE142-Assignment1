#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

void ls(void);

int
main(int argc, char **argv)
{
    char *line = NULL;
    size_t linesize = 0;
    size_t linelen;
    int rc;
    while ((linelen = getline(&line, &linesize, stdin)) != -1) 
    {
        if (strncmp("exit", line, 4) == 0) 
        {
            // user wants to exit
            exit(0);
        }

        if (strncmp("ls", line, 2) == 0)
        {
            ls();
        }
    }
    free(line);
    if (ferror(stdin))
        err(1, "getline");    
}


/********************************* Command Functions **********************************/
void ls(void)
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
               if (access("/bin/ls",X_OK) == -1)
                {
                    printf("error");
                } 
                else
                {
                    char *argm[] = {"ls", 0};
                    execvp(argm[0], argm);
                    printf("obviously there was an error here, damn");
                }
                
            }
            else //Parent
            {
                wait(NULL);
            }
}
