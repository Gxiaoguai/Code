#ifndef STORE_H_INCLUDED
#define STORE_H_INCLUDED

/* ***** 共享的存储结构的操作 ***** */
/* Author: DJM*/
/* Funciton: 对Shell前缀的存储区的操作*/
extern void store_promptiInit();                      //段前缀初始化
extern char * store_promptGet(char * index);          //获取段前缀
extern void store_promptPut(int id, char * data);     //更改段前缀
extern void store_promptiPrintAll();                  //打印所有前缀信息

/* Author: DJM*/
/* Funciton: 对历史命令的存储区的操作*/
extern void store_commandInit();                       //历史命令区初始化
extern char * store_commandGet(int i);                 //获取最近命令
extern void store_commandPut(char * data);             //更新最近命令
extern void store_commandPrintAll();                   //打印历史命令信息

#include "store.c"


/* ********
*
*
*store_promptiInit()
*此函数为方便测试而存在
*给段前缀的用户名，主机名等暂时给予虚假值
*
*
*store_promptGet（char * index）
*形参index 为 username hostname pathname root的字符串下标
*eg:store_promptGet("username")从存储结构体获得username
*注意函数返回类型的接受
*
*
*store_promptPut(int id, char * data)
*更改段前缀,id 为 1 2 3 4 分别对应修改
*username ，hostname，pathname，root
*eg:store_promptPut(1, "xuzhiguang")
*eg:store_promptPut(4, "#")
*
*
*store_promptiPrintAll()
*打印所有前缀信息
*
*
******** */




#endif // STORE_H_INCLUDED


