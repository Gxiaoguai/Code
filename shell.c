/*********************
*Project: TUBShell
*Description: a private shell for OS command by C
*Author: DJM ZHJ WQ
*Create Time: 2019.6.18
**********************/

/****** 库调用 ******/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/stat.h>
//#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
//#include <grp.h>
//#include <libgen.h>
//#include "DbLinkList.h"

/****** 自定义模块 ******/
#include "store.h"

#define MAX_NAME_LEN 100 
#define MAX_CMD_LEN 100
#define MAX_PATH_LEN 100
#define MODE_RW_UGO (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

/****** 全局量 ******/
char *commandCompose[MAX_CMD_LEN] = {""};	/* 分割后的命令组成多字符串数组 */

/*char *commandCompose;
commandCompose[0] = (char *)malloc(sizeof(char) * MAX_CMD_LEN * 25);
int p;
for(p = 1; p < M; p ++){
    a[p] = a[p - 1] + n;
}*/

/****** 函数声明 ******/
void sayHello();        //进入提示
void printPrefix();     //打印段前缀 依赖getprompt_wq()
void pipe_zhj();		//管道通信
void getprompt_wq();	//获取用户信息
void cd_wq();			//cd 命令
void ls_djm();			//ls 命令
void touch_djm();		//touch 命令
void gedit_djm();		//gedit 命令
void commandStrSplit(char contentStr[]);	//对命令字符串分割
void commandStrLinkAndSplit();				//对commandCompose[]连接重分割
int commandJudge();						//对命令判断是否为特殊cmd(& | < >)
int commandControl();						//对命令判断应调用什么函数完成
char *alias(char commanddStr[]);			//别名替换
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

	getprompt_wq();		/* 先取得OS的前缀信息 */
	char * user, * host, * folder, * root;
	user = (char *)malloc(MAX_NAME_LEN / 2);
	host = (char *)malloc(MAX_NAME_LEN / 2);
	folder = (char *)malloc(MAX_NAME_LEN / 2);
	root = (char *)malloc(sizeof(char));

	user = store_promptGet("username");
	host = store_promptGet("hostname");
	folder = store_promptGet("pathname");
	root = store_promptGet("root");
	printf("[TUBShell %s@%s:%s]%s ",user, host, folder, root);
	//free(user);
	//free(host);
	//free(folder);
	//free(root);
}

/***** Info *****/
/* Author: ZHJ */
/* Function: 实现管道*/
void pipe_zhj(){}

/***** Info *****/
/* Author: WQ */
/* Function: 从OS获知部分前缀信息并放入存储区 */
void getprompt_wq(){
    /* Note: 获取用户名，主机，当前目录，超级用户确认*/

    /* 获取用户名 */
	struct passwd *my_info;
	int uid = getuid();         //返回用户id
	my_info = getpwuid(uid);    //通过用户的uid查找用户的passwd数据
	store_promptPut(1, my_info->pw_name);

    /* 获取主机名 */
	char hostname[MAX_NAME_LEN];
	gethostname(hostname, sizeof(hostname));
	store_promptPut(2, hostname);
	
	/* 获取当前目录 */
	char pathname[MAX_PATH_LEN];
	getcwd(pathname, MAX_PATH_LEN);
	if (strncmp(pathname, my_info->pw_dir, strlen(my_info->pw_dir)) == 0){//比较两条路径
		sprintf(pathname, "%s%s", "~", &pathname[strlen(my_info->pw_dir)]);
		store_promptPut(3, pathname);
	} else{
		store_promptPut(3, pathname);
	}
	
    /* 确认是否为超级用户 */
	if (uid == 0) {
		store_promptPut(4, "#");
	}
	else {
		store_promptPut(4, "$");
	}
}

/***** Info *****/
/* Author: WQ */
/* Function: cd功能 */
void cd_wq(){
	/* Note:放在能调用存储命令数组commandCompose[]同一文件里*/
	struct passwd *pwd;
	char pathname[MAX_PATH_LEN];			//储存路径名
	pwd = getpwuid(getuid());				//获取用户信息 
	if(strcmp(commandCompose[1]," ") == 0){	//如果第二个字符串为空，则代表进入根目录 
		strcpy(pathname, pwd->pw_dir);		//获取pathname   pwd->pw_dir获取的目录都是/root 
		if(chdir(pathname) == -1){			//如果有错
			perror("myshell: chdir");		//报错
			exit(1);
		}
	} else {								//如果有路径
		if(chdir(commandCompose[1]) == -1){	//如果chdir执行失败
			printf("TUBshell: cd: %s :No such file or directory\n", commandCompose[1]);	
		}	
	}
}

/***** Info *****/
/* Author: DJM */
/* Function: ls功能 */
void ls_djm(){
	//权限 硬链接数 拥有者 所属组 文件大小 创建时间 文件名
	/* struct stat TUBstat;
	if(lstat(filename, &TUBstat) == -1){
		printf("lstat error");
		exit(1);
	}
	char permission[15] = {" "};
	char type;
	type = judgeFileType(&TUBstat);	
	char *bname = basename(filename); */
	
	DIR * dir;	/* struct pointer */
	struct dirent * rent;
	char strtmp[100] = {" "};
	int i = 0;
	dir = opendir(".");	/* 打开当前目录 */
	while((rent = readdir(dir))){
		strcpy(strtmp, rent->d_name);
		if(strtmp[0] == '.' || strtmp[0] == ' ')
			continue;
		printf("%s  ", strtmp);
		//if(!((++i) % 6))
			//printf("\n");
	}
	printf("\n");
	closedir(dir);
}

/***** Info *****/
/* Author: DJM */
/* Function: touch功能 */
void touch_djm(char filename[]){
	int fd = -1;
	fd = open(filename, O_WRONLY|O_CREAT,MODE_RW_UGO);
	//write(fd, s, sizeof(s));
	if(fd == -1)
		printf("create file error\n");
	close(fd);
}

/***** Info *****/
/* Author: DJM */
/* Function: gedit功能 */
void gedit_djm(char filename[]){
	if(fork() == 0){
		if(execlp("gedit", "gedit", filename, NULL) == -1){
			perror("execl \'gedit\' error\n");
			exit(1);
		}
	}
	sleep(1);
}

/***** Info *****/
/* Author: DJM */
/* Function: 对命令字符串进行分割放入commandCompose */
void commandStrSplit(char * contentStr){
	/* Note: split str of command,result save in commandCompose */
	int i = 0, j = 0;
	int k = 0, m = 0;
	char strtmp[25][25] = {""};
	int len = 0;	/* contentStr紧凑串(没有空格)的长度 */
	
	for(j = 0; j < strlen(contentStr); j++){
		if(contentStr[j] == ' ' && strlen(strtmp[k]) == 0){
			continue;
		} else if (contentStr[j] == ' ' && strlen(strtmp[k]) != 0){
			commandCompose[i++] = strtmp[k++];
		} else if(contentStr[j] == '|' || contentStr[j] == '>' || contentStr[j] == '<'){
			if(strlen(strtmp[k]) != 0){
				commandCompose[i++] = strtmp[k++];
			}
			strtmp[k][0] = contentStr[j];
			commandCompose[i++] = strtmp[k++];
		} else if(contentStr[j] != ' '){
			for(m = 0; strtmp[k][m]; m++){}
			strtmp[k][m] = contentStr[j];
			
			if((j + 2) >= strlen(contentStr)){
				commandCompose[i++] = strtmp[k++];
				break;
			}
		}
		
		if(contentStr[j] != ' ' || contentStr[j] != '\n')
			len++;
	}
	commandCompose[i] = NULL;
	
	/* 打印测试 */
	/*
	printf("contentStr len is %ld\n",strlen(contentStr));
	printf("total len is %d\n",len);
	int sum = 0;
	for(i = 0; sum < len; i++){
		printf("第%d个分割 *%s*\n", i, commandCompose[i]);
		printf("len *%ld*\n", strlen(commandCompose[i]));
		sum += strlen(commandCompose[i]);
	}
	*/
}

/***** Info *****/
/* 弃用 */
/* Author: DJM */
/* Function: 对commandCompose[]进行连接重新分割 */
void commandStrLinkAndSplit(){
	int i = 0;
	char strtmp[MAX_CMD_LEN] = {""};
	while(strcmp(commandCompose[i], " ")){
		strcat(strtmp, commandCompose[i]);
		strcat(strtmp, " ");
		i++;
	}
	commandStrSplit(strtmp);
}

/***** Info *****/
/* Author: DJM */
/* Function: 命令判断(特殊,普通) */
int commandJudge(){
	/* Note: & < > | */
	/* 普通命令返回 0 */
	/* 上述特殊命令分别返回 1(|) 2(< >) 3(&)*/
	int __switch = 0;
	int i, j;
	for(i = 0; commandCompose[i]; i++){
		for(j = 0; commandCompose[i][j]; j++){
			switch(commandCompose[i][j]){
				case '|': __switch = 1; break;
				case '<': ;
				case '>': __switch = 2; break;
				case '&': __switch = 3; break;
				default: __switch = 0;
			}
			if(__switch != 0){
				//printf("special cmd\n");
				break;
			}
		}
		if(__switch != 0){
			break;
		}
	}
	if(__switch == 0){
		//printf("common command\n");
	}
	return __switch;
}

/***** Info *****/
/* Author: DJM */
/* Function: 命令控制器 */
int commandControl(int __switch){
	/* Note: __switch 为 0/1/2/3 分别代表 常规/管道/重定向/后台控制 命令 */
	char cmd[MAX_CMD_LEN];
	strcpy(cmd ,commandCompose[0]);
	
	/* 判断命令调用 */
	switch(__switch){
		case 0:
			if(fork() == 0){
				//if((execlp("ls", "ls", NULL)) == -1){
				if((execvp(cmd, commandCompose)) == -1){
					perror("execvp error\n");
					exit(1);
				}
			}
			sleep(1);
			break;
		case 1: ;
		case 2: ;
		case 3: 
			printf("command %s is done\n", cmd);
			break;
		default: printf("command \'%s\' no exist\n", cmd);;
	}
	
	return __switch;
	
	/* 保留日后用 */
	/* ls_djm() cd_wq() touch_djm() gedit_djm() */
	/* ls cd touch gedit */
	/* cat echo rename history alias unalias clear exit sleep shutdown reboot  */
}

/***** Info *****/
/* Author:  */
/* Function: 别名替换 */
char *alias(char commanddStr[]){
	return commanddStr;
}

/***** Info *****/
/* Author: DJM */
/* Function: 获取输入的命令,并决定调用*/
int getInputCommand(){
	/* Note: 注意命令输入的结束符号 */
	int i = 0, j = 0;			/* 用于for循环 */
	int __switch = 1;			/* 判断控制调用开关 和 shell退出开关 */
	char contentStr[100];		/* 存输入的数组 */
	strcpy(contentStr, " ");	/* init */

	fgets(contentStr, 100, stdin);	/* input command */

	/* 命令字符串 ->重命名->分割->调用 */
	char resultAlias[100];
	strcpy(resultAlias, alias(contentStr));
	commandStrSplit(resultAlias);		/* split result put in global array '*commandCompose[]' */
	
	__switch = commandJudge();
	__switch = commandControl(__switch);	/* exec command */
	return (__switch+1);
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












