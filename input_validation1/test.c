#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char cmd[256];
    char *user_input = argv[1];

    snprintf(cmd, sizeof(cmd), "echo Input: %s", user_input);

    system(cmd);
    return 0;
}
