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

/****** 函数声明 ******/
void sayHello();        					//进入提示
void printPrefix();     					//打印段前缀 依赖getprompt_wq()
void getprompt_wq();						//获取用户信息
int cd_wq(char op_path[2][MAX_CMD_LEN/2]);	//cd 命令
int ls_djm();								//ls 命令
int touch_djm(char filename[]);				//touch 命令
int gedit_djm(char filename[]);				//gedit 命令
int commonCmd_beforeExec_search();			//exec外部调用之前，先查找我们做的命令实现
int commonCmd_djm();						//常规命令
int pipeCmd_zhj();							// | 管道命令
int redirectCmd_djm();						//< > 重定向命令
int backstageCmd_djm();						//& 后台处理命令
void commandStrSplit(char contentStr[]);	//对命令字符串分割
void commandStrLinkAndSplit();				//对commandCompose[]连接重分割
int commandJudge();							//对命令判断是否为特殊cmd(& | < >)
int commandControl();						//对命令判断应调用什么函数完成
int getInputCommand();  					//获取输入命令
void shell();           					//shell的总入口

/****** 从store.c引入 ******/
void aliasInsert_zhj();						//插入alias存储区
void printAlia_zhj();						//打印alias结果
char * checkAlias_zhj(char *findstr);		//检查alias存储区
char * judgeAlias_zhj(char *p);				//判断有无别名（需要checkAlias_zhj()）
int alias_zhj();							//alias命令
int unalias_zhj();							//unalias命令

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
	printf("[\e[36;1mTUBShell\e[0m \e[32;1m%s@%s\e[0m:\e[34;1m%s\e[0m]%s ",user, host, folder, root);
	//free(user);
	//free(host);
	//free(folder);
	//free(root);
}

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
int cd_wq(char op_path[2][MAX_CMD_LEN/2]){
	/* Note:放在能调用存储命令数组commandCompose[]同一文件里*/
	/* 调用成功返回1，失败返回-1 */
	int ret = 1;
	struct passwd *pwd;
	char pathname[MAX_PATH_LEN];					//储存路径名
	pwd = getpwuid(getuid());						//获取用户信息 
	
	if(strcmp(op_path[1],"") == 0){					//如果第二个字符串为空，则代表进入根目录 
		strcpy(pathname, pwd->pw_dir);				//获取pathname   pwd->pw_dir获取的目录都是/root
		if(ret=(chdir(pathname)) == -1){			//如果有错
			ret = -1;
			perror("TUBshell: chdir");				//报错
			exit(1);
		}
	} else {										//如果有路径
		if(ret=(chdir(op_path[1])) == -1){			//如果chdir执行失败
			ret = -1;
			printf("TUBshell: cd: %s :No such file or directory\n", commandCompose[1]);
			exit(1);
		}	
	}
	
	if(!ret)			//ret = 0; chdir() success
		ret = 1; 
	return ret;
}

/***** Info *****/
/* Author: DJM */
/* Function: ls功能 */
int ls_djm(){
	//-l(未自实现) 权限 硬链接数 拥有者 所属组 文件大小 创建时间 文件名
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
	return 1;
}

/***** Info *****/
/* Author: DJM */
/* Function: touch功能 */
int touch_djm(char filename[]){
	int fd = -1;
	fd = open(filename, O_WRONLY|O_CREAT,MODE_RW_UGO);
	//write(fd, s, sizeof(s));
	if(fd == -1)
		printf("create file error\n");
	close(fd);
	return 1;
}

/***** Info *****/
/* Author: DJM */
/* Function: gedit功能 */
int gedit_djm(char filename[]){
	if(fork() == 0){
		if(execlp("gedit", "gedit", filename, NULL) == -1){
			perror("execl \'gedit\' error\n");
			exit(1);
		}
	}
	sleep(1);
	return 1;
}

/***** Info *****/
/* Author: DJM */
/* Function: 查找我们做的命令实现 */
int commonCmd_beforeExec_search(){
	/* Note: 在exec外部调用前查找，看有无我们自己实现的内建命令 */
	int ret = -1;
	char str_Op_Path[2][MAX_CMD_LEN / 2];
	char cmd[MAX_CMD_LEN / 2];
	strcpy(cmd, commandCompose[0]);
	strcpy(str_Op_Path[0], commandCompose[0]);
	if(!strcmp(commandCompose[0], "cd")){
		if(commandCompose[1] == NULL){
			//printf("cd folow is none\n");
		} else {
			strcpy(str_Op_Path[1], commandCompose[1]);
		}
	} 
	
	ret = (strcmp(cmd, "cd")) ? ret : cd_wq(str_Op_Path);
	ret = (strcmp(cmd, "alias")) ? ret : alias_zhj();
	ret = (strcmp(cmd, "unalias")) ? ret : unalias_zhj();
	//ret = (strcmp(cmd, "touch")) ? ret : touch_djm("none");
	//ret = (strcmp(cmd, "gedit")) ? ret : gedit_djm("none");
	//ret = (strcmp(cmd, "ls")) ? ret : gedit_djm("none");
	
	/* 保留日后用 */
	/* ls_djm() cd_wq() touch_djm() gedit_djm() */
	/* ls cd touch gedit */
	/* cat echo rename history alias unalias clear exit sleep shutdown reboot  */
	return ret;
}

/***** Info *****/
/* Author: DJM */
/* Function: 实现常规命令 */
int commonCmd_djm(){
	int __switch = 0;		/* 常规命令标识为0的返回 */
	int funcRet = -1;		/* 用于函数内部获取调用函数返回值 1/-1 success/failure */
	char cmd[MAX_CMD_LEN];
	strcpy(cmd ,commandCompose[0]);

	funcRet = commonCmd_beforeExec_search();
	
	if(funcRet == 1){			//查询成功
		//printf("cmd is exist in our program and done\n");
	} else {					//查询失败
		if(fork() == 0){
			if((__switch = execvp(cmd, commandCompose)) == -1){
				//perror("execvp error\n");
				printf("Command \'%s\' not found\n", cmd);
				exit(1);
			}
		}
	}
	sleep(1);
	return __switch;
};

/***** Info *****/
/* Author: ZHJ */
/* Function: 实现管道命令 */
int pipeCmd_zhj(){}

/***** Info *****/
/* Author: DJM */
/* Function: 实现重定向命令 */
int redirectCmd_djm(){}

/***** Info *****/
/* Author: DJM */
/* Function: 实现后台处理命令 */
int backstageCmd_djm(){
	int i= -1;
	int pid;
	int __switch = 3;
	char cmd[MAX_CMD_LEN];
	strcpy(cmd ,commandCompose[0]);
	
	while(strcmp(commandCompose[++i], "&"));
	commandCompose[i] = NULL;
	
	if((pid = fork()) == 0){
		sleep(1);
		printf("\n");
		if((execvp(cmd, commandCompose)) == -1){
			perror("execvp error\n");
			exit(1);
		}
	}
	printf("Pid [\e[32;1m%d\e[0m]\n", pid);
	return __switch;
}

/***** Info *****/
/* Author: DJM */
/* Function: 对命令字符串进行分割放入commandCompose */
void commandStrSplit(char * contentStr){
	/* Note: split str of command,result save in commandCompose */
	int i = 0, j = 0;
	int k = 0, m = 0;
	char strtmp[25][25] = {""};
	int switch_keepStr = 0;	/* 遇到左单引号，开开关(=1)，遇右单引号，关开关(=0) */	
	int len = 0;			/* contentStr紧凑串(没有空格，换行)的长度 */
	
	for(j = 0; j < strlen(contentStr); j++){
		if(contentStr[j] == ' ' && strlen(strtmp[k]) == 0){
			continue;
		} else if (contentStr[j] == ' ' && strlen(strtmp[k]) != 0){
			if(switch_keepStr == 0){
				// no ' control
				commandCompose[i++] = strtmp[k++];
			} else {
				// have ' control
				for(m = 0; strtmp[k][m]; m++){}
				strtmp[k][m] = contentStr[j];
			}
		} else if(contentStr[j]=='|'||contentStr[j]=='>'||contentStr[j]=='<'||contentStr[j]=='&'){
			if(strlen(strtmp[k]) != 0){
				commandCompose[i++] = strtmp[k++];
			}
			strtmp[k][0] = contentStr[j];
			commandCompose[i++] = strtmp[k++];
		} else if(contentStr[j] != ' ' && contentStr[j] != '\n' ){
			// set ' control
			if(contentStr[j] == '\''){
				if(switch_keepStr == 0){		//首次遇到'，开
					switch_keepStr = 1;
				} else {						//再次遇到'，关
					switch_keepStr = 0;
				}
			}
			
			for(m = 0; strtmp[k][m]; m++){}
			strtmp[k][m] = contentStr[j];
			
			if((j + 1) >= strlen(contentStr)){
				commandCompose[i++] = strtmp[k++];
				break;
			}
		}
		if(contentStr[j] != ' ' && contentStr[j] != '\n' && contentStr[j] != '\0')
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
			__switch = commonCmd_djm();
			break;
		case 1:
			printf("command %s is done\n", cmd);
			break;
		case 2:
			printf("command %s is done\n", cmd);
			break;
		case 3: 
			__switch = backstageCmd_djm();
			break;
		default: printf("command \'%s\' no exist\n", cmd);;
	}
	
	return __switch;
}

/***** Info *****/
/* Author:  */
/* Function: 别名替换 */
/*
char *alias_zhj(char commanddStr[]){
	return commanddStr;
}
*/

/***** Info *****/
/* Author: DJM */
/* Function: 获取输入的命令,并决定调用*/
int getInputCommand(){
	/* Note: 注意命令输入的结束符号 */
	int i = 0, j = 0;				/* 用于for循环 */
	int __switch = 1;				/* 判断控制调用开关 和 shell退出开关 */
	char contentStr[100];			/* 存输入的数组 */
	strcpy(contentStr, " ");		/* init */

	fgets(contentStr, 100, stdin);	/* input command */
	
	/* 判断是否为空回车 fgets()默认带'\0' */
	if(strlen(contentStr) == 1){
		//printf("this is enter\n");
		return 1;
	}
	
	//printf("before contentStr is *%s*\n", contentStr);
	
	/* fgets()默认带'\n',将其替换为\0 */
	while(contentStr[i] != '\n'){
		i++;
	}
	contentStr[i] = '\0';
	
	//printf("after contentStr is *%s*\n", contentStr);
	
	//return 0;
	
	/* 命令字符串 ->重命名->分割->调用 */
	char resultAlias[MAX_CMD_LEN];
	
	//测试通过
	//commandCompose[0] = "alias";
	//commandCompose[1] = "djm='ls -l'";
	//aliasInsert_zhj();
	//printAlia_zhj();
	
	strcpy(resultAlias, judgeAlias_zhj(contentStr));
	//printf("alias result is *%s*\n", resultAlias);
	
	commandStrSplit(resultAlias);		/* split result put in global array '*commandCompose[]' */
	
	__switch = commandJudge();
	__switch = commandControl(__switch);	/* exec command */
	
	/* 因四种命令分别为 0 1 2 3 ，故+1使得继续运行TUBShell */
	return (__switch + 1);
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

/* **********:**********:**********: 共享的alias存储结构体及操作 **********:**********:**********: */
struct Record
{
    char word[MAX_CMD_LEN];
    char mean[MAX_CMD_LEN];
};
struct Node
{
    struct Record data;
    struct Node *next;
};
static struct Node dictionary;
static struct Node* head = &dictionary;
static struct Node* currentRecord = &dictionary;

/***** Info *****/
/* Author: ZHJ */
/* Function: alias存储结构体插入 */
void aliasInsert_zhj(){
	//准备做插入操作(带检测)
    struct Node *temp = (struct Node*)malloc(sizeof(struct Node));
    char str[MAX_CMD_LEN / 2];
    strcpy(str, commandCompose[1]);
	
	//插入
    char *tmp = "";
    tmp = strtok(str, "=");
    strcpy((temp->data).word, tmp);
    
    /* 检测重名 */
	char checkResult[MAX_CMD_LEN] = "";
	strcpy(checkResult, checkAlias_zhj(str));		//进别名 出命令
	if(strcmp(checkResult, str) != 0){
		//有重名
		struct Node* p = head;
    	p = p->next;
		while(p != NULL){
		    if(strcmp(str, (p->data).word) == 0){
		        tmp = strtok(NULL, "'");
		        strcpy((p->data).mean, tmp);
		        break;
		    }
		    p = p->next;
		}
        return;
    }
    /* 检测完毕 */
    
    //无别名
    tmp = strtok(NULL, "'");
    strcpy((temp->data).mean, tmp);
    
    currentRecord->next = temp;
    temp->next = NULL;
    currentRecord = currentRecord->next;
    return;
}

/***** Info *****/
/* Author: ZHJ */
/* Function: 打印alias存储结构体所有结果 */
void printAlia_zhj(){
	struct Node* p = head;
	p = p->next;
	while(p != NULL){
		printf("alias: %s=\'%s\'\n", (p->data).word, (p->data).mean);
		p = p->next;
	}
}

/***** Info *****/
/* Author: ZHJ */
/* Function: 遍历alias存储结构体 */
char * checkAlias_zhj(char *findstr){
    struct Node* p = head;
    p = p->next;
    char exam[MAX_CMD_LEN / 2 + 1];
    static char retStr[MAX_CMD_LEN] = "";
    strcpy(retStr, findstr);
    strcpy(exam, findstr);
    
    while(p != NULL){
        if(strcmp(exam, (p->data).word) == 0){
            strcpy(retStr, (p->data).mean);
            break;
        }
        p = p->next;
    }
    return retStr;
}

/***** Info *****/
/* Author: ZHJ */
/* Function: 判断输入有无别名，有则替换 */
char * judgeAlias_zhj(char *cmdStr){
	/* Note: 判断输入的命令行中有无别名， */
	/* 有则替换，没有则返回原串 */
	/* 调用checkAlias_zhj()遍历alias存储结构体 */
	static char resultStr[MAX_CMD_LEN];
	strcpy(resultStr, "");
	char *target;
	char *tmp;
	char checkResult[MAX_CMD_LEN] = "";

	target = strtok_r(cmdStr, " ", &tmp);
	strcpy(checkResult, checkAlias_zhj(target));		/* search and put in checkResult */
    if(strlen(checkResult) != 0){
        strcpy(resultStr, checkResult);
	}
	strcat(resultStr, " ");
	strcat(resultStr, tmp);
	return resultStr;
}

/***** Info *****/
/* Author: ZHJ */
/* Function: 实现alias命令 */
int alias_zhj(){
	if(commandCompose[1] == NULL){
		printAlia_zhj();                	//print all the relations
	} else {
		aliasInsert_zhj(currentRecord);		//insert the relation
	}
	return 1;
}

/***** Info *****/
/* Author: ZHJ */
/* Function: 实现unalias命令 */
int unalias_zhj(){
    char checkResult[MAX_CMD_LEN] = "";
    strcpy(checkResult, checkAlias_zhj(commandCompose[1]));

    if(strcmp(checkResult, commandCompose[1]) == 0){
        printf("[unalias] \'%s\':not found", commandCompose[1]);
        return 1;
    }

	char deletestr[MAX_CMD_LEN / 2 + 1];
	strcpy(deletestr, commandCompose[1]);
    struct Node *p = head;
    struct Node *tmp;
    int i = 0;
    while((p->next) != NULL){
        if(strcmp(deletestr, ((p->next)->data).word) == 0){
            tmp = p->next;
            p->next = tmp->next;
           	tmp->next = NULL;
            free(tmp);
            break;
        }
        p = p->next;
        i++;
    }
    return 1;
}













