#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 1
int main(int argc, char *argv[])
{
    FILE *src, *dst;
    char buffer[BUF_SIZE];
    if(argc == 1){
        printf("mycp: 파일 명령어 누락\n");
    }
    else if(argc == 2){
        printf("mycp: '%s' 다음에 대상 파일 명령이 누락됨\n", argv[1]);
    }
    else if(argc == 3){
        src = fopen(argv[1], "r");
        //읽을 파일을 열기 실패
        if(errno)
        {
            printf("mycp: '%s': %s\n", argv[1], strerror(errno));
            exit(0);
        }
        dst = fopen(argv[2], "w");
        //쓸 파일 열기 실패
        if(errno)
        {
            printf("mycp: '%s': %s\n", argv[1], strerror(errno));
            exit(0);
        }
        while(fread(buffer, 1,sizeof(buffer), src) > 0)
        {
            fwrite(buffer, 1, sizeof(buffer), dst);
        }
        fclose(src);
        fclose(dst);
    }
    return 0;
}