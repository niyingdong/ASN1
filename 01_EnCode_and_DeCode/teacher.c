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
	//����name
	DER_ITCAST_String_To_AnyBuf(&temp,p->name,strlen(p->name)+1);
	DER_ItAsn1_WritePrintableString(temp,&head);
	DER_ITCAST_FreeQueue(temp);//�����ѵ��������ͷŵ�һ���������ٵ��ڴ�ռ�
	next = head;
	//����age
	DER_ItAsn1_WriteInteger(p->age,&head->next);
	next = next->next;
	//����p
	EncodeChar(p->p,stlen(p->p)+1,&next->next);
	next=next->next;
	//����plen
	DER_ItAsn1_WriteInteger(p->plen, &next->next);
	//���л�
	DER_ItAsn1_WriteSequence(head,&temp);//��������һ��ANYBUFF��������
	//���������ֵ
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
	//��indata�����л�������
	DER_ITCAST_String_To_AnyBuf(&temp,indata,inlen);//��char*-->ITCAST_ANYBUF
	DER_ItAsn1_ReadSequence(temp,&head);
	DER_ITCAST_FreeQueue(temp);//�����ѵ��������ͷŵ�һ���������ٵ��ڴ�ռ�
	//����name
	DER_ItAsn1_ReadPrintableString(head,&temp);
	memcpy(pt->name, temp->pData, temp->dataLen);
	DER_ITCAST_FreeQueue(temp);
	next = next->next;
	//����age
	DER_ItAsn1_ReadInteger(next,&(pt->age));
	next = next->next;
	//����p
	int plen = 0;
	DecodeChar(next,&pt->p,&plen);
	next = next->next;
	//����plen
	DER_ItAsn1_ReadInteger(next, &(pt->plen));
	//���������ֵ
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
	strcpy(tea.name,"·��");
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