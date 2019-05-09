#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void line_parser(int ad3cnt, int ad2cnt, int ad1cnt, int cmdcnt, int ctr, char *addr3, char *addr2, char *addr1, char *cmd, char *line);
void print_bits(FILE *out, int num);
void print_where(FILE *out, char* arg);
int print_loc(FILE *out, char *loc, int run);

int main(int argc, char **argv)
{
	FILE *in, *out;
	out = fopen(argv[2],"w");
	in = fopen(argv[1],"r");
	char line[1024];
	int i_num=0,invalid_flag=0;
	for (int i = 0; i < 2; i++) {
		if (i) {
			fprintf(out,"`include \"CPU.vh\"\n\nmodule AsyncROM(input [7:0] addr,\n\toutput reg [34:0] data);\n");
			fprintf(out,"\talways @(addr)\n\t\tcase(addr)\n");
		}
		while (fgets(line, 1024, in)) {
			if (strlen(line) < 5)
				continue;
			char cmd[5] = "", addr1[10] = "", addr2[10] = "", addr3[10] = "";
			int ctr=0,cmdcnt=0,ad1cnt=0,ad2cnt=0,ad3cnt=0;
			for (int i = 0; line[i]; i++) {
				if (line[i] == '\t') {
					continue;
				}
				if (line[i] == ' ') {
					ctr++;
					continue;
				}
				if (line[i] == ';' || line[i] == '/')
					break;
				switch (ctr) {
				case 0: cmd[cmdcnt++] = line[i];
					break;
				case 1: addr1[ad1cnt++] = line[i];
					break;
				case 2: addr2[ad2cnt++] = line[i];
					break;
				case 3: addr3[ad3cnt++] = line[i];
					break;
				default:
					break;
				}
			}
			 cmd[cmdcnt] = '\0'; addr1[ad1cnt] = '\0'; addr2[ad2cnt] = '\0'; addr3[ad3cnt] = '\0';
			if (cmdcnt == 3)
				cmd[++cmdcnt] = '\0';
			if (cmdcnt == 4)
				cmd[++cmdcnt] = '\0';
			if (i)
				fprintf(out,"\t\t\t%d: data = {",i_num++);
			switch (*((int *) cmd)) {
			case 0x504f4e: //NOP (NOP)
				if (i)
					fprintf(out,"35'b0");
				break;
			case 0x435441: //ATC (ATC)
				if (i) {
					fprintf(out,"`ATC,");
					invalid_flag=print_loc(out,addr1,i);
					fprintf(out,"`N10,`N10,");
					print_bits(out,atoi(addr2)-1);
				}
				break;
			case 0x564f4d: //MOV PUR (MOV)
				if (i) {
					fprintf(out,"`MOV,`PUR,");
					print_where(out, addr1);
					print_where(out, addr2);
					fprintf(out, "`N8");
				}
				break;
			case 0x4c4853: //MOV SHL (SHL)
				if (i) {
					fprintf(out,"`MOV,`SHL,");
					print_where(out, addr1);
					print_where(out, addr2);
					fprintf(out, "`N8");
				}
				break;
			case 0x524853: //MOV SHR (SHR)
				if (i) {
					fprintf(out,"`MOV,`SHR,");
					print_where(out, addr1);
					print_where(out, addr2);
					fprintf(out, "`N8");
				}
				break;
			case 0x504d4a: //JMP UNC (JMP)
				if (i) {
					fprintf(out,"`JMP,`UNC,");
					fprintf(out,"`N10,`N10,");
					print_bits(out,atoi(addr1)-1);
				}
				break;
			case 0x51454a: //JMP EQ (JEQ)
				if (i) {
					fprintf(out,"`JMP,`EQ,");
					print_where(out,addr1);
					print_where(out,addr2);
					print_bits(out,atoi(addr3)-1);
				}
				break;
			case 0x544c554a: //JMP ULT (JULT)
				if (i) {
					fprintf(out,"`JMP,`ULT,");
					print_where(out, addr1);
					print_where(out, addr2);
					print_bits(out, atoi(addr3)-1);
				}
				break;
			case 0x544c534a: //JMP SLT (JSLT)
				if (i) {
					fprintf(out,"`JMP,`SLT,");
					print_where(out, addr1);
					print_where(out, addr2);
					print_bits(out, atoi(addr3)-1);
				}
				break;
			case 0x454c554a: //JMP ULE (JULE)
				if (i) {
					fprintf(out,"`JMP,`ULE,");
					print_where(out,addr1);
					print_where(out,addr2);
					print_bits(out,atoi(addr3)-1);
				}
				break;
			case 0x454c534a: //JMP SLE (JSLE)
				if (i) {
					fprintf(out,"`JMP,`SLE,");
					print_where(out,addr1);
					print_where(out,addr2);
					print_bits(out,atoi(addr3)-1);
				}
				break;
			case 0x444155: //ACC UAD (UAD)
				if (i) {
					fprintf(out,"`ACC,`UAD,");
					print_where(out,addr1);
					print_where(out,addr2);
					fprintf(out,"`N8");
				}
				break;
			case 0x444153: //ACC SAD (SAD)
				if (i) {
					fprintf(out,"`ACC,`SAD,");
					print_where(out,addr1);
					print_where(out,addr2);
					fprintf(out,"`N8");
				}
				break;
			case 0x544d55: //ACC UMT (UMT)
				if (i) {
					fprintf(out,"`ACC,`UMT,");
					print_where(out,addr1);
					print_where(out,addr2);
					fprintf(out,"`N8");
				}
				break;
			case 0x544d53: //ACC SMT (SMT)
				if (i) {
					fprintf(out,"`ACC,`SMT,");
					print_where(out,addr1);
					print_where(out,addr2);
					fprintf(out,"`N8");
				}
				break;
			case 0x444e41: //ACC AND (AND)
				if (i) {
					fprintf(out,"`ACC,`AND,");
					print_where(out,addr1);
					print_where(out,addr2);
					fprintf(out,"`N8");
				}
				break;
			case 0x524f: //ACC OR (OR)
				if (i) {
					fprintf(out,"`ACC,`OR,");
					print_where(out,addr1);
					print_where(out,addr2);
					fprintf(out,"`N8");
				}
				break;
			case 0x524f58: //ACC XOR (XOR)
				if (i) {
					fprintf(out,"`ACC,`XOR,");
					print_where(out,addr1);
					print_where(out,addr2);
					fprintf(out,"`N8");
				}
				break;
			default:
				printf("ERROR: Invalid command %s\n",cmd);
				invalid_flag=1;
				break;
			}
			if (i)
				fprintf(out,"};\n");
		}
		if (invalid_flag && !i) {
			printf("There are errors, please fix them before contiuing\n");
			exit(0);
		}
		if (i) {
			fprintf(out,"\t\t\tdefault: data = 35'b0;\n");
			fprintf(out,"\tendcase\nendmodule");
		}
		rewind(in);
	}
}

void print_bits(FILE *out, int num)
{
	fprintf(out,"8'd%d",num);
}

void print_where(FILE *out, char* arg)
{
	if (arg[0] == 'R') {
		fprintf(out, "`REG,");
		print_bits(out, atoi(arg+1));
		fprintf(out,",");
	} else if (arg[0] == '*') {
		fprintf(out, "`IND,");
		print_bits(out, atoi(arg+1));
		fprintf(out,",");
	} else if (arg[0] == 'O') {
		fprintf(out,"`REG,`DOUT,");
	} else if (arg[0] == 'I') {
		fprintf(out,"`REG,`DINP,");
	} else if (arg[0] == 'G') {
		fprintf(out,"`REG,`GOUT,");
	} else {
		fprintf(out, "`NUM,");
		print_bits(out, atoi(arg));
		fprintf(out,",");
	}
}

int print_loc(FILE *out, char *loc, int run)
{
	switch (*((int *) loc)) {
	case 0x4c504d53: //SMPL
		if (run)
			fprintf(out,"`SMPL,");
		break;
	case 0x54464853: //SHFT
		if (run)
			fprintf(out,"`SHFT,");
		break;
	case 0x574c464f: //OFLW
		if (run)
			fprintf(out,"`OFLW,");
		break;
	case 0x324e5442: //BTN2
		if (run)
			fprintf(out,"3'b010,");
		break;
	case 0x314e5442: //BTN1
		if (run)
			fprintf(out,"3'b001,");
		break;
	case 0x304e5442: //BTN0
		if (run)
			fprintf(out,"3'b000,");
		break;
	default:
		printf("ERROR: Invalid flag: %s, %x",loc, *((int *)loc));
		return 1;
		break;
	}
}
