#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int index;
    int len;
    char str[100]; // esp+0x28

    index = 0;
    fgets(str, 100, stdin);
    index = 0;
    len = strlen(str);
    while (index < len)
    {
        if (str[index] > 64 && str[index] <= 90)
            str[index] = str[index] ^ 32;
        len = strlen(str);
        index++;
    }
    printf(str);
    exit(0);
}
