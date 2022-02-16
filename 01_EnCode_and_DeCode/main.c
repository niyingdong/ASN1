#define _CRT_SECURE_NO_WARNINGS
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
        TypeName(const TypeName&); \
        void operator=(const TypeName&)
        
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "itcast_asn1_der.h"
#include "itcastderlog.h"

namespace {
struct Animal_t {
 public:
  unsigned char name[32]; //����
  ITCAST_UINT32 age;  //����
  ITCAST_UINT32 high; //���

  Animal_t(const unsigned char* name, ITCAST_UINT32 age, ITCAST_UINT32 high)
    :age(age), high(high) {
    memset(this->name, 0, sizeof(name));
    memcpy(this->name, name, strlen((char*)name) + 1);
  }
  Animal_t() = default;

 private:
   //��ֹ���������ɿ�����������ظ�ֵ�����
  DISALLOW_COPY_AND_ASSIGN(Animal_t);
  };
}
//����(��malloc)
ITCAST_INT EnCode(Animal_t* inData, ITCAST_ANYBUF** outAnyBuf, char** outData, ITCAST_INT* count);
//����
ITCAST_INT DeCode(ITCAST_ANYBUF** inData, Animal_t** outData);
//д�뵽�ļ�
ITCAST_INT writeToFile(const char* filename, const char* writeBuffer, ITCAST_INT length);


int main(int argc, char** argv) {

  //�������
  ITCAST_INT retval = -1;
  ITCAST_ANYBUF* head = NULL;
  Animal_t* animal = new Animal_t((const unsigned char*)"WangXiaoming", 18, 175);
  Animal_t* tmp_animal = NULL;
  char* tmp = NULL;
  ITCAST_INT count = -1;

  //����(��malloc)
  retval = EnCode(animal, &head, &tmp, &count);
  if (0 != retval) {
    printf("EnCode failed...\n");
    return -1;
  }

  //д���ļ�
  retval = writeToFile("./test", tmp, count);
  if (0 != retval) {
    printf("writeToFile failed...\n");
    return -2;
  }

  //����
  retval = DeCode(&head, &tmp_animal);
  if (0 != retval) {
    printf("DeCode failed...\n");
    return -3;
  }

  //�ж��Ƿ���ȷ
  if (!memcmp(animal->name, tmp_animal->name, strlen((char*)animal->name))) {
    printf("Successed...\n");
  }
  else {
    printf("unknow error...\n");
    return -4;
  }
  //�ͷ��ڴ�
  delete animal;
  delete tmp_animal;
  DER_ITCAST_FreeQueue(head);

  return 0;
}



//����
ITCAST_INT EnCode(Animal_t* inData, ITCAST_ANYBUF** outAnyBuf , char** outData, ITCAST_INT* count) {

  if (NULL == inData || NULL == outData || NULL == count) {
    printf("parameter is invalid...\n");
    return 1;
  }

  ITCAST_ANYBUF* tmp_anybuf = NULL;
  ITCAST_ANYBUF* head = NULL;
  ITCAST_ANYBUF* outDataNode = NULL;

  //name����
  //�ַ���ת����ANYBUF
  int retval = DER_ITCAST_String_To_AnyBuf(&tmp_anybuf, inData->name, strlen((char*)inData->name));
  if (0 != retval) {
    printf("String_to_AnyBuf failed...\n");
    return -5;
  }
  //ANYBUF����
  retval = DER_ItAsn1_WritePrintableString(tmp_anybuf, &head);
  if (0 != retval) {
    printf("String_to_AnyBuf failed...\n");
    return -12;
  }
  tmp_anybuf = head;

  //age����
  retval = DER_ItAsn1_WriteInteger(inData->age, &(tmp_anybuf->next));
  if (0 != retval) {
    printf("WriteInteger failed...\n");
    return -6;
  }
  tmp_anybuf = tmp_anybuf->next;

  //high����
  retval = DER_ItAsn1_WriteInteger(inData->high, &(tmp_anybuf->next));
  if (0 != retval) {
    printf("WriteInteger failed...\n");
    return -6;
  }

  //���б���(��ʱ�������ݶ��Ѿ�д��head��)
  retval = DER_ItAsn1_WriteSequence(head, &outDataNode);
  if (0 != retval) {
    printf("WriteStruct failed...\n");
    return -8;
  }

  //����
  *outData = (char*)malloc(outDataNode->dataLen + 1);
  memset(*outData, 0, outDataNode->dataLen + 1);
  memcpy(*outData, outDataNode->pData, outDataNode->dataLen+1);
  *count = outDataNode->dataLen;
  *outAnyBuf = outDataNode;

  //�ͷ��ڴ�
  retval = DER_ITCAST_FreeQueue(head);
  if (0 != retval) {
    printf("FreeQueue failed...\n");
    return -13;
  }

  return 0;
}
//����
ITCAST_INT DeCode(ITCAST_ANYBUF** inData, Animal_t** outData) {
  if (NULL == inData || NULL == outData) {
    printf("parameter is invalid...\n");
    return 12;
  }

  *outData = new Animal_t();
  Animal_t* animal = *outData;
  ITCAST_ANYBUF* head = NULL;
  ITCAST_ANYBUF* tmp_anybuf = NULL;

  //����ṹ��
  int retval = DER_ItAsn1_ReadSequence(*inData, &head);
  if (0 != retval) {
    printf("ReadStruct failed...\n");
    return -10;
  }
  tmp_anybuf = head;

  //����name
  char* buf = NULL;
  retval = DecodeChar(head, 
                      &buf, 
                      (int*)&head->dataLen);
  if (0 != retval) {
    printf("DecodeChar Name failed...\n");
    return -11;
  }
  memcpy(animal->name, buf, strlen(buf));
  
  //����age
  retval = DER_ItAsn1_ReadInteger(tmp_anybuf->next, &animal->age);
  if (0 != retval) {
    printf("ReadAge failed...\n");
    return -12;
  }
  tmp_anybuf = tmp_anybuf->next;

  //����high
  retval = DER_ItAsn1_ReadInteger(tmp_anybuf->next, &(*outData)->high);
  if (0 != retval) {
    printf("ReadHigh failed...\n");
    return -12;
  }

  printf("Name: %s || Age: %u || High: %u \n", animal->name, animal->age, animal->high);

  return 0;
}

//д�뵽�ļ�
ITCAST_INT writeToFile(const char* filename, const char* writeBuffer, ITCAST_INT length) {
  //���ļ�
  FILE* fd = fopen(filename, "w");
  if (fd < 0) {
    printf("fopen failed...\n");
    return -9;
  }
  //д������
  printf("------%s-----\n", writeBuffer);
  int retval = fwrite(writeBuffer, 1, length, fd);
  if (fd < 0) {
    printf("fwrite failed...\n");
    return -10;
  }

  //�ر��ļ�
  fclose(fd);
  return 0;
}

