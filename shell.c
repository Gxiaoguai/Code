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
//#include "DbLinkList.h"

/*** 自定义模块 ***/
#include "store.h"

#define MAX_NAME_LEN 100 
#define MAX_CMD_LEN 100 

/****** 函数声明 ******/
void sayHello();        //进入提示
void printPrefix();     //打印段前缀
int getInputCommand();  //获取输入命令
void getprompt_wq();	//获取用户信息
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

	getprompt_wq();		/* 先取得OS的前缀信息 */
	char * user, * host, * folder, * root;
	user = (char *)malloc(MAX_NAME_LEN / 2);
	host = (char *)malloc(MAX_NAME_LEN / 2);
	folder = (char *)malloc(MAX_NAME_LEN / 2);
	root = (char *)malloc(sizeof(char));

	user = store_promptGet("username");
	host = store_promptGet("hostname");
	folder = "***";
	root = store_promptGet("root");
	printf("[TUBShell %s@%s:%s]%s ",user, host, folder, root);
	//free(user);
	//free(host);
	//free(folder);
	//free(root);
}

/***** Info *****/
/* Author: DJM */
/* Function: 获取输入的命令,并决定调用*/
int getInputCommand(){
	/* Note: 注意命令输入的结束符号 */
	int i = 0, j = 0;		/* 用于for循环 */
	int __switch = 1;		/* shell退出开关 */
	char contentStr[100];		/* 存输入的数组 */
	char commandCompose[10][MAX_CMD_LEN / 2];	/* 分割后的命令组成多字符串数组 */
	char strtmp[MAX_CMD_LEN / 2];

	strcpy(contentStr, " ");	/* init */
	for(i = 0; i < 10; i++){
		strcpy(commandCompose[i], " ");
	}

	fgets(contentStr, 100, stdin);	/* input command */

	i = 0;			/* split str of command,result save in commandCompose */
	strcpy(strtmp, "");
	for(j = 0; j < strlen(contentStr); j++){
		if(contentStr[j] == ' ' && strlen(strtmp) == 0){
			continue;
		} else if (contentStr[j] == ' ' && strlen(strtmp) != 0){
			strcpy(commandCompose[i], strtmp);
			i++;
			strcpy(strtmp, "");
		}
		else if(contentStr[j] != ' '){
			sprintf(strtmp, "%s%c", strtmp, contentStr[j]);
			if((j + 2) == strlen(contentStr)){	//命令串即将结束
				strcpy(commandCompose[i], strtmp);
			}
		}
	}
	
	/* 判断命令调用 */
	if(!strcmp(commandCompose[0], "ls")){			/* 目录操作命令 */
		printf("ls command is done\n");
	} else if (!strcmp(commandCompose[0], "cd")){
		printf("cd command is done\n");
	} else if (!strcmp(commandCompose[0], "cat")){		/* 文件操作命令 */
		printf("cat command is done\n");
	} else if (!strcmp(commandCompose[0], "echo")){
		printf("echo command is done\n");
	} else if (!strcmp(commandCompose[0], "rename")){
		printf("rename command is done\n");
	} else if (!strcmp(commandCompose[0], "history")){	/* 其他命令 */
		printf("history command is done\n");
	} else if (!strcmp(commandCompose[0], "alias")){
		printf("alias command is done\n");
	} else if (!strcmp(commandCompose[0], "unalias")){
		printf("unalias command is done\n");
	} else if (!strcmp(commandCompose[0], "clear")){
		printf("clear command is done\n");
	} else if (!strcmp(commandCompose[0], "exit")){		/* 系统操作 */
		__switch = 0;
	} else if (!strcmp(commandCompose[0], "sleep")){
		printf("sleep command is done\n");
	} else if (!strcmp(commandCompose[0], "shutdown")){
		printf("shutdown command is done\n");
	} else if (!strcmp(commandCompose[0], "reboot")){
		printf("reboot command is done\n");
	} else {
		printf("command \'%s\' no exist\n", commandCompose[0]);
	}

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
		running_switch = getInputCommand();		//根据输入的命令执行相应的动作
	}
}

int main(){
    /* 测试区 */
    /*
    store_commandInit();
    store_commandPut("ls");
    store_commandPut("cd");
    store_commandPut("as");
    store_commandPut("vi");
    printf("the last cmd is %s\n", store_commandGet(1));
    printf("the last cmd is %s\n", store_commandGet(2));
    store_commandPrintAll();
    */
    //return 0;
    /* 测试区 */

    shell();
    return 0;
}












