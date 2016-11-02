#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h> /*task结构体所在的头文件*/
#include<linux/list.h>
#include<linux/string.h>
/*
 * 宏定义一些字符串
 */
#define DRIVER_LICENSE "GPL" /*模块遵守的协议*/
#define DRIVER_AUTHOR "Liao Jinyuan<liaojinyuan@zju.edu.cn>" /*模块作者*/
#define DRIVER_DESC "A simple module to show task information" /*模块描述*/

/*
 * 入口函数，在insmod时执行
 */
static int __init taskInfoInit(void){
    struct task_struct *task, *p; /*用task取得链表头，用p遍历每个节点*/
    struct list_head *pos; /*遍历链表用的位置游标*/
    int unrunnable      = 0, /*以下为task各种状态的计数变量，初始为0*/
        running         = 0,
        interruptible   = 0,
        uninterruptible = 0,
        exit_dead       = 0,
        exit_zombie     = 0,
        dead            = 0,
        wakekill        = 0,
        waking          = 0,
        parked          = 0,
        noload          = 0,
        total           = 0; /*总共的个数*/
    char state[16]; /*存储状态名的字符串*/


    printk(KERN_INFO "\n********************************************************\n");
    printk(KERN_INFO "%16s%8s%16s%16s\n\n", "name", "pid", "state", "parent"); /*输出结果的表头，有4项*/

    task = &init_task; /*初始化一个task结构*/
    list_for_each(pos, &task->tasks){ /*tasks为链表头节点*/
        p = list_entry(pos, struct task_struct, tasks); /*取得遍历的当前节点*/

        /*对于每个task，根据结构中的state值判断其状态，保存下其状态名，并将相应状态计数器+1，这些值均在sched.h中定义*/
        switch(p->state){ /* -1 unrunnable, 0 runnable, >0 stopped */
            case -1:
                strncpy(state, "unrunnable", 16); /*记录状态名，下同*/
                unrunnable++; /*更新计数器，下同*/
                break;
            case TASK_RUNNING: /*0*/
                strncpy(state, "running", 16);
                running++;
                break;
            case TASK_INTERRUPTIBLE: /*1*/
                strncpy(state, "interruptible", 16);
                interruptible++;
                break;
            case TASK_UNINTERRUPTIBLE: /*2*/
                strncpy(state, "uninterruptible", 16);
                uninterruptible++;
                break;
            case EXIT_DEAD: /*16*/
                strncpy(state, "exit_dead", 16);
                exit_dead++;
                break;
            case EXIT_ZOMBIE: /*32*/
                strncpy(state, "exit_zombie", 16);
                exit_zombie++;
                break;
            case TASK_DEAD: /*64*/
                strncpy(state, "dead", 16);
                dead++;
                break;
            case TASK_WAKEKILL: /*128*/
                strncpy(state, "wakekill", 16);
                wakekill++;
                break;
            case TASK_WAKING: /*256*/
                strncpy(state, "waking", 16);
                waking++;
                break;
            case TASK_PARKED: /*512*/
                strncpy(state, "parked", 16);
                parked++;
                break;
            case TASK_NOLOAD: /*1024*/
                strncpy(state, "noload", 16);
                noload++;
                break;
            default:
                break;
        }
        
        total++; /*更新总数计数器*/
        printk(KERN_INFO "%16s%8d%16s%16s\n", p->comm, p->pid, state, p->parent->comm); /*输出当前遍历的进程的信息*/
    }
    printk(KERN_INFO "********************************************************\n\n");

    /*输出出于各种状态的进程的计数值以及总数*/
    printk(KERN_INFO "********************************************************\n");
    printk(KERN_INFO "The statistics:\n");
    printk(KERN_INFO "%20s%6s\n", "STATE", "COUNT"); /*表头*/
    printk(KERN_INFO "%20s%6d\n", "unrunnable", unrunnable);
    printk(KERN_INFO "%20s%6d\n", "running", running);
    printk(KERN_INFO "%20s%6d\n", "interruptible", interruptible);
    printk(KERN_INFO "%20s%6d\n", "uninterruptible", uninterruptible);
    printk(KERN_INFO "%20s%6d\n", "exit_dead", exit_dead);
    printk(KERN_INFO "%20s%6d\n", "exit_zombie", exit_zombie);
    printk(KERN_INFO "%20s%6d\n", "dead", dead);
    printk(KERN_INFO "%20s%6d\n", "wakekill", wakekill);
    printk(KERN_INFO "%20s%6d\n", "waking", waking);
    printk(KERN_INFO "%20s%6d\n", "parked", parked);
    printk(KERN_INFO "%20s%6d\n", "noload", noload);
    printk(KERN_INFO "%20s%6d\n", "TOTAL", total); /*总数*/
    printk(KERN_INFO "********************************************************\n");
    return 0;
}

/*
 * 出口函数
 */
static void __exit taskInfoExit(void){
    printk(KERN_INFO "Exit...\n");
}

/*
 * 调用两个宏函数来指定上述函数成为入口函数和出口函数，它们在init.h中定义
 */
module_init(taskInfoInit);
module_exit(taskInfoExit);

/*
 * 调用指定模块信息的宏函数
 */
MODULE_LICENSE(DRIVER_LICENSE); /*模块协议*/
MODULE_AUTHOR(DRIVER_AUTHOR); /*模块作者*/
MODULE_DESCRIPTION(DRIVER_DESC); /*模块描述*/
