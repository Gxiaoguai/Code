
#define MAX_NAME_LEN 100

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
void store_promptiInit(){
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
    static char ret[MAX_NAME_LEN/2] = "";
    strcpy(ret ,"\"no exit such index\"");
    if(!strcmp("username", index))
        strcpy(ret, TubShellPrompt->username);
    if(!strcmp("hostname", index))
        strcpy(ret, TubShellPrompt->hostname);
    if(!strcmp("leaflist", index))
        strcpy(ret, TubShellPrompt->leaflist);
    if(!strcmp("root", index))
        strcpy(ret ,TubShellPrompt->root);
    return ret;
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





