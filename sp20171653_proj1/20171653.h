#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>

typedef struct HashTable{
	int opcode;
	char command[100];
	char format[100];
	struct HashTable *next;
}hash;

typedef struct History{
	char command[100];
	struct History *next;
}history;


void Get_Command(); //명령어 입력받는 함수


void Read_Opcode(); //opcode.txt읽어오는 함수
int Hash_Function(char *arr); //hash값 생성
int Print_Opcode(char *arr); //mnemonic에 해당하는 opcode 출력
void Opcode_List(); //opcode hash table 출력


void Print_All_Command(); //h[elp]
void Current_Directory_File(); //d[ir]
void Quit_Sicsim(); //q[uit]
void View_History(); //hi[story]
void Push_Command(char *arr); //history를 링크드리스트 형태로 저장


void dump(int start, int end); //du[mp]
void Edit(int addr, int value); //e[dit]
void Fill(int start, int end, int value); //f[ill]
void Reset(); //reset
