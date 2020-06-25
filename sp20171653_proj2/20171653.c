#include "20171653.h"

unsigned char memory[16][65536];

history *head;
hash *hashhead[20];
symbol *symbolhead[26];
symbol *symbolkeep[26];
int obj_len[100]={0};

int dumpend=-1;
int dumpstart;
int program_len;
int base_addr;


/*
   main함수*/
int main(void){
	Read_Opcode();
	while(1){
		Get_Command();

	}
}


/*
	Print_All_Command()
	h[elp]
	shell에서 실행가능한 모든 명령어 리스트 출력
*/			   
void Print_All_Command(){
	printf("h[elp]\n");				
	printf("d[ir]\n");					
	printf("q[uit]\n");							
	printf("hi[story]\n");								
	printf("du[mp] [start, end]\n");								
	printf("e[dit] address, value\n");				
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");

	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
}


/*
	Current_Directory_File()
    d[ir]
	현재 디렉터리의 파일 출력
*/
void Current_Directory_File(){
	
	DIR *directory=opendir("./");
	struct dirent *di;
	struct stat statbuf;

	if(directory !=NULL){
		while((di = readdir(directory))!=NULL){
			lstat(di->d_name,&statbuf);
			if(S_ISREG(statbuf.st_mode))
				printf("%s*\t", di->d_name);
			else if(S_ISDIR(statbuf.st_mode)&&S_IXUSR)
				printf("%s/\t", di->d_name);
		}
		printf("\n");
	}
	closedir(directory);

}


/*
	Quit_Sicsim()
    q[uit]
	링크드리스트로 구현된 history와 opcode hash table을 모두 free하여 삭제해준 뒤 sicsim 종료
*/
void Quit_Sicsim(){

	history *del;
	hash *hashdel;

	while(head != NULL){
		del = head;
		head = head->next;
		free(del);
	}
	for(int i=0; i<20; i++){
		while(hashhead[i]!=NULL){
			hashdel = hashhead[i];
			hashhead[i] = hashhead[i]->next;
			free(hashdel);
		}
	}

	exit(0);
}



/*
	View_History()
    hi[story]
	링크드르스트로 구현된 history를 head부터 따라가면서 사용한 명령어 순서대로 번호와 함께 보여줌
*/
void View_History(){

	int num=1;
	history *temp = head;
	while(temp!=NULL){
		printf("%d %s\n", num, temp->command);
		temp = temp->next;
		num++;
	}


}



/*
   Push_Command(char *arr)
   command를 history의 링크드리스트의 맨 뒤에 삽입함.
 */
void Push_Command(char *arr){

	history *save = NULL;
	history *temp;

	save = (history*)malloc(sizeof(history));
	strcpy(save->command, arr);
	save->next=NULL;

	if(head == NULL){
		head = save;
	}
	else{
		temp = head;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = save;
	}	
}


/*
   dump(int start, int end)
   du[mp] start, end
   start번지부터 end번지까지의 메모리의 내용을 <출력하는 메모리의 주소를 5자리 16진수 형태로, 메모리의 내용을 16진수 형태로, 메모리의 내용에 대응하는 ASCII code형태>로 출력해줌
start번지와 end번지가 있는 줄의 각각 첫번지~start-1번지와 end+1번지~끝번지는 16진수로 출력해주지않고 ASCII코드부분역시 '.' 으로 출력해주므로 case를 나눠 출력해줌
*/

void dump(int start, int end){

	int startaddr=start/16;
	int endaddr=end/16;
	
	for(int i= startaddr; i<=endaddr; i++){
		printf("%05X ", i*16);

		if(startaddr == endaddr){			//start와 end 번지가 한줄에 있는경우
			for(int j=0; j<start%16; j++){
				printf("   ");
			}

			for(int j=start%16; j<=end%16; j++){
				printf("%02X ", memory[j][i]);
			}
			for(int j=end%16+1; j<16; j++){
				printf("   ");
			}
			printf("; ");

			for(int j=0; j<start%16; j++){
				printf(".");
			}

			for(int j=start%16; j<=end%16; j++){
				
				if(memory[j][i]>=0x20 && memory[j][i]<=0x7E){
					printf("%c",memory[j][i]);
				}
				else{
					printf(".");
				}
			}
			for(int j=end%16+1; j<16; j++){
				printf(".");
			}
		}

		else if(i== startaddr){			//start번지가 있는 줄
			for(int j=0; j<start%16; j++){
				printf("   ");
			}
		
			for(int j=start%16; j<16; j++){
				printf("%02X ", memory[j][i]);
			}
			printf("; ");


			for(int j=0; j<start%16; j++){
				printf(".");
			}
			for(int j=start%16; j<16; j++){
				if(memory[j][i]>=0x20 && memory[j][i]<=0x7E){
					printf("%c",memory[j][i]);
				}
				else{
					printf(".");
				}
			}

		}

		else if(i== endaddr){					//end번지가 있는 줄
			for(int j=0; j<=end%16; j++){
				printf("%02X ", memory[j][i]);
			}
			for(int j=end%16+1; j<16; j++){
				printf("   ");
			}
			printf("; ");


			for(int j=0; j<=end%16; j++){
				if(memory[j][i]>=0x20 && memory[j][i]<=0x7E){
					printf("%c",memory[j][i]);
				}
				else{
					printf(".");
				}
			}
			for(int j=end%16+1; j<16; j++){
				printf(".");
			}

		}

		else{									//중간
			for(int j=0; j<16; j++){
				printf("%02X ", memory[j][i]);
			}
			printf("; ");

			for(int j=0; j<16; j++){
				if(memory[j][i]>=0x20 && memory[j][i]<=0x7E){
					printf("%c",memory[j][i]);
				}
				else{
					printf(".");
				}
			}


		}

		
					

		printf("\n");
	}

}

/*
   Edit(int addr, int value)
   e[dit]
   메모리에서 해당 address의 값을 value로 바꿔 줌*/

void Edit(int addr, int value){

	memory[addr%16][addr/16]=value;
}


/*
   Fill(int start, int end, int value)
   f[ill]
   for문에서 Edit함수를 호출하여 start에서 end번지까지 value로 바꿔 줌*/
void Fill(int start, int end, int value){
	for(int i=start; i<=end; i++){
		Edit(i, value);
	}
}

/*
   Reset()
   reset
   for문에서 Edit함수를 호출하여 메모리를 전부 0으로 바꿔 줌 */
void Reset(){
	for(int i=0; i<=0xFFFFF; i++){
		Edit(i, 0);
	}
}

/*
   Read_Opcode()
   opcode.txt 읽어오는 함수
   fopen으로 opcode.txt를 열어와서 파일의 끝까지 읽어온다.
   읽어온 내용을 opcode hash table에 링크드리스트 형태로 저장해준다.
 */
void Read_Opcode(){

	int n;
	int opcode;
	char mnemonic[100];
	char num[100];
	hash *new = NULL;
	hash *temp;
	
	FILE *ofp = fopen("opcode.txt", "r");

	if(ofp == NULL){
		printf("opcode.txt ERROR!");				//opcode파일이 없으면 ERROR출력
		return;
	}

	for(int i=0; i<20; i++){
		hashhead[i] = NULL;
	}

	while(fscanf(ofp, "%X%s%s", &opcode, mnemonic, num)!=EOF){
		new = (hash*)malloc(sizeof(hash));
		new->opcode = opcode;
		strcpy(new->command, mnemonic);
		strcpy(new->format, num);
		
		n=Hash_Function(mnemonic);					//저장할 hash값을 return받아온다.

		if(hashhead[n] == NULL){
			hashhead[n] = new;
		}
		else{
			temp = hashhead[n];
			while(temp->next !=  NULL){
				temp = temp->next;
			}
			temp->next = new;
		}
		//printf("%X %s %s\n", new->opcode, new->command, new->number);
	}

	fclose(ofp);									//파일을 닫음


}

/*
   Hash_Function(char *arr)
   hash 값 결정해서 return해줌
 */
int Hash_Function(char *arr){

	int sum=0;
	for(int i=0; i<strlen(arr); i++){
		sum+=(int)arr[i];
	}
	return sum%20;
}


/*
   Print_Opcode(char *arr)
   opcode mnemonic
   명령어에 해당하는 opcode를 출력하기 위해 링크드리스트로 구현된 hash table을 탐색한다
   유효한 mnemonic인 경우 그에 해당하는 opcode를 출력하고 1을 return하고
   유효하지않은 mnemonic인 경우 0을 return 한다
 */
int Print_Opcode(char *arr){
	char mnemonic[100];
	char *token;
	int num;

	hash *temp;

	token = strtok(arr, " ");
	token = strtok(NULL," ");
	strcpy(mnemonic, token);

	num = Hash_Function(mnemonic);

	temp = hashhead[num];
	while(strcmp(temp->command, mnemonic) != 0){
		temp = temp->next;
		if(temp == NULL){
			return 0;
		}
	}

	printf("opcode is %X\n", temp->opcode);
	return 1;
}

/*
	hash* Find_Opcode(char *arr)
	arr이 opcode table에 존재하면 그 opcode가 있는 노드를 반환하고 존재하지않으면 0을 반환
	*/
hash *Find_Opcode(char *arr){
	hash *temp;
	int num;
	num = Hash_Function(arr);
	temp = hashhead[num];

	while(strcmp(temp->command, arr) != 0){
		temp = temp->next;
		if(temp == NULL){
			return NULL;
		}
	}

	return temp;
//	strcpy(form, temp->format);

//	return (form[0]-48);
}


/*
   Opcode_List()
   opcodelist
   링크드리스트로 구현된 opcode hash table을 차례로 탐색하면서 그  내용 출력해준다*/
void Opcode_List(){
	hash *temp;

	for(int i=0; i<20; i++){
		printf("%d : ", i);
		temp = hashhead[i];
		if(temp != NULL){
			while(temp->next!=NULL){
				printf("[%s,%X] -> ", temp->command, temp->opcode);
				temp = temp->next;
			}
			printf("[%s,%X]", temp->command, temp->opcode);
		}
		printf("\n");
	}
}


/*
   int Print_File(char *arr)
   type filename
   현재 디렉터리에 해당 파일이 존재한다면 읽어서 화면에 출력하고 1을 반환
   해당파일이 존재하지 않으면 0을 반환
 */

int Print_File(char *arr){
	char filename[100];
	char *token;
	FILE *ofp=NULL;
	char get;

	token = strtok(arr, " ");
	token = strtok(NULL, " ");
	strcpy(filename, token);

	ofp = fopen(filename, "r");
	if(ofp == NULL)		//해당 파일이 존재하지 않음
	{
		printf("FILE DOES NOT EXIST!\n");
		return 0;
	}

	//while(fread(&get, sizeof(char), 1, ofp)){
	while(fscanf(ofp, "%c", &get)!=EOF){
		printf("%c", get);			//파일의 내용출력
	}
	return 1;
}

/*
   void Reset_SymbolTable()
   symbol table을 모두 초기화해주는 함수
 */
void Reset_SymbolTable(){

	symbol *symboldel;
	
	for(int i=0; i<26; i++){
		while(symbolhead[i]!=NULL){
			symboldel = symbolhead[i];
			symbolhead[i] = symbolhead[i]->next;
			free(symboldel);
		}
	}

}

/*
	void PasteSymbol(int sw)
	assemble 도중 오류가 있는경우 그전에 제대로 assemble된 경우의 symbol table 출력을 위해 복사해두는 함수
	sw가 1인 경우엔 symbolkeep에 symbolhead를 복사
	sw가 0인 경우엔 symbolhead에 symbolkeep을 복사*/
void PasteSymbol(int sw){
	symbol *temp;
	symbol *new;
	symbol *paste;

	if(sw ==0){					//assemble 중 오류
		Reset_SymbolTable();
		for(int i=0; i<26; i++){
			temp = symbolkeep[i];
			paste = symbolhead[i];

			while(temp!=NULL){
				new = (symbol*)malloc(sizeof(symbol));
				new->num = temp->num;
				strcpy(new->symbol, temp->symbol);
				new->next = NULL;
				if(symbolhead[i]==NULL){
					symbolhead[i] = new;
					paste = symbolhead[i];
					temp = temp->next;
					continue;
				}
				else{
					paste->next = new;
				}
				paste = paste->next;
				temp = temp->next;
			}
		}
	}

	else if(sw == 1){			//assemble성공

		symbol *symboldel;
	
		for(int i=0; i<26; i++){
			while(symbolkeep[i]!=NULL){
				symboldel = symbolkeep[i];
				symbolkeep[i] = symbolkeep[i]->next;
				free(symboldel);
			}
		}

		for(int i=0; i<26; i++){
			temp = symbolhead[i];
			paste = symbolkeep[i];

			while(temp!=NULL){
				new = (symbol*)malloc(sizeof(symbol));
				new->num = temp->num;
				strcpy(new->symbol, temp->symbol);
				new->next = NULL;
				if(symbolkeep[i]==NULL){
					symbolkeep[i] = new;
					temp = temp->next;
					paste = symbolkeep[i];
					continue;
				}
				else{
					paste->next = new;
				}
				paste = paste->next;
				temp = temp->next;
			}
		}
				
	}
}


/*
   void Make_SymbolTable(char *arr, int addr)
   symbol table에 symbol을 추가해
   */
void Make_SymbolTable(char *arr, int addr){
	symbol *new=NULL;
	symbol *temp;
	symbol *prev;
	symbol *sw;
	int hash;

	sw = Check_Duplicate(arr);		//symbol의 중복여부 확인
	if(sw!=NULL)					//중복되는 경우
		return;

	hash = arr[0]-65;
	new = (symbol*)malloc(sizeof(symbol));
	strcpy(new->symbol, arr);
	new->num = addr;
	new->next = NULL;
			/*새로운 노드 추가*/
	if(symbolhead[hash]==NULL){
		symbolhead[hash]=new;
	}
	else{
		temp = symbolhead[hash];
		prev = temp;
		while(temp->next!=NULL){
			if(strcmp(temp->symbol, arr)>0){	//알파벳 내림차순으로 정렬하기위해 string비교
				prev->next = new;
				new->next = temp;
				return;
			}

			prev = temp;
			temp=temp->next;
		}

		if(strcmp(temp->symbol, arr)>0){	//링크드 리스트의 마지막 node의앞에 위치시켜야 하는 경우
			if(prev == temp){		//head가 가리키는게 마지막 리스트였을 경우
				symbolhead[hash]=new;
				new->next=temp;
				//printf("a\n");
				return ;
			}
			prev->next = new;
			new->next = temp;
			return;
		}
		temp->next = new;
	}


}

/*
	symbol* Check_Duplicate(char *arr)
	이미 symbol table에 저장된 symbol인지 확인
	저장되어있는경우 그 노드를 반환, 아닌경우 NULL을 반환
 */
symbol * Check_Duplicate(char *arr){

	symbol *check = symbolhead[arr[0]-65];
	

	while(check != NULL){
		if(strcmp(arr, check->symbol)==0){
			return check;
		}
		check = check->next;
	}

	return NULL;
	

}

/*
   int Assemble_File(char*arr)
   file name에 해당하는 소스 파일을 읽어서 오브젝트파일과 리스팅파일을 만듦
 */
int Assemble_File(char*arr){
	char filename[100];
	char* token = NULL;
	int sw;

	token = strtok(arr, " ");
	token = strtok(NULL, " ");
	strcpy(filename, token);

	for(int i=0; i<100; i++){
		obj_len[i]=0;
	}

	sw=Pass1(filename);		//pass1 실행

	if(sw==0){				//pass1 실행중 에러 발견
		return 0;
		
	}

	sw = Pass2(filename);		//pass2 실행

	if(sw == 0){				//pass2 실행중 에러 발견
		return 0;
	}

	
	return 1;
}


/* 
	int Pass1(char*filename)
	pass1실행해 location해 location counter계산
 */
int Pass1(char* filename){
	FILE*ofpr,*ofpw;
	char str[100];
	char mnemonic[100];
	char sym[100];
	char temp[50];
	char *token;
	int line = 5;
	int location=0;
	int len;
	int form;
	hash *opcode;
	int operand;
	int obj_cnt=0;
	int obj_line=0;
	symbol *use;

	ofpr = fopen(filename, "r");
	if(ofpr==NULL){
		printf("FILE DOES NOT EXIST!\n");	//.asm 파일 존재하지않음
		return 0;
	}

	ofpw = fopen("loc.txt", "w");
	fgets(str, sizeof(str), ofpr);
	
	sscanf(str,"%s %s",temp, mnemonic);	//START가 저장되어야함
	
	if(strcmp(mnemonic, "START")!=0){	//START저장되어있지않음
		fclose(ofpr);
		fclose(ofpw);
		return 0;
	}
	fprintf(ofpw, "%d %d\n", line, location);
	line+=5;

	fgets(str, sizeof(str), ofpr);
	sscanf(str+7, "%s", mnemonic);

	while(strcmp(mnemonic, "END") != 0){		//END나올 때 까지
		len = strlen(str);

		fprintf(ofpw, "%d %d\n", line, location);
		
		line+=5;//line은 5 증가


		if(str[0] =='.'){
			
		}
		else{
			if(str[0]!=' '){						//symbol이 존재하는 경우symbol table에 저장
				sscanf(str, "%s", sym);

				use = Check_Duplicate(sym);			//중복 확인
				if(use != NULL){
					printf("SYMBOL ERROR!\n");		//같은 label이 두번 나오는 경우는 에러
					fclose(ofpr);
					fclose(ofpw);
					return 0;
				}
				Make_SymbolTable(sym, location);

				//printf("%s\n",temp);
			}

			if(strcmp(mnemonic,"BYTE") == 0){		//BYTE
				strncpy(temp, str+14, len-14);
				if(temp[0]=='C'){					//C
					token = strtok(temp+1, "'");
					location += strlen(token);
					if(obj_cnt + strlen(token)>30){
						obj_len[obj_line++]=obj_cnt;
						obj_cnt=0;
					}
					obj_cnt += strlen(token);
				}
				else if(temp[0]=='X'){				//X
					token = strtok(temp+1, "'");
					location += strlen(token)/2;
					if(obj_cnt + strlen(token)/2 >30){
						obj_len[obj_line++] = obj_cnt;
						obj_cnt=0;
					}
					obj_cnt +=strlen(token)/2;
				}
				else{
					printf("INVALID OPERAND!\n");

					fclose(ofpr);
					fclose(ofpw);
					return 0;
				}
			}

			else if(strcmp(mnemonic,"WORD")==0){		//WORD
				location += 3;
				if(obj_cnt + 3 >30){
					obj_len[obj_line++] = obj_cnt;
					obj_cnt=0;
				}
				obj_cnt += 3;
			}

			else if(strcmp(mnemonic, "RESB")==0){		//RESB
				sscanf(str+14, "%d", &operand);
				//printf("%d\n",operand);
				location += operand;
				if(obj_cnt !=0){
					obj_len[obj_line++] = obj_cnt;
					obj_cnt=0;
				}
			}
			else if(strcmp(mnemonic, "RESW")==0){		//RESW
				sscanf(str+14, "%d", &operand);
				location +=operand*3;
				if(obj_cnt !=0){
					obj_len[obj_line++] = obj_cnt;
					obj_cnt=0;
				}
			}
			else if(strcmp(mnemonic, "BASE")==0){		//BASE

			}
			else if(mnemonic[0] == '+'){				//format4
				sscanf(mnemonic+1, "%s", temp);
				opcode = Find_Opcode(temp);
				if(opcode ==NULL){						//opcode.txt에 존재하지 않는 mnemonic이 들어옴
					printf("OPCODE ERROR!\n");
					fclose(ofpr);
					fclose(ofpw);
					return 0;
				}
				else{
					location += 4;
					if(obj_cnt + 4 > 30){
						obj_len[obj_line++] = obj_cnt;
						obj_cnt = 0;
					}
					obj_cnt += 4;
				}
			}

			else{										//format 1, 2, 3
				opcode = Find_Opcode(mnemonic);
				if(opcode == NULL){						//opcode.txt에 존재하지 않는 mnemonic이 들어
					printf("OPCODE ERROR!\n");
					fclose(ofpr);
					fclose(ofpw);
					return 0;
				}
				else{
					form = opcode->format[0]-48;
					location += form;
					if(obj_cnt+form > 30){
						obj_len[obj_line++] = obj_cnt;
						obj_cnt=0;
					}
					obj_cnt += form;
				}
			}
		}
	

	
		fgets(str, sizeof(str), ofpr);
		sscanf(str+7,"%s", mnemonic);

	}

	program_len = location;
	obj_len[obj_line] = obj_cnt;
	fclose(ofpr);		//파일 닫아줌
	fclose(ofpw);

	return 1;
}


/*
   int Pass2(char*filename)
   pass2 실행해 .lst파일과 .obj파일 생성
   */
int Pass2(char*filename){
	FILE *ofp, *loc, *ofpl, *ofpj;
	char str[100];
	char sym[100];
	char opcode[100];
	char operand[10];
	char temp[100];		//format4일때 사용
	int modi[100];
	char *token;
	int line, location;
	int len;
	int imme;
	int comma=0;
	int flag=1;
	int obj;
	int form;
	int pc;
	int cnt = 0;
	int obj_cnt=0;
	int obj_line=0;

	hash *now;
	symbol *use;


	ofp = fopen(filename, "r");
	loc = fopen("loc.txt", "r");
	
	strcpy(str, filename);			//.lst파일 생성
	token = strtok(str, ".");
	strcpy(str, token);
	strcat(str, ".lst");
	ofpl = fopen(str, "w");


	strcpy(str, filename);			//.obj파일 생성
	token = strtok(str, ".");
	strcpy(str, token);
	strcat(str, ".obj");
	ofpj = fopen(str, "w");

	fgets(str, sizeof(str), ofp);
	fscanf(loc, "%d %d", &line, &location);
	//printf("aa\n");
	
	sscanf(str, "%s", sym);
	len = strlen(sym);
	for(int i=0; i<6-len; i++){
		strcat(sym," ");
	}

	base_addr = -1;
	len = strlen(str);
	str[len-1] = '\0';
	fprintf(ofpl, "%d\t%04X\t%s\n", line, location, str);		//START
	fprintf(ofpj, "H%s%06X%06X", sym, 0, program_len);
	//printf("%d\t%04X\t%s\n", line, location, str);

	fgets(str, sizeof(str), ofp);
	fscanf(loc, "%d %d", &line, &location);
	sscanf(str+7, "%s", opcode);

	while(strcmp(opcode,"END")!=0){
	//	now = (hash*)malloc(sizeof(hash));



		comma=0;		//operand의 콤마 수 초기화


		len = strlen(str);
		str[len-1] = '\0';

		if(str[0]=='.'){
			fprintf(ofpl, "%d\t\t%s\n", line, str);

		}

		else{
			if(strcmp(opcode, "WORD")==0){		//WORD
				//printf("aaa");
				//strncpy(operand, str+14, 10);
				if(sscanf(str+14, "%d", &imme)==1){
					//printf("aa");
					if(imme>=0x000000 && imme <=0xffffff){	//범위 확인
						fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n",line, location, str, imme);
					
						if(obj_cnt + 3 > 30 || obj_cnt == 0){
							fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
							obj_cnt=0;
							obj_line++;
						}
						fprintf(ofpj, "%06X", imme);
						obj_cnt += 3;
					}
					else{
						printf("INVALID OPERAND!\n");
						flag=0;
						break;
					}
				}
				else{
					printf("INVALID OPERAND!\n");
					flag= 0;
					break;
				}
				
			}
			else if(strcmp(opcode, "RESW")==0){	//RESW
				fprintf(ofpl,"%d\t%04X\t%s\n", line, location, str);
				if(obj_cnt !=0){
					obj_cnt=0;
				}
			}
			else if(strcmp(opcode, "RESB")==0){	//RESB
				fprintf(ofpl,"%d\t%04X\t%s\n", line, location, str);
				if(obj_cnt !=0){
					obj_cnt=0;
				}
			}
			else if(strcmp(opcode, "BYTE")==0){	//BYTE

				strncpy(operand, str+14, len-14);
				
				if(operand[0]=='C'){				//C
					token = strtok(operand+1, "'");
					fprintf(ofpl, "%d\t%04X\t%s\t\t", line, location, str);
					for(int i=0; i<strlen(token); i++){
						fprintf(ofpl, "%02X", token[i]);
					}
					fprintf(ofpl, "\n");
						
					
					if(obj_cnt + strlen(token) > 30 || obj_cnt == 0){
						fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
						obj_cnt=0;
						obj_line++;
					}
					for(int i=0; i<strlen(token); i++){
						fprintf(ofpj, "%02X", token[i]);
					}
					obj_cnt += strlen(token);
					
				}
				else if(operand[0]=='X'){			//X
					token = strtok(operand+1, "'");
					fprintf(ofpl, "%d\t%04X\t%s\t\t", line, location, str);
					for(int i=0; i<strlen(token); i++){
						fprintf(ofpl,"%c",token[i]);
					}
					fprintf(ofpl, "\n");


					if(obj_cnt + strlen(token)/2 > 30 || obj_cnt == 0){
						fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
						obj_cnt=0;
						obj_line++;
					}
					for(int i=0; i<strlen(token); i++){
						fprintf(ofpj,"%c",token[i]);
					}
					obj_cnt +=strlen(token)/2; 
				}
				else{
					printf("INVALID OPERAND!\n");
					flag = 0;
					break;
				}

			}
			else if(strcmp(opcode, "BASE")==0){	//BASE
				fprintf(ofpl,"%d\t\t%s\n", line, str);
				sscanf(str+14, "%s", operand);
				use = Check_Duplicate(operand);
				base_addr = use->num;
				//printf("%06X\n", base_addr);
			}
			else if(strcmp(opcode, "RSUB")==0){	//RSUB
				//printf("[%s]\n", opcode);
				fprintf(ofpl, "%d\t%04X\t%s\t\t\t%06X\n", line, location, str,0x4F0000);
				if(obj_cnt + 3 > 30 || obj_cnt == 0){
					fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
					obj_cnt=0;
					obj_line++;
				}
				fprintf(ofpj, "%06X",0x4F0000);
				obj_cnt += 3;
			}

			else if(opcode[0] == '+'){			//format4
				
				sscanf(opcode+1, "%s", temp);
				now = Find_Opcode(temp);

				if(now!=NULL){
					strncpy(operand, str+14, 10);
					for(int i=0; i<strlen(operand); i++){
						if(operand[i]==','){
							comma++;
						}
					}

					token = strtok(operand, ".\n");
					strcpy(operand, token);
				//	printf("%s\n", operand);
					if(comma == 1){					//simple
						token = strtok(operand, ",");
						use = Check_Duplicate(token);
						if(use!=NULL){				//nixbpe = 111001
							obj = Format4(now->opcode, 0x39, use->num);
							modi[cnt]=location;
							cnt++;
							fprintf(ofpl, "%d\t%04X\t%s\t\t%08X\n", line, location, str, obj);

							if(obj_cnt + 4 > 30 || obj_cnt == 0){
								fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
								obj_cnt=0;
								obj_line++;
							}
							fprintf(ofpj,"%08X", obj);
							obj_cnt +=4;
						}
						else{

							printf("INVALID OPERAND!\n");
							flag = 0;
							break;
						}
					}

					else if(comma ==0){
						if(operand[0]=='#'){			//immediate
							sscanf(token, "%s", operand);
							if(sscanf(operand+1,"%d", &imme) !=1){
	
								use = Check_Duplicate(operand+1);
								if(use != NULL){		//nixbpe = 010001
									obj = Format4(now->opcode,0x11, use->num);
									fprintf(ofpl, "%d\t%04X\t%s\t\t%08X\n", line, location, str, obj);
									modi[cnt]=location;
									cnt++;

									if(obj_cnt + 4 > 30 || obj_cnt == 0){
										fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
										obj_cnt=0;
										obj_line++;
									}
									fprintf(ofpj,"%08X", obj);
									obj_cnt +=4;

								}
								else{
									printf("INVALID OPERAND!\n");
									flag = 0;
									break;
								}
							}
							else{
								sscanf(operand+1, "%d", &imme);		//nixbpe = 010001
								obj = Format4(now->opcode, 0x11, imme);
								fprintf(ofpl, "%d\t%04X\t%s\t\t%08X\n", line, location, str, obj);
							
								if(obj_cnt + 4 > 30 || obj_cnt == 0){
									fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
									obj_cnt=0;
									obj_line++;
								}
								fprintf(ofpj,"%08X", obj);
								obj_cnt +=4;
							
							}

						}

						else if(operand[0] == '@'){			//indirect
							sscanf(token,"%s", operand);
							use = Check_Duplicate(operand+1);
							if(use!=NULL){					//nixbpe = 100001
								obj = Format4(now->opcode, 0x21, use->num);
								fprintf(ofpl, "%d\t%04X\t%s\t\t%08X\n", line, location, str, obj);
								modi[cnt]=location;
								cnt++;

								if(obj_cnt + 4 > 30 || obj_cnt == 0){
									fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
									obj_cnt=0;
									obj_line++;
								}
								fprintf(ofpj,"%08X", obj);
								obj_cnt +=4;
						

							}

							else{

								printf("INVALID OPERAND!\n");
								flag = 0;
								break;
							}

						}

						else{					//simple format4는 pc base 사용X
							sscanf(token, "%s", operand);
							use = Check_Duplicate(operand);
							if(use !=NULL){
								obj = Format4(now->opcode, 0x31, use->num);			//nixbpe = 110001
								fprintf(ofpl, "%d\t%04X\t%s\t\t%08X\n", line, location, str, obj);
								modi[cnt]=location;
								cnt++;


								if(obj_cnt + 4 > 30 || obj_cnt == 0){
									fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
									obj_cnt=0;
									obj_line++;
								}
								fprintf(ofpj,"%08X", obj);
								obj_cnt +=4;
							
							}
							else{
								printf("INVALID OPERAND!\n");
								flag = 0;
								break;
							}
						
						}
						
					}
					else{

						printf("INVALID OPERAND!\n");
						flag = 0;
						break;
					}
				}
				else{

					printf("INVALID OPERAND!\n");
					flag = 0;
					break;
				}
			}

			else{

				now = Find_Opcode(opcode);
				
				
				
				if(now!=NULL){
					form = now->format[0]-48;

					strncpy(operand, str+14, 10);
					token = strtok(operand,".\0");
					for(int i=0; i<strlen(operand); i++){
						if(operand[i]==',')
							comma++;
					}		//operand안의 comma수 세기

					if(comma == 1){				//comma 한개
						if(form == 2){			//format2
							obj = Format2(now->opcode, operand[0], operand[3]);
							fprintf(ofpl, "%d\t%04X\t%s\t\t%04X\n", line, location, str, obj);

							if(obj_cnt + 2 > 30 || obj_cnt == 0){
								fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
								obj_cnt=0;
								obj_line++;
							}
							fprintf(ofpj,"%04X", obj);
							obj_cnt +=2;
						
						}
						else if(form == 3){		//format3
							token = strtok(operand, ",");
							use = Check_Duplicate(token);
							if(use != NULL){
								pc = location+3;
								if(use->num - pc >= -2048 && use->num - pc <=2047){		//pc relative
									obj = Format3(now->opcode, 0x3A, use->num - pc);	//nixbpe = 111010
									fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n", line, location, str, obj);


									if(obj_cnt + 3 > 30 || obj_cnt == 0){
										fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
										obj_cnt=0;
										obj_line++;
									}
									fprintf(ofpj,"%06X", obj);
									obj_cnt +=3;
						
								}
								else if(use->num - base_addr >= 0 && use->num-base_addr < 4096 && base_addr != -1){		//base relative
									obj = Format3(now->opcode, 0x3C, use->num-base_addr);		//nixbpe = 111100
									fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n", line, location, str, obj);

									if(obj_cnt + 3 > 30 || obj_cnt == 0){
										fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
										obj_cnt=0;
										obj_line++;
									}
									fprintf(ofpj,"%06X", obj);
									obj_cnt +=3;
						
								}
								else{
									printf("INVALID OPERAND!\n");
									flag = 0;
									break;
								}
							}
							else{
								printf("INVALID OPERAND!\n");
								flag =0;
								 break;
							}
						}
						else{
							printf("INVALID OPERAND!\n");
							flag = 0;
							break;
						}

					}

					else if (comma == 0){
						if(form == 1){		//format1
							obj = now->opcode;
							fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n", line, location, str, obj);
							

							if(obj_cnt + 1 > 30 || obj_cnt == 0){
								fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
								obj_cnt=0;
								obj_line++;
							}
							fprintf(ofpj,"%02X", obj);
							obj_cnt += 1;
						
						}
						else if(form == 2){		//format2
							strncpy(operand, str+14, 10);
							//if(strcmp(opcode,"CLEAR")==0 || strcmp(opcode,"TIXR")==0){
							//printf("%c\n", operand[0]);
							obj = Format2(now->opcode, operand[0], 0);
							fprintf(ofpl, "%d\t%04X\t%s\t\t\t%04X\n", line, location, str, obj);



							if(obj_cnt + 2 > 30 || obj_cnt == 0){
								fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
								obj_cnt=0;
								obj_line++;
							}
							fprintf(ofpj,"%04X", obj);
							obj_cnt +=2;
						
							//}
						}
						else if(form == 3){		//format3
							sscanf(operand,"%s", temp);

							if(temp[0]=='#'){
								if(sscanf(temp+1,"%d", &imme) != 1){			//숫자아님
									sscanf(temp+1, "%s", operand);

									use = Check_Duplicate(operand);
									if(use!=NULL){
										pc = location+3;
										if(use->num - pc >= -2048 && use->num - pc <=2047){		//pc relative
											obj = Format3(now->opcode, 0x12, use->num - pc);	//nixbpe = 010010
											fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n", line, location, str, obj);

											if(obj_cnt + 3 > 30 || obj_cnt == 0){
												fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
												obj_cnt=0;
												obj_line++;
											}
											fprintf(ofpj,"%06X", obj);
											obj_cnt +=3;
										}


										else if(use->num - base_addr >= 0 && use->num-base_addr < 4096 && base_addr != -1){		//base relative
											obj = Format3(now->opcode, 0x14, use->num-base_addr);		//nixbpe = 010100
											fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n", line, location, str, obj);
								
											if(obj_cnt + 3 > 30 || obj_cnt == 0){
												fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
												obj_cnt=0;
												obj_line++;
											}
											fprintf(ofpj,"%06X", obj);
											obj_cnt +=3;
										}
									
										else{
											//printf("b\n");
											printf("INVALID OPERAND!\n");
											flag = 0;
											break;
										}
									}
									else{
										//printf("a\n");
										printf("INVALID OPERAND!\n");
										flag =0;
										 break;
									}
								}
								else{				//숫자
									if(imme >= -2048 && imme<=2047){
										obj = Format3(now->opcode, 0x10, imme);		//nixbpe = 010000
										fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n", line, location, str, obj);


										if(obj_cnt + 3 > 30 || obj_cnt == 0){
											fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
											obj_cnt=0;
											obj_line++;
										}
										fprintf(ofpj,"%06X", obj);
										obj_cnt +=3;
									}
									else{
										printf("INVALID OPERAND!\n");
										flag = 0;
										break;
									}
								}
							}
							else if(temp[0]=='@'){			//indirect
								sscanf(temp+1, "%s", operand);
								use = Check_Duplicate(operand);
								if(use!=NULL){
									pc = location+3;

									if(use->num - pc >= -2048 && use->num - pc <=2047){		//pc relative
										obj = Format3(now->opcode, 0x22, use->num - pc);	//nixbpe = 100010
										fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n", line, location, str, obj);
										if(obj_cnt + 3 > 30 || obj_cnt == 0){
											fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
											obj_cnt=0;
											obj_line++;
										}
										fprintf(ofpj,"%06X", obj);
										obj_cnt +=3;


									}
									else if(use->num - base_addr >= 0 && use->num-base_addr < 4096 && base_addr !=-1){		//base relative
										obj = Format3(now->opcode, 0x24, use->num-base_addr);		//nixbpe = 100100
										fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n", line, location, str, obj);
										if(obj_cnt + 3 > 30 || obj_cnt == 0){
											fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
											obj_cnt=0;
											obj_line++;
										}
										fprintf(ofpj,"%06X", obj);
										obj_cnt +=3;


									}
									else{
										printf("INVALID OPERAND!\n");
										flag = 0;
										break;
									}
								}
								else{

									printf("INVALID OPERAND!\n");
									flag = 0;
									break;
								}
								
							}
							else{				//simple
								
								use = Check_Duplicate(temp);
								if(use!=NULL){
									pc = location+3;
									
									if(use->num - pc >= -2048 && use->num - pc <=2047){		//pc relative
										obj = Format3(now->opcode, 0x32, use->num - pc);	//nixbpe = 110010
										fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n", line, location, str, obj);
							
										
										if(obj_cnt + 3 > 30 || obj_cnt == 0){
											fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
											obj_cnt=0;
											obj_line++;
										}
										fprintf(ofpj,"%06X", obj);
										obj_cnt +=3;
									}
									else if(use->num - base_addr >= 0 && use->num-base_addr < 4096 && base_addr != -1){		//base relative
										obj = Format3(now->opcode, 0x34, use->num-base_addr);		//nixbpe = 110100
										fprintf(ofpl, "%d\t%04X\t%s\t\t%06X\n", line, location, str, obj);
								
										
										if(obj_cnt + 3 > 30 || obj_cnt == 0){
											fprintf(ofpj, "\nT%06X%02X", location, obj_len[obj_line]);
											obj_cnt=0;
											obj_line++;
										}
										fprintf(ofpj,"%06X", obj);
										obj_cnt +=3;
									}
									else{
										printf("INVALID OPERAND!\n");
										flag = 0;
										break;
									}
								}
								else{

									printf("INVALID OPERAND!\n");
									flag = 0;
									break;
								}	



							}

						}
						else{
							
							printf("INVALID OPERAND!\n");
							flag = 0;
							break;
						}
							
					}
					else{

						printf("INVALID OPERAND!\n");
						flag = 0;
						break;
					}

				}
				else{

					printf("INVALID OPERAND!\n");
					flag = 0;
					break;
				}
			}
			


		}

			
		fgets(str, sizeof(str), ofp);
		fscanf(loc, "%d %d", &line, &location);
		sscanf(str+7, "%s", opcode);
	//	free(now);

	}
	fprintf(ofpl,"%d\t\t%s", line+5, str);


	for(int i=0; i<cnt; i++){			//modification record 출력
		fprintf(ofpj,"\nM%06X05", modi[i]+1);
				
	}
	
	fprintf(ofpj, "\nE%06X\n", 0x000000);

	fclose(ofp);
	fclose(loc);
	fclose(ofpl);

	fclose(ofpj);
	if(flag == 1)
		printf("Successfully assemble %s\n", filename);
	else{		//중간에 에러 존재
		strcpy(str, filename);			//.lst파일 삭제
		token = strtok(str, ".");
		strcpy(str, token);
		strcat(str, ".lst");
		remove(str);


		strcpy(str, filename);			//.obj파일 삭제
		token = strtok(str, ".");
		strcpy(str, token);
		strcat(str, ".obj");
		remove(str);
	}


	return flag;

}

/*
   [format4]
   opcode, addr을 통해 objcode생성
   */
int Format4(int opcode, int reg, int addr){
	int obj=0;


	addr&=0xFFFFF;
	opcode <<= 24;
	obj |= opcode;
	reg <<= 20;
	obj |= reg;
	obj |= addr;

	return obj;

}


/*
   [format3]
   opcode, nixbpe, addr통해 objcode 생성*/
int Format3(int opcode, int reg, int addr){
	int obj = 0;
	addr &=0xFFF;

	opcode <<=16;
	obj |= opcode;
	reg <<= 12;
	obj |= reg;
	obj |= addr;
	
	return obj;

}

/*
   [format2]
   opcode, reg1, reg2 통해 objcode생성*/
int Format2(int opcode, char reg1, char reg2){
	int obj = 0;
	int r1=0, r2=0;

	if(reg1 == 'A')
		r1 = 0;
	else if(reg1 == 'X')
		r1 = 1;
	else if(reg1 == 'L')
		r1 = 2;
	else if(reg1 == 'B')
		r1 = 3;
	else if(reg1 == 'S')
		r1 = 4;
	else if(reg1 == 'T')
		r1 = 5;
	else if(reg1 == 'F')
		r1 = 6;


	if(reg2 == 'A')
		r2 = 0;
	else if(reg2 == 'X')
		r2 = 1;
	else if(reg2 == 'L')
		r2 = 2;
	else if(reg2 == 'B')
		r2 = 3;
	else if(reg2 == 'S')
		r2 = 4;
	else if(reg2 == 'T')
		r2 = 5;
	else if(reg2 == 'F')
		r2 = 6;

	opcode <<= 8;
	obj |= opcode;
	r1 <<= 4;
	obj |= r1;
	obj |= r2;

	return obj;
}


/*
   void Print_SymbolTable()
   assemble한 파일의 symbol table 출력*/
void Print_SymbolTable(){
	symbol *temp;
	for(int i=0; i<26; i++){
		temp = symbolhead[i];
		while(temp != NULL){
			printf("\t%s\t%04X\n", temp->symbol, temp->num);
			temp = temp->next;
		}
	}
	//Reset_SymbolTable();
}

/*
   Get_Command()
   command를 입력받아 유효한 명령어인 경우 각각의 명령어에 맞는 함수를 실행해 주고
	유효하지 않은 경우엔 ERROR를 출력해 준다*/
void Get_Command(){
	char str[100]={0};
	char save[100]={0};
	char use[100]={0};
	char command[100] = {0};
	int num[5]={-1, -1, -1,-1,-1};
	int i, comma=0;
	int len;
	char *temp=NULL;
	int cnt=0;
	int sw;



	printf("sicsim> ");
	fgets(str, sizeof(str), stdin);			//명령어 입력받음
	len = strlen(str);
	str[len-1] = '\0';

	for(i=0; i<len;i++){
		if(str[i] == ','){
			comma++;
		}
	}//dump [start,end], edit, fill 인경우엔 0이 아님


	strcpy(save, str);			//history에 push하기위해 원래 명령 그대로 저장
	temp = strtok(str, ", ");
	strcpy(command, temp);		//명령어의 첫단어만 잘라서 저장
	//printf("%s", command);

	temp = strtok(NULL, ", ");	

	while(temp != NULL){
		sscanf(temp, "%X", &num[cnt]);		//숫자부분 저장우
		cnt++;
		temp = strtok(NULL,", ");
	}

	if(strcmp(command,"h")==0 || strcmp(command, "help")==0){

		if(cnt!=0)
			printf("command ERROR\n");
		else{
			Push_Command(save);
			Print_All_Command();
		}
		
	}// h[elp]	

	else if(strcmp(command, "q")==0 || strcmp(command,"quit")==0){
		if(cnt!=0)
			printf("command ERROR!\n");
		
		else{
			Push_Command(save);
			Quit_Sicsim();
		}
	}//q[uit]

	else if(strcmp(command, "hi")==0 || strcmp(command, "history")==0){
		if(cnt!=0)
			printf("command ERROR!\n");
		else{
			Push_Command(save);
			View_History();
		}
	}//hi[story]

	else if(strcmp(command, "d")==0 || strcmp(command, "dir")==0){
		if(cnt!=0){
			printf("command ERROR!\n");
		}
		else{
			Push_Command(save);
			Current_Directory_File();
		}
	}//d[ir]

	else if(strcmp(command, "opcode")==0 && cnt==1){
		//Push_Command(save);
		strcpy(use, save);
		sw = Print_Opcode(use);
		if(sw == 1){
			Push_Command(save);				//opcode.txt에 존재하는 mnemonic일때만 history에 삽입
		}
		else{
			printf("command ERROR!\n");		//존재하지않는 mnemonic인 경우 ERROR 출력
		}
	}//opcode mnemonic

	else if(strcmp(command, "opcodelist")==0 && cnt==0){
		Push_Command(save);
		Opcode_List();
	}//opcodelist

	else if(strcmp(command, "dump")==0 || strcmp(command, "du")==0){
		if(cnt>2){
			printf("command ERROR!\n");
		}

		else{
			if(comma>=2){
				printf("command ERROR!\n");
			}
			else if(comma==1 && cnt==2){		//dump start, end
				if(num[0]!=-1 && num[1]!=-1){
					if((num[0]<=num[1]) && (num[0] >=0) &&(num[1]>=0) && (num[0]<=0xFFFFF) && (num[1]<=0xFFFFF)){
						Push_Command(save);
						dump(num[0], num[1]);
						dumpend = num[1];
						dumpstart = num[0];
					}
					else{
						printf("command ERROR!\n");
					}
				}
				else{
					printf("command ERROR!\n");
				}
			}
			else if(comma==0){
				if(num[0]==-1 && cnt==0){		//dump
					Push_Command(save);
					
					dumpstart = dumpend+ 1; 

					if(dumpstart > 0xFFFFF){
						dumpstart=0;
					}

					dumpend = dumpstart+159;
					if(dumpend > 0xFFFFF){
						dumpend = 0xFFFFF;
					}

					dump(dumpstart, dumpend);
				}

				else if(num[0]!=-1 && cnt==1){		//dump start
					if(num[0]>=0 && num[0]<=0xFFFFF){
						Push_Command(save);
						dumpstart = num[0];
						dumpend = dumpstart+159;
						if(dumpend>0xFFFFF){
							dumpend=0xFFFFF;
						}

						dump(dumpstart, dumpend);
					}
					else{
						printf("command ERROR!\n");
					}


				}
				else{
					printf("command ERROR!\n");
				}
			
			}
		}
	}//du[mp]

	else if(strcmp(command, "e")==0 || strcmp(command, "edit")==0){
		if(comma==1 && cnt==2){
			if(num[0]>=0 && num[0]<=0xFFFFF && num[1]>=0 && num[1]<=0xFF){
				Push_Command(save);
				Edit(num[0], num[1]);
			}
			else{
				printf("command ERROR!\n");
			}
		}
		else{
			printf("command ERROR!\n");
		}
	}//e[dit]

	else if(strcmp(command,"f")==0 || strcmp(command, "fill")==0){
		if(comma == 2 && cnt==3){
			if(num[0]>=0 && num[0]<=0xFFFFF && num[1]>=0 && num[1]<=0xFFFFF && num[0]<=num[1] && num[2]>=0 && num[2]<=0xFF){
				Push_Command(save);
				Fill(num[0], num[1], num[2]);
			}
			else{
				printf("command ERROR!\n");
			}
		}
		else{
			printf("command ERROR!\n");
		}
	}//f[ill]

	else if(strcmp(command, "reset")==0 && cnt==0){
		Push_Command(save);
		Reset();
	}//reset


	else if(strcmp(command,"type")==0 && cnt==1){

		strcpy(use, save);
		sw = Print_File(use);

		if(sw==1){
			Push_Command(save);		//해당파일이 존재할 때만 명령을 history에 저장
		}

	}//type filename

	else if(strcmp(command, "assemble")==0 && cnt==1){
		Reset_SymbolTable();
		strcpy(use, save);
		sw = Assemble_File(use);
		if(sw==1){
			Push_Command(save);
		}
		else{
			printf("ASSEMBLE ERROR!\n");
		}

		PasteSymbol(sw);
		remove("loc.txt");		//추가로 사용한 txt파일 삭제

	}//assemble filename

	else if(strcmp(command, "symbol")==0 && cnt ==0){
		Print_SymbolTable();
		Push_Command(save);

	}//symbol

	else{
		printf("command ERROR!\n");
	}

}
