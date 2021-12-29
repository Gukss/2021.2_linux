#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void timeover(int signum)
{
    return;
}

void sleep3()
{
    int timelimit=3;
    struct sigaction act;

    act.sa_handler=timeover;
    sigaction(SIGALRM, &act, NULL);

    alarm(timelimit);
    pause();
}
void main()
{
    int i = 0;
    while(1)
    {
        printf("%d\n", i);
        sleep3();
        i++;
    }
}