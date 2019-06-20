/*********************
*Project: TUBShell
*Description: a private shell for OS command by C
*Author: DJM ZHJ WQ
*Create Time: 2019.6.18
**********************/

/****** 库调用 ******/
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

/*** 自定义模块 ***/
#include "store.h"

#define MAX_NAME_LEN 100 
#define MAX_CMD_LEN 100 

/****** 函数声明 ******/
void sayHello();        //进入提示
void printPrefix();     //打印段前缀
int getInputCommand();  //获取输入命令

void shell();           //shell的总入口

/****** 函数实现 ******/
/***** Info *****/
/* Author: DJM */
/* Function: 用户进入TUBShell的入口声明 */
void sayHello(){
	/* Note: 打印进入了TUBShell的控制信息*/
	printf("This is TUBShell\n");
}

/***** Info *****/
/* Author: DJM */
/* Function: 打印前缀(每一次命令的开始)*/
void printPrefix(){
	/* Note: 打印的前缀分三部分 [\u@\h \f]*/
	/*第一部分,\u,用户名*/
	/*第二部分,\h,主机名*/
	/*第三部分,\f,当前所在的目录名或文件名*/
	char user[MAX_CMD_LEN] = "root",
	host[MAX_CMD_LEN] = "localhost",
	folder[MAX_CMD_LEN] = "xx";
	printf("[TUBShell %s@%s %s]", user, host, folder);
}

/***** Info *****/
/* Author: DJM */
/* Function: 等待输入命令,并获取命令信息*/
int getInputCommand(){
	/* Note: 注意命令输入的结束符号 */
	int i;
	int __switch = 1;
	char * __content;
	__content = (char *)malloc(MAX_CMD_LEN);
	char __exit[5] = "exit";

	scanf("%s", __content);
	printf("command \'%s\' no exist\n", __content);
	if(!strcmp(__content, __exit)){
		__switch = 0;
	}
	
	free(__content);
	return __switch;
}

/***** Info *****/
/* Author: WQ */
/* Function: 从OS获知部分前缀信息并放入存储区 */
void getprompt_wq(){
    	/* Note: 获取用户名，主机，超级用户确认*/

    	/* 获取用户名 */
	struct passwd *my_info;
	int uid = getuid();         //返回用户id
	my_info = getpwuid(uid);    //通过用户的uid查找用户的passwd数据
	store_promptPut(1, my_info->pw_name);

    	/* 获取主机名 */
	char hostname[MAX_NAME_LEN];
	gethostname(hostname, sizeof(hostname));
	store_promptPut(2, hostname);

    	/* 确认是否为超级用户 */
	if (uid == 0) {
		store_promptPut(4, "#");
	}
	else {
		store_promptPut(4, "$");
	}
}

/***** Info *****/
/* Author: none */
/* Function: Shell入口*/
void shell(){
	int running_switch = 1;
	sayHello();
	while(running_switch){
		printPrefix();
		running_switch = getInputCommand();
	}
}

int main(){
    /* 测试区 */
    store_promptiPrintAll();
    getprompt_wq();
    store_promptiPrintAll();
    return 0;
    /* 测试区 */

    shell();
    return 0;
}












