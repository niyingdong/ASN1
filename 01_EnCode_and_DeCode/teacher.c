#include <stdio.h>
#include <string.h>
#include <iostream>
#include "itcast_asn1_der.h"
#include "itcastderlog.h"
#include "teacher.h"
int encodeTeacher(Teacher* p, char** outdata, int* outlen)
{
	ITCAST_ANYBUF* head = NULL;
	ITCAST_ANYBUF* temp = NULL;
	ITCAST_ANYBUF* next = NULL;
	//编码name
	DER_ITCAST_String_To_AnyBuf(&temp,p->name,strlen(p->name)+1);
	DER_ItAsn1_WritePrintableString(temp,&head);
	DER_ITCAST_FreeQueue(temp);//数据已导入链表，释放第一个函数开辟的内存空间
	next = head;
	//编码age
	DER_ItAsn1_WriteInteger(p->age,&head->next);
	next = next->next;
	//编码p
	EncodeChar(p->p,stlen(p->p)+1,&next->next);
	next=next->next;
	//编码plen
	DER_ItAsn1_WriteInteger(p->plen, &next->next);
	//序列化
	DER_ItAsn1_WriteSequence(head,&temp);//将链表变成一个ANYBUFF类型数据
	//输出参数赋值
	*outdata = temp->pData;
	*outlen = temp->dataLen;

	DER_ITCAST_FreeQueue(head);
	return 0;
}
int decodeTeacher(char* indata, int inlen, Teacher** p)
{
	ITCAST_ANYBUF* head = NULL;
	ITCAST_ANYBUF* temp = NULL;
	ITCAST_ANYBUF* next = NULL;
	Teacher* pt= (Teacher*)malloc(sizeof(Teacher));
	//将indata反序列化成链表
	DER_ITCAST_String_To_AnyBuf(&temp,indata,inlen);//将char*-->ITCAST_ANYBUF
	DER_ItAsn1_ReadSequence(temp,&head);
	DER_ITCAST_FreeQueue(temp);//数据已导入链表，释放第一个函数开辟的内存空间
	//解码name
	DER_ItAsn1_ReadPrintableString(head,&temp);
	memcpy(pt->name, temp->pData, temp->dataLen);
	DER_ITCAST_FreeQueue(temp);
	next = next->next;
	//解码age
	DER_ItAsn1_ReadInteger(next,&(pt->age));
	next = next->next;
	//解码p
	int plen = 0;
	DecodeChar(next,&pt->p,&plen);
	next = next->next;
	//解码plen
	DER_ItAsn1_ReadInteger(next, &(pt->plen));
	//输出参数赋值
	*p = pt;

	DER_ITCAST_FreeQueue(head);
	return 0;
}
void freetea(Teacher** p)
{
	if (*p != NULL)
	{
		if ((*p)->p != NULL)
		{
			free((*p)->p);
		}
		free(*p);
	}
}
int main()
{
	Teacher tea;
	memset(&tea,0x00,sizeof(tea));
	strcpy(tea.name,"路飞");
	tea.age = 18;
	tea.p = (char*)malloc(100);
	strcpy(tea.p,"I'm quee!!!");
	tea.plen = strlen(tea.p);
	
	char* outdata;
	int outlen;
	encodeTeacher(&tea,&outdata,&outlen);

	Teacher* pt;
	decodeTeacher(outdata,outlen,&pt);
	printf("%s,%d,%s,%d\n", pt->name, pt->age, pt->p, pt->plen);

	freetea(&pt);
	return 0;
}