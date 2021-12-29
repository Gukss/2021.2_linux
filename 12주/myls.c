#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

typedef struct Node {
    struct Node* next;
    char dirpath[256];
}Node;

void append(Node* list, char* newpath)
{
    if (list->next == NULL)
    {
        Node* newNode = malloc(sizeof(Node));
        strcpy(newNode->dirpath, newpath);
        newNode->next = NULL;
        list->next = newNode;
    }
    else
    {
        Node* cur = list;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        Node* newNode = malloc(sizeof(Node));
        strcpy(newNode->dirpath, newpath);
        newNode->next = NULL;
        cur->next = newNode;
    }
}

int basic(unsigned int flag, char* path)
{
    struct dirent* dentry = NULL;
    struct stat statbuf;
    char fullpath[256];
    struct tm* time;
    long int total = 0;

    Node* list1 = malloc(sizeof(Node));
    list1->next = NULL;

    struct dirent** namelist;
    int listcount = 0;
    int i = 0;

    //옵션 R 처리
    if ((flag & 0x20) != 0)
    {
        printf("%s:\n", path);
    }

    listcount = scandir(path, &namelist, NULL, alphasort);
    //옵션 r 설정
    if ((flag & 0x40) != 0)
    {
        i = listcount - 1;
    }
    while (1)
    {
        //옵션 r  종료 설정
        if (i < 0 || i > listcount - 1)
        {
            break;
        }

        if (namelist[i]->d_ino == 0)
        {
            if ((flag & 0x40) != 0)
            {
                i--;
            }
            else
            {
                i++;
            }
        }
        //옵션 a 처리
        if ((flag & 0x01) == 0)
        {
            if (strncmp(namelist[i]->d_name, ".", 1) == 0)
            {
                //옵션 r  숨김파일 있으면 인덱스 바꾸고 continue
                if ((flag & 0x40) != 0)
                {
                    i--;
                    continue;
                }
                else
                {
                    i++;
                    continue;
                }
            }
        }

        strcpy(fullpath, path);
        strcat(fullpath, "/");
        strcat(fullpath, namelist[i]->d_name);

        if (lstat(fullpath, &statbuf) < 0)
        {
            fprintf(stderr, "stat error\n");
        }

        //옵션 i 처리
        if ((flag & 0x02) != 0)
        {
            printf("%7ld ", namelist[i]->d_ino);
        }

        //옵션 s 처리
        if (((flag & 0x04) != 0) || ((flag & 0x10) != 0))
        {
            if ((flag & 0x04) != 0)
                printf("%7ld ", statbuf.st_blocks / 2);
            total = total + statbuf.st_blocks / 2;
        }

        //옵션 l 처리
        if ((flag & 0x10) != 0)
        {
            //파일 유형
            switch (statbuf.st_mode & S_IFMT)
            {
            case S_IFREG:
                printf("-");
                break;
            case S_IFDIR:
                printf("d");
                break;
            case S_IFIFO:
                printf("p");
                break;
            case S_IFLNK:
                printf("l");
                break;
            case S_IFCHR:
                printf("c");
                break;
            case S_IFBLK:
                printf("b");
                break;
            case S_IFSOCK:
                printf("s");
                break;
            default:
                break;
            }

            //소유자권한
            if ((S_IRUSR & statbuf.st_mode) != 0)
                printf("r");
            else
                printf("-");
            if ((S_IWUSR & statbuf.st_mode) != 0)
                printf("w");
            else
                printf("-");
            if ((S_IXUSR & statbuf.st_mode) != 0)
                printf("x");
            else
                printf("-");
            //그룹 권한
            if ((S_IRGRP & statbuf.st_mode) != 0)
                printf("r");
            else
                printf("-");
            if ((S_IWGRP & statbuf.st_mode) != 0)
                printf("w");
            else
                printf("-");
            if ((S_IXGRP & statbuf.st_mode) != 0)
                printf("x");
            else
                printf("-");
            //기타 권한
            if ((S_IROTH & statbuf.st_mode) != 0)
                printf("r");
            else
                printf("-");
            if ((S_IWOTH & statbuf.st_mode) != 0)
                printf("w");
            else
                printf("-");
            if ((S_IXOTH & statbuf.st_mode) != 0)
                printf("x");
            else
                printf("-");
            printf(" ");

            //link
            printf("%2ld ", statbuf.st_nlink);

            //사용자명
            printf("%s ", getpwuid(statbuf.st_uid)->pw_name);
            //그룹명
            printf("%s ", getgrgid(statbuf.st_gid)->gr_name);
            //파일 사이즈
            printf("%7ld ", statbuf.st_size);
            //파일 접근시간
            time = localtime(&statbuf.st_mtime);
            printf("%2d월 %2d %2d:%2d ", time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min);
        }
        //기본 출력
        printf("%s", namelist[i]->d_name);

        //옵션 F 처리
        if ((flag & 0x08) != 0)
        {
            if (S_ISDIR(statbuf.st_mode))
                printf("/");
            else if (S_ISSOCK(statbuf.st_mode))
                printf("=");
            else if (S_ISFIFO(statbuf.st_mode))
                printf("|");
            else if (S_ISLNK(statbuf.st_mode))
                printf("@");
            else if ((S_IXUSR & statbuf.st_mode) != 0)
                printf("*");
        }
        printf("\n");

        //옵션 R 처리
        if ((flag & 0x20) != 0)
        {
            if (S_ISDIR(statbuf.st_mode))
            {
                //링크드리스트로 구현하기
                append(list1, fullpath);
            }
        }

        //옵션 r  인덱스 바꾸기
        if ((flag & 0x40) != 0)
        {
            i--;
        }
        else
        {
            i++;
        }
    }

    //옵션 s 처리
    if (((flag & 0x04) != 0) || ((flag & 0x10) != 0))
    {
        printf("합계 %ld", total);
    }

    printf("\n");

    //옵션 R 처리
    if ((flag & 0x20) != 0)
    {
        Node* cur = list1->next;
        while (cur != NULL)
        {
            basic(flag, cur->dirpath);
            cur = cur->next;
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int opt = 0;
    unsigned int flag = 0;
    char dir[256][256];
    char path[256];
    int i = 0;

    while ((opt = getopt(argc, argv, "aisFlRr")) != -1)
    {
        switch (opt)
        {
        case 'a':
            if ((flag & 0x01) == 0)
            {
                flag |= 0x01;
            }
            break;
        case 'i':
            if ((flag & 0x02) == 0)
            {
                flag |= 0x02;
            }
            break;
        case 's':
            if ((flag & 0x04) == 0)
            {
                flag |= 0x04;
            }
            break;
        case 'F':
            if ((flag & 0x08) == 0)
            {
                flag |= 0x08;
            }
            break;
        case 'l':
            if ((flag & 0x10) == 0)
            {
                flag |= 0x10;
            }
            break;
        case 'R':
            if ((flag & 0x20) == 0)
            {
                flag |= 0x20;
            }
            break;
        case 'r':
            if ((flag & 0x40) == 0)
            {
                flag |= 0x40;
            }
            break;
        default:
            break;
        }
    }
    int j = 0;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            continue;
        }
        else
        {
            strcpy(dir[j], argv[i]);
            j++;
        }
    }
    //인자가 없는 경우
    i = 0;
    if (strlen(dir[i]) == 0)
    {
        strcpy(dir[i], ".");
    }
    while (strlen(dir[i]))
    {
        chdir(dir[i]);
        getcwd(path, 256);
        if (argc > 2)
        {
            printf("%s:\n", path);
        }
        basic(flag, path);
        i++;
        printf("\n");
        chdir("..");
    }
}