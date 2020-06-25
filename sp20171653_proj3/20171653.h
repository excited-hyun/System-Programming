#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>

typedef struct ExternalTable{
	char symbol[100];
	int addr;
	int len;
	struct ExternalTable *next;
}est;

typedef struct SymbolTable{
	char symbol[100];
	int num;
	struct SymbolTable *next;
}symbol;

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



int Print_File(char *arr);		//type filename
void Reset_SymbolTable();		//symbl table reset해주는 함수	
int Assemble_File(char *arr);	//assemble filename
int Pass1(char*filename);		//location counter와 .obj파일의 한줄 길이 결정
int Pass2(char*filename);		//.lst, .obj파일 생성해 내용저장
void Make_SymbolTable(char *arr, int addr);		//assemble하면서 읽어들인 symbol을 table에 추가
symbol *Check_Duplicate(char *arr);		//symbol이 이미 symbol table에 저장된지 확인
hash* Find_Opcode(char *arr);			//opcode table에 해당하는 opcode존재여부 확인
void Print_SymbolTable();		//가장 최근에 assemble된 symbol table출력
int Format4(int opcode, int reg, int addr);		//format4의 objcode생성해 반환
int Format3(int opcode, int reg, int addr);		//format3의 objcode생성해 반환
int Format2(int opcode, char reg1, char reg2);	//format2의 objcode생성해 반환
void PasteSymbol(int sw);		//assemble에 실패하는경우에 대비해 symboltable을 복사해두는 함수



int Linking_Loader_Pass1(char *filename, int num);		//Load의 Pass1을 실행하는 함수
int Linking_Loader_Pass2(char *filename, int num);		//Load의 Pass2를 실행하는 함수
void Reset_ESTAB();					//external symbol table을 초기화 해주는 함수
int Make_ESTAB(char *symbol, int csaddr, int cslth, int num);	//external symbol table에 추가해주는 함수
est *Find_ESTAB(char *symbol);		//ESTAB에 이미 존재하면 그 노드를 반환하는 함수
void Print_ESTAB();					//ESTAB을 출력하는 함수
void Clear_BreakPoint();			//bp clear명령어 수행하는 함수 bp를 모두 지워줌
void Print_BreakPoint();			//sicsim에 존재하는 bp 모두 출력
void Set_BreakPoint(int num);		//입력된 bp추가
void Run();							//메모리에 load된 프로그램 실행
void Print_Register();				//실행 결과인 레지스터 값 출력
hash *Find_Opcode_By_Num(int op);	//opcode번호로 해쉬테이블에서 해당하는 노드 찾아서 반환
void Store_Reg(int addr, int value);	//symbol이 STR_인 경우 저장해줌 
