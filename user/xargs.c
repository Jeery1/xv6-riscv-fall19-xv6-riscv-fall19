#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_LINE 512  // 单行最大长度
#define MAX_ARGS 32   // 最大参数数量

// 自定义读取函数，正确处理EOF(Ctrl+D)
int read_line(char *buf, int max) {
    int i = 0;
    char c;
    
    while(i < max - 1) {
        int n = read(0, &c, 1);
        if(n <= 0) return n;  // EOF或错误
        if(c == '\n' || c == '\r') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    int arg_count;
    
    // 1. 初始化基础命令参数
    if(argc < 2) {
        fprintf(2, "Usage: xargs command [args...]\n");
        exit(1);
    }

    // 复制原始命令参数(跳过argv[0]的"xargs")
    for(arg_count = 0; arg_count < argc - 1; arg_count++) {
        args[arg_count] = argv[arg_count + 1];
    }

    // 2. 逐行读取标准输入
    while(1) {
        memset(line, 0, sizeof(line));
        int ret = read_line(line, sizeof(line));
        
        // 处理EOF(Ctrl+D)
        if(ret <= 0) break;
        
        // 跳过空行
        if(strlen(line) == 0) continue;

        // 3. 分割当前行参数
        char *p = line;
        char *arg;
        while((arg = strchr(p, ' ')) != 0 && arg_count < MAX_ARGS - 1) {
            *arg = '\0';
            if(*p != '\0') {
                args[arg_count++] = p;
            }
            p = arg + 1;
        }

        // 添加最后一个参数
        if(*p != '\0' && arg_count < MAX_ARGS - 1) {
            args[arg_count++] = p;
        }

        // 4. 执行命令
        args[arg_count] = 0;  // NULL终止
        int pid = fork();
        if(pid < 0) {
            fprintf(2, "xargs: fork failed\n");
            exit(1);
        } else if(pid == 0) {
            exec(args[0], args);
            fprintf(2, "xargs: exec %s failed\n", args[0]);
            exit(1);
        } else {
            wait(0);
        }

        // 5. 重置参数计数器(保留原始命令参数)
        arg_count = argc - 1;
    }

    exit(0);
}