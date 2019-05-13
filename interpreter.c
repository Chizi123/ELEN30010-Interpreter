#include <stdio.h>
#include <stdlib.h>

#define SHFT 5
#define OFLW 4
#define SMPL 3
#define BTN2 2
#define BTN1 1
#define BTN0 0

struct instruction {
	char cmd[5];
	char addr1[10];
	char addr2[10];
	char addr3[10];
};

int parser(FILE *in, struct instruction *ins);
char *get_loc(char *Reg, char *addr, int IP);
char get_num(char *Reg, char *addr);
int print_loc(char *loc);

int main(int argc, char **argv) {
	FILE *in;
	in = fopen(argv[1], "r");
	char IP = 0;
	struct instruction ins[256];
	char Reg[32];
	char *DINP = Reg + 28;
	char *GOUT = Reg + 29;
	char *DOUT = Reg + 30;
	char *FLAG = Reg + 31;
	char ad1, ad2, ad3, *loc;
	int temp;
	*DOUT = 0;
	*DINP = 0;
	*GOUT = 0;

	parser(in, ins);

	while (1) {
		switch (*((int *)ins[IP].cmd)) {
		case 0x504f4e: // NOP (NOP)
			break;
		case 0x435441: // ATC (ATC)
			if (*FLAG & 1 << print_loc(ins[IP].addr1)) {
				*FLAG &= ~(1 << print_loc(ins[IP].addr2));
				IP = atoi(ins[IP].addr2);
				continue;
			}
			*FLAG &= ~(1 << print_loc(ins[IP].addr2));
			break;
		case 0x564f4d: // MOV PUR (MOV)
			ad1 = get_num(Reg, ins[IP].addr1);
			loc = get_loc(Reg, ins[IP].addr2, IP);
			*loc = ad1;
			break;
		case 0x4c4853: // MOV SHL (SHL)
			ad1 = get_num(Reg, ins[IP].addr1);
			loc = get_loc(Reg, ins[IP].addr2, IP);
			*loc = ((unsigned char)ad1) << 1;
			break;
		case 0x524853: // MOV SHR (SHR)
			ad1 = get_num(Reg, ins[IP].addr1);
			loc = get_loc(Reg, ins[IP].addr2, IP);
			*loc = ((unsigned char)ad1) >> 1;
			break;
		case 0x504d4a: // JMP UNC (JMP)
			ad1 = get_num(Reg, ins[IP].addr1);
			IP = ad1;
			continue;
			break;
		case 0x51454a: // JMP EQ (JEQ)
			ad1 = get_num(Reg, ins[IP].addr1);
			ad2 = get_num(Reg, ins[IP].addr2);
			ad3 = get_num(Reg, ins[IP].addr3);
			if ((ad1 == ad2)) {
				IP = ad3;
				continue;
			}
			break;
		case 0x544c554a: // JMP ULT (JULT)
			ad1 = get_num(Reg, ins[IP].addr1);
			ad2 = get_num(Reg, ins[IP].addr2);
			ad3 = get_num(Reg, ins[IP].addr3);
			if ((unsigned char)ad1 < (unsigned char)ad2) {
				IP = ad3;
				continue;
			}
			break;
		case 0x544c534a: // JMP SLT (JSLT)
			ad1 = get_num(Reg, ins[IP].addr1);
			ad2 = get_num(Reg, ins[IP].addr2);
			ad3 = get_num(Reg, ins[IP].addr3);
			if (ad1 < ad2) {
				IP = ad3;
				continue;
			}
			break;
		case 0x454c554a: // JMP ULE (JULE)
			ad1 = get_num(Reg, ins[IP].addr1);
			ad2 = get_num(Reg, ins[IP].addr2);
			ad3 = get_num(Reg, ins[IP].addr3);
			if ((unsigned char)ad1 <= (unsigned char)ad2) {
				IP = ad3;
				continue;
			}
			break;
		case 0x454c534a: // JMP SLE (JSLE)
			ad1 = get_num(Reg, ins[IP].addr1);
			ad2 = get_num(Reg, ins[IP].addr2);
			ad3 = get_num(Reg, ins[IP].addr3);
			if (ad1 <= ad2) {
				IP = ad3;
				continue;
			}
			break;
		case 0x444155: // ACC UAD (UAD)
			loc = get_loc(Reg, ins[IP].addr1, IP);
			ad1 = get_num(Reg, ins[IP].addr2);
			temp = *loc = (unsigned char)(*loc) + (unsigned char)ad1;
			if (temp > 255)
				*FLAG |= 1 << OFLW;
			break;
		case 0x444153: // ACC SAD (SAD)
			loc = get_loc(Reg, ins[IP].addr1, IP);
			ad1 = get_num(Reg, ins[IP].addr2);
			temp = *loc = (*loc) + ad1;
			if (temp < -128 || temp > 127)
				*FLAG |= 1 << OFLW;
			break;
		case 0x544d55: // ACC UMT (UMT)
			loc = get_loc(Reg, ins[IP].addr1, IP);
			ad1 = get_num(Reg, ins[IP].addr2);
			temp = *loc = (unsigned char)(*loc) * (unsigned char)ad1;
			if (temp > 255)
				*FLAG |= 1 << OFLW;
			break;
		case 0x544d53: // ACC SMT (SMT)
			loc = get_loc(Reg, ins[IP].addr1, IP);
			ad1 = get_num(Reg, ins[IP].addr2);
			temp = *loc = (*loc) * ad1;
			if (temp < -128 || temp > 127)
				*FLAG |= 1 << OFLW;
			break;
		case 0x444e41: // ACC AND (AND)
			loc = get_loc(Reg, ins[IP].addr1, IP);
			ad1 = get_num(Reg, ins[IP].addr2);
			*loc &= ad1;
			break;
		case 0x524f: // ACC OR (OR)
			loc = get_loc(Reg, ins[IP].addr1, IP);
			ad1 = get_num(Reg, ins[IP].addr2);
			*loc |= ad1;
			break;
		case 0x524f58: // ACC XOR (XOR)
			loc = get_loc(Reg, ins[IP].addr1, IP);
			ad1 = get_num(Reg, ins[IP].addr2);
			*loc ^= ad1;
			break;
		default:
			printf("ERROR: Invalid command %s\n", ins[IP].cmd);
			exit(2);
			break;
		}
		printf("%d %d\n", IP - 1, *DOUT);
		IP++;
	}
}

int parser(FILE *in, struct instruction *ins) {
	char line[1024];
	int i = 0;
	for (i = 0; fgets(line, 1024, in); i++) {
		int ctr = 0, cmdcnt = 0, ad1cnt = 0, ad2cnt = 0, ad3cnt = 0;
		for (int j = 0; line[j]; j++) {
			if (line[j] == '\t') {
				continue;
			}
			if (line[j] == ' ') {
				ctr++;
				continue;
			}
			if (line[i] == ';' || line[i] == '/')
				break;
			switch (ctr) {
			case 0:
				ins[i].cmd[cmdcnt++] = line[j];
				break;
			case 1:
				ins[i].addr1[ad1cnt++] = line[j];
				break;
			case 2:
				ins[i].addr2[ad2cnt++] = line[j];
				break;
			case 3:
				ins[i].addr3[ad3cnt++] = line[j];
				break;
			default:
				break;
			}
		}
		ins[i].cmd[cmdcnt] = '\0';
		ins[i].addr1[ad1cnt] = '\0';
		ins[i].addr2[ad2cnt] = '\0';
		ins[i].addr3[ad3cnt] = '\0';
		if (cmdcnt == 3)
			ins[i].cmd[++cmdcnt] = '\0';
		if (cmdcnt == 4)
			ins[i].cmd[++cmdcnt] = '\0';
	}
	return 1;
}

char *get_loc(char *Reg, char *addr, int IP) {
	if (addr[0] == 'R') {
		return Reg + atoi(addr + 1);
	} else if (addr[0] == '*') {
		return Reg + Reg[atoi(addr + 1)];
	} else if (addr[0] == 'I') {
		return Reg + 28;
	} else if (addr[0] == 'O') {
		return Reg + 30;
	} else {
		printf("Error: Line %d, trying to move into a number", IP);
		exit(0);
	}
}

char get_num(char *Reg, char *addr) {
	if (addr[0] == 'R')
		return Reg[atoi(addr + 1)];
	else if (addr[0] == '*')
		return Reg[(int)Reg[atoi(addr + 1)]];
	else if (addr[0] == 'I')
		return Reg[28];
	else if (addr[0] == 'O')
		return Reg[30];
	else
		return atoi(addr);
}

int print_loc(char *loc) {
	switch (*((int *)loc)) {
	case 0x4c504d53: // SMPL
		return SMPL;
		break;
	case 0x54464853: // SHFT
		return SHFT;
		break;
	case 0x574c464f: // OFLW
		return OFLW;
		break;
	case 0x42544e32: // BTN2
		return BTN2;
		break;
	case 0x314e5442: // BTN1
		return BTN1;
		break;
	case 0x304e5442: // BTN0
		return BTN0;
		break;
	default:
		printf("ERROR: Invalid flag: %s", loc);
		exit(1);
		break;
	}
}
