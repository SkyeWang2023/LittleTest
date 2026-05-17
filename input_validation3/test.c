#include <stdio.h>
#include <stdlib.h>

int test(char* filename)
{
    char buffer[1024];

    // 打开文件
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen failed");
        return 1;
    }

    // 读取并输出
    fread(buffer, 1, 1023, fp);
    printf("读取内容：\n%s\n", buffer);

    fclose(fp);
    return 0;
}
