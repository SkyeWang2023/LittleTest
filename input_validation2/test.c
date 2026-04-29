#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char filename[256];
    char buffer[1024];

    // 用户输入文件名，直接拼接路径
    char *user_input = argv[1];

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
