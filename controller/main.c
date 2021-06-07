#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "creator/creator.h"

#include "../pscommon/logger/logThread.h"
#include "../pscommon/logger/log.h"

const char* component = "CONTROLLER";

static void exit_handler(int);

int main(int argc, char *argv[])
{
    initLogger(component);
    LG_INF("Controller starts");

    /* Create empty signal set to run sigsuspend */
    sigset_t mask;
    sigemptyset(&mask);
    sigprocmask(0, NULL, &mask);
	sigaddset(&mask, SIGRTMAX);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    /* Prepare sigaction struct for handler */
    struct sigaction action;
    action.sa_handler = exit_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    /* Register signal handler for SIGINT */
    if (sigaction(SIGRTMIN, &action, NULL) < 0)
    {
        LG_ERR("Cannot register SIGRTMIN handler.");
        return -1;
    }

    createThreadsForController();

	while(1)
	{
		sigsuspend(&mask);
	}

	return EXIT_SUCCESS;
}

/* Exit signal handler function */
void exit_handler(int sig)
{
    clean();
    exit(EXIT_SUCCESS);
}
