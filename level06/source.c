#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <stdlib.h>

int auth(char *str, unsigned int number)
{
    int len;
    int calcul;
    int needed;
    int edx;
    int ecx;
    int index; // ebp-0x14
    long long result;

    len = strcspn(str, "\n");
    str[len] = 0;
    len = strnlen(str, 32);
    index = 0; // ebp-0x14
    if (len <= 5)
        return (1);
    if (ptrace(0, 0, (caddr_t)1, 0) == -1)
    {
        puts("\033[32m.---------------------------.");
        puts("\033[31m| !! TAMPERING DETECTED !!  |");
        puts("\033[32m'---------------------------'");
        return (1);
    }
    calcul = (str[3] ^ 4919) + 6221293; // ebp-0x10
    while (index < len)
    {
        if (str[index] <= 31)
            return (1);
        ecx = (str[index] ^ calcul);
        result = 2284010283 * ecx;
        edx = result >> 32;
        needed = ecx - edx;
        needed = needed >> 1;
        needed += edx;
        needed = needed >> 10;
        needed *= 1337;
        needed = ecx - needed;
        calcul += needed;
        getchar();
        index++;
    }
    if (number != calcul)
        return (1);
    return 0;
}

int main(int ac, char **av)
{
    char str[32];
    unsigned int number;
    puts("***********************************");
    puts("*\t\tlevel06\t\t  *");
    puts("***********************************");
    printf("-> Enter Login: ");
    fgets(str, 32, stdin);
    puts("***********************************");
    puts("***** NEW ACCOUNT DETECTED ********");
    puts("***********************************");
    printf("-> Enter Serial: ");
    scanf("%u", &number);
    if (auth(str, number) != 0)
    {
        return (1);
    }
    puts("Authenticated!");
    system("/bin/sh");
    return (0);
}