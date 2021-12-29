#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define STR_LEN 1024
#define MAX_TOKENS 10

struct CMD {
    char *name;
    char *desc;
    int (*cmd) (int argc, char *argv[]);
};


int cmd_cd (int argc, char *argv[]);
int cmd_exit (int argc, char *argv[]);
int cmd_pwd (int argc, char *argv[]);
int cmd_help (int argc, char *argv[]);
int cmd_echo (int argc, char *argv[]);

struct CMD builtin[] = {
    {"cd", "change working directory", cmd_cd},
    {"exit", "exit from this shell", cmd_exit},
    {"pwd", "Print the name of the current working directory.", cmd_pwd},
    {"help", "Display information about builtin commands.", cmd_help},
    {"echo", "Write arguments to the standard output.", cmd_echo}
};

int cmd_cd (int argc, char *argv[])
{
    if(argc == 1)
    {
        chdir(getenv("HOME"));
    }
    else if(argc == 2)
    {
        if(chdir(argv[1]))
        {
            fprintf(stderr, "MyShell: cd: %s: 디렉터리가 아닙니다.\n", argv[1]);
            return 0;
        }
    }
    else
    {
        fprintf(stderr, "MyShell: cd: 인자가 너무 많음\n");
    }
    return 0;
}
int cmd_exit (int argc, char *argv[])
{
    _exit(0);
}
int cmd_pwd (int argc, char *argv[])
{
    char pwdbuf[STR_LEN];
    getcwd(pwdbuf, STR_LEN);
    printf("%s\n",pwdbuf);
    return 0;
}
int cmd_help (int argc, char *argv[])
{
    for(int i = 0; i < 5; ++i)
    {
        if(strcmp(builtin[i].name, argv[1]) == 0)
        {
            printf("%s\n", builtin[i].desc);
        }
    }

    return 0;
}
int cmd_echo (int argc, char *argv[])
{
    for(int i=1;i<argc;i++)
    {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}

//char cmdLine[STR_LEN]={'\0'};

void handler(int signum)
{
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigaction(signum, &act, NULL);
    fputs("\n", stdout);
}

int cmdProcessing(void)
{
    char *cmdTokens[MAX_TOKENS];

    char cmdLine[STR_LEN];

    char delim[] = " \t\n\r";
    char *token;
    int tokenNum;
    int exitCode = 0;
    pid_t child;

    struct sigaction act;
    act.sa_handler = handler;
    sigfillset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    cmdLine[0] = -128;
    fputs("[mysh v0.1] $ ",stdout);
    fgets(cmdLine, STR_LEN, stdin);

    tokenNum = 0;
    token = strtok(cmdLine, delim);
    while(token)
    {
        cmdTokens[tokenNum++] = token;
        token = strtok(NULL, delim);
    }
    cmdTokens[tokenNum] = NULL;

    for(int i = 0; i < 5; ++i)
    {
        if(strcmp(cmdTokens[0], builtin[i].name) == 0)
        {
            return builtin[i].cmd(tokenNum, cmdTokens);
        }
    }

    if(cmdLine[0] == -128)
    {
        return 0;
    }

    char path[MAX_TOKENS] = "/bin/";
    strcat(path, cmdTokens[0]);

    child = fork(); //내장 함수가 아니면 새 프로세스 생성
    
    if(child > 0)
    {
        int status;
        wait(&status);

        if(!status){
            return status;
        }
    }
    else if(child == 0)
    {
        act.sa_handler = SIG_DFL;
        sigaction(SIGINT, &act, NULL);

        execv(path, cmdTokens);
    }
    else
    {
        fputs("프로세스를 생성할 수 없습니다.\n", stdout);
    }
    return 0;
}

int main (void)
{
    int isExit = 0;

    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigfillset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    
    while(!isExit)
    {
        isExit = cmdProcessing();
    }
    fputs("마이 셸을 종료합니다.\n", stdout);
    return 0;
}