#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	pid_t pid_simulation, pid_controller;
	char * robot_simulation[3] = {"./robot_simulation", NULL};
    char * robot_controller[3] = {"./robot_controller", NULL};

	pid_simulation = fork();

	if (pid_simulation == 0)
    {
		execvp(robot_simulation[0], robot_simulation);
	} 
    else
    {
        pid_controller = fork();
        if (pid_controller == 0)
        {
            execvp(robot_controller[0], robot_controller);
        }
	}

	// /* Wait for processes*/
	while(getc(stdin)=='q') {}

    kill(pid_simulation, SIGRTMIN);
    waitpid(pid_simulation, NULL, 0);
    kill(pid_controller, SIGRTMIN);
    waitpid(pid_controller, NULL, 0);

    printf("Main exits\n");
    return EXIT_SUCCESS;
}