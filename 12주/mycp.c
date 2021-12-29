#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#define BUF_SIZE 1024
int main(int argc, char *argv[])
{
    int fdin, fdout;
    ssize_t nread;
    char buffer[BUF_SIZE];
    struct stat statbufin, statbufout;

    switch (argc)
    {
    case 1:
        fprintf(stderr, "mycp: 파일 명령어 누락\n");
        break;
    case 2:
        fprintf(stderr, "mycp: '%s' 다음에 대상 파일 명령이 누락됨\n", argv[1]);
        break;
    case 3:
        fdin = open(argv[1], O_RDONLY);
        if(fdin == -1)
        {
            fprintf(stderr, "mycp: '%s' 를 설명할 수 없음: 그런 파일이나 디렉터리가 없습니다\n", argv[1]);
            exit(1);
        }
        fstat(fdin, &statbufin);
        fdout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, statbufin.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
        fstat(fdout, &statbufout);

        if(fdout == -1)
        {
            //b가 디렉터리인 경우
            chdir(argv[2]);
            fdout = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, statbufin.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
            if(fdout == -1)
            {
                fprintf(stderr, "mycp: 디렉터리 '%s/%s'를 디렉터리가 아닌 것으로 덮어쓸 수 없음\n", argv[2],argv[1]);
                exit(1);
            }
            while ((nread = read(fdin, buffer, BUF_SIZE)) > 0)
            {
                if (write(fdout, buffer, nread) < nread)
                {
                    fprintf(stderr, "Write Error fdout");
                    break;
                }
            }
        }
        
        //일반파일이면, a가 심볼릭 링크인 경우 원본 파일의 내용 복사
        if (S_ISREG(statbufin.st_mode) || S_ISREG(statbufout.st_mode))
        {
            while ((nread = read(fdin, buffer, BUF_SIZE)) > 0)
            {
                if (write(fdout, buffer, nread) < nread)
                {
                    fprintf(stderr, "Write Error");
                    break;
                }
            }
        }
        //a가 디렉터리인 경우
        else if (S_ISDIR(statbufin.st_mode))
        {
            fprintf(stderr, "mycp: -r not specified; omitting directory '%s'\n", argv[1]);
            exit(1);
        }
        //일반파일이 아니면
        else
        {
            fprintf(stderr, "Error");
            exit(1);
        }
        break;

    default:
        
        fdout = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, statbufin.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
        if(!(fdout == -1))
        {
            fprintf(stderr, "mycp: 대상 '%s' 은 디렉터리가 아님\n", argv[argc-1]);
            exit(1);
        }
        for(int i=1; i<argc-1; i++)
        {
            fdin = open(argv[i], O_RDONLY);
            fstat(fdin, &statbufin);
            if (fdin == -1)
            {
                fprintf(stderr, "mycp: '%s' 를 설명할 수 없음: 그런 파일이나 디렉터리가 없습니다\n", argv[i]);
                exit(1);
            }
            chdir(argv[argc - 1]);
            fdout = open(argv[i], O_WRONLY | O_CREAT | O_TRUNC, statbufin.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
            
            while ((nread = read(fdin, buffer, BUF_SIZE)) > 0)
            {
                if (write(fdout, buffer, nread) < nread)
                {
                    fprintf(stderr, "Write Error fdout");
                    break;
                }
            }
            chdir("..");
        }
        break;
    }
    close(fdin);
    close(fdout);

    return 0;
}