#ifndef STORE_H_INCLUDED
#define STORE_H_INCLUDED

/* ***** ����Ĵ洢�ṹ�Ĳ��� ***** */
/* Author: DJM*/
/* Funciton: ��Shellǰ׺�Ĵ洢���Ĳ���*/
extern void store_promptiInit();                      //��ǰ׺��ʼ��
extern char * store_promptGet(char * index);          //��ȡ��ǰ׺
extern void store_promptPut(int id, char * data);     //���Ķ�ǰ׺
extern void store_promptiPrintAll();                  //��ӡ����ǰ׺��Ϣ
#include "store.c"

#endif // STORE_H_INCLUDED

/* ********
*
*
*store_promptiInit()
*�˺���Ϊ������Զ�����
*����ǰ׺���û���������������ʱ�������ֵ
*
*
*store_promptGet��char * index��
*�β�index Ϊ username hostname leaflist root���ַ����±�
*eg:store_promptGet("username")�Ӵ洢�ṹ����username
*ע�⺯���������͵Ľ���
*
*
*store_promptPut(int id, char * data)
*���Ķ�ǰ׺,id Ϊ 1 2 3 4 �ֱ��Ӧ�޸�
*username ��hostname��leaflist��root
*eg:store_promptPut(1, "xuzhiguang")
*eg:store_promptPut(4, "#")
*
*
*store_promptiPrintAll()
*��ӡ����ǰ׺��Ϣ
*
*
******** */



