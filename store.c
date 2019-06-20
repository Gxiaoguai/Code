
#define MAX_NAME_LEN 100
#define MAX_CMD_LEN 100

/***** Info *****/
/* Author: DJM */
/* Function: 共享的Shell前缀*/
struct Prompt{
    char username[MAX_NAME_LEN/2];
    char hostname[MAX_NAME_LEN/2];
    char leaflist[MAX_NAME_LEN/2];
    char root[1];
    int num;
};
static struct Prompt ShellPrompt;
static struct Prompt *TubShellPrompt = &ShellPrompt;

/***** Info *****/
/* Author: DJM */
/* Function: 段前缀初始化*/
void store_promptInit(){
    /* Note: 此函数为方便测试而存在 */
    strcpy(TubShellPrompt->username, "root2019");
    strcpy(TubShellPrompt->hostname, "localhost");
    strcpy(TubShellPrompt->leaflist, "/");
    strcpy(TubShellPrompt->root, "0");
}

/***** Info *****/
/* Author: DJM */
/* Function: 获取段前缀*/
char * store_promptGet(char * index){
    /* Note: */
    /* **************
    * 形参index 为 username hostname leaflist root
    * 否则返回无此下标的提示
    ************** */
    static char ret1[MAX_NAME_LEN/2] = "";
    static char ret2[MAX_NAME_LEN/2] = "";
    static char ret3[MAX_NAME_LEN/2] = "";
    static char ret4[1] = "";
    //strcpy(ret ,"\"no exit such index\"");

    if(!strcmp("username", index)){
        strcpy(ret1, TubShellPrompt->username);
	return ret1;
    }
    if(!strcmp("hostname", index)){
        strcpy(ret2, TubShellPrompt->hostname);
	return ret2;
    }
    if(!strcmp("leaflist", index)){
        strcpy(ret3, TubShellPrompt->leaflist);
	return ret3;
    }
    if(!strcmp("root", index)){
        strcpy(ret4 ,TubShellPrompt->root);
    	return ret4;
    }
    return "\"no exit such index\"";
}

/***** Info *****/
/* Author: DJM */
/* Function: 更改段前缀 */
void store_promptPut(int id, char * data){
    /* Note: */
    /* **************
    * id 为 1 2 3 4 分别对应修改
    * username ，hostname，leaflist，root
    * data为要赋的串值
    ************** */
    switch(id){
        case 1:
            strcpy(TubShellPrompt->username, data);
            break;
        case 2:
            strcpy(TubShellPrompt->hostname, data);
            break;
        case 3:
            strcpy(TubShellPrompt->leaflist, data);
            break;
        case 4:
            strcpy(TubShellPrompt->root, data);
            break;
    }
}

/***** Info *****/
/* Author: DJM */
/* Function: 打印段前缀信息（方便测试） */
void store_promptiPrintAll(){
    printf("username is %s\n", TubShellPrompt->username);
    printf("hostname is %s\n", TubShellPrompt->hostname);
    printf("leaflist is %s\n", TubShellPrompt->leaflist);
    printf("root ? %s\n", TubShellPrompt->root);
}




/***** Info *****/
/* Author: DJM */
/* Function: 共享的历史命令 */
char commandHistory[100][MAX_CMD_LEN / 2];

/***** Info *****/
/* Author: DJM */
/* Function:  */
void store_commandInit(){
	int i;
	for(i = 0; i < 100; i ++)
		strcpy(commandHistory[i], "none");
}

/***** Info *****/
/* Author: DJM */
/* Function:  */
char * store_commandGet(int i){
	int j = 99;
	while(!strcmp(commandHistory[j], "none")){
		j--;
	}
	if((j - i + 1) >= 0)
		return commandHistory[j - i + 1];
	else
		return "error";
}

/***** Info *****/
/* Author: DJM */
/* Function: 每次都在数组后面插入最新的命令 */
void store_commandPut(char * data){
	int i = 0;	
	while(strcmp(commandHistory[i], "none")){
		i++;
	}
	strcpy(commandHistory[i], data);
}

/***** Info *****/
/* Author: DJM */
/* Function:  */
void store_commandPrintAll(){
	int i;
	for(i = 99; i >=0 ; i--){
		//if(!strcmp(commandHistory[i], "none")){
			printf("cmd %d is \'%s\'\n", i, commandHistory[i]);
		//}
	}
}




