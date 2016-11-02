#include<stdio.h>
#include<string.h>

#define BUFFERSIZE 56 /*输出的进程列表每一行的长度，其实就是sizeof(s)*/
/*这个宏是为了处理向前seek时到达文件头出现的异常，因为时从末尾开始处理的，所以向后seek不会有异常的*/
#define FSEEK(stream, offset, whence) {                                                                \
                                        if(fseek((stream), (offset), (whence)) == -1){                 \
                                            printf("You should run \"insmod\" and \"rmmod\" first\n"); \
                                            return 0;                                                  \
                                        }                                                              \
                                      }
#define  RETURN {fclose(fp); return 0;} /*这个宏是为了保证在程序退出前关闭fp*/

int main(){
    char buf[BUFFERSIZE]; /*储存进程列表一行信息用的字符串*/
    char c; /*缓存用的字符变量*/
    char* s = "            name     pid           state          parent"; /*表头字符串常量，用来在文件中查找表头位置，也就是第一行的位置*/
    FILE* fp = fopen("/var/log/kern.log", "r"); /*kernel.log为系统log所在位置，打开并读取这个文件，从中取得用户态需要的信息*/

    FSEEK(fp, -1, SEEK_END); /*因为最好是读取最后一次输出的结果，故从文件尾向前寻找表头*/
     /*这里开始查找表头的位置，具体思路是让文件指针从文件尾开始向前遍历，读取每一行最后的BUFFERSIZE长度的字符串，
      *如果该字符串与表头相同，则判定找到了第一行*/
    while(1){
        c = fgetc(fp);
        FSEEK(fp, -1, SEEK_CUR);
        while(c != '\n'){ /*先找到换行符，也就是寻找行尾*/
            FSEEK(fp, -1, SEEK_CUR);
            c = fgetc(fp);
            FSEEK(fp, -1, SEEK_CUR);
        }
        FSEEK(fp, -BUFFERSIZE, SEEK_CUR); /*从行尾往前数BUFFERSIZE个字节*/
        fread(buf, sizeof(char), BUFFERSIZE, fp); /*在读BUFFERSIZE个字节就得到一行长度的字符串*/
        FSEEK(fp, -1, SEEK_CUR); /*这一句是为了处理两个换行符相邻的情况，往回读一个字节，在循环下来就能正常读取一行*/
        if(strcmp(buf, s) == 0){ /*比较这一行是否就是表头*/
            FSEEK(fp, 1, SEEK_CUR); /*如果找到表头，先得抵消上面那个-1*/
            FSEEK(fp, -BUFFERSIZE, SEEK_CUR); /*然后回到表头的第一个字符位置*/
            break;
        }
    }
    /*这里开始从表头第一个字符输出用户态所有信息，为便于过滤掉无关信息，这里选择一个一个字符读取，知道读取到"End..."处*/
    printf("*********************************************************\n");
    while((c = fgetc(fp)) != EOF){ /*逐个往后读取*/
        if(c == '\n'){ /*因为log文件每一行开头有一段用户态不需要的信息，所以每行行尾要开始特殊处理*/
            printf("\n");
            while(c != ']'){ /*过滤到上行行尾到这行"]"之间的字符*/
                c = fgetc(fp);
                if(c == EOF) /*防止在这之间到达文件末尾*/
                    RETURN
            }
            c = fgetc(fp); /*要输出的字符*/
            if(fgetc(fp) == 'E') /*如果到达"End..."就结束*/
                RETURN
            else
                FSEEK(fp, -1, SEEK_CUR); /*抵消上面fgetc()移动的1字符*/
        }
        printf("%c", c);
    }

    RETURN
}
