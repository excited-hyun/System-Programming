#include "20171653.h"

unsigned char memory[16][65536];

history *head;
hash *hashhead[20];
int dumpend=-1;
int dumpstart;


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
		sscanf(temp, "%X", &num[cnt]);		//숫자부분 저장
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
		int sw = Print_Opcode(use);
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

	else{
		printf("command ERROR!\n");
	}

}
