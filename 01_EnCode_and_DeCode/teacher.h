#ifndef _TEACHER_H_
#define _TEACHER_H_
typedef struct _Teacher
{
	char name[64];
	int age;
	char* p;
	long plen;
}Teacher;
int encodeTeacher(Teacher* p,char** outdata,int* outlen);
int decodeTeacher(char* indata,int inlen,Teacher** p);
void freetea(Teacher** p);


#endif