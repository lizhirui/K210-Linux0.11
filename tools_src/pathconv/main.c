#include <stdio.h>
#include <string.h>
#include <windows.h>

int main(int argc,char **argv)
{
    int i;
    int j;
    int len;
    int totallength = 0;
    char *buf;
    int hasblank = 0;
    char *ptr;

    for(i = 1;i < argc;i++)
    {
        len = (int)strlen(argv[i]);
        totallength += len + 3;//两个引号和一个空格
    }

    buf = malloc(totallength + 1);
    ptr = buf;

    for(i = 1;i < argc;i++)
    {
        len = (int)strlen(argv[i]);
        hasblank = 0;

        for(j = 0;j < len;j++)
        {
            if(argv[i][j] == ' ')
            {
                hasblank = 1;
                break;
            }
        }

        if(hasblank == 1)
        {
            *(ptr++) = '\"';
        }

        memcpy(ptr,argv[i],len);

        for(j = 0;j < len;j++,ptr++)
        {
            if(*ptr == '/')
            {
                *ptr = '\\';
            }
        }

        if(hasblank == 1)
        {
            *(ptr++) = '\"';
        }

        *(ptr++) = ' ';
    }

    *(ptr - 1) = 0x00;
    system(buf);
    free(buf);
    return 0;
}