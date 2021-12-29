#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void cat(FILE* fp, int nFlags, int EFlags)
{
    char c;
    int i = 1;
    int flag = 1;
    while ((c = fgetc(fp)) != EOF)
    {
        if (nFlags && flag) {
            //printf ���� �ٸ��� ����� ����
            printf("%4d", i++);
            flag = 0;
        }
        if (c == '\n') {
            flag = 1;
        }
        if (c == '\n' && EFlags) {
            fputc('$', stdout);
        }
        fputc(c, stdout);
    }
    return;
}

int main(int argc, char* argv[])
{
    FILE* in;
    int opt = 0;
    int EFlags = 0;
    int nFlags = 0;

    while ((opt = getopt(argc, argv, "En")) != -1)
    {
        switch (opt) {
        case 'E':
            EFlags = 1; break;
        case 'n':
            nFlags = 1; break;
        default:
            break;
        }
    }
    //���ڰ� ���� ���
    if (argc < 2)
    {
        cat(stdin, nFlags, EFlags);
    }
    //���ڰ� �ִ� ���
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-') {
                i++;
            }
            in = fopen(argv[i], "r");
            //���� ���� �� ���� ó��
            if (errno) {
                printf("MyCat: %s: %s\n", argv[i], strerror(errno));
                continue;
            }
            cat(in, nFlags, EFlags);
            fclose(in);
        }
    }
    return 0;
}