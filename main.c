#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	char * robot_simulation[3] = {"./robot_simulation", NULL};
    char * robot_controller[3] = {"./robot_controller", NULL};

	pid = fork();

	if (pid == 0)
    {
		execvp(robot_simulation[0], robot_simulation);
	} 
    else
    {
        pid = fork();
        if (pid == 0)
        {
            execvp(robot_controller[0], robot_controller);
        }
		// waitpid(pid, NULL, 0);
	}

    /* Wait for threads*/
	while(getc(stdin)=='q') {}
	
    return EXIT_SUCCESS;
}