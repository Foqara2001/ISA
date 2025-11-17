#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include<string.h>

int registerMemory[16] = { 0 };
unsigned int IORegister[23] = { 0 };
char IMEM[4096][10];
unsigned int diskinArr[128][128] = { { 0 } };
int pc = 0;
unsigned char frameBuffer[256][256] = { { 0 } };
int interaptMode = 1;
int irq2inMem[4096];
char diskArr[16386][20];
int busyTime = 0;
int f = 0;
FILE* hwregtrace = NULL;

int openMEMIN(char* IMEMIN);
int openDISKIN(char* diskinF);
int addF(int rd, int rs, int rt, int big);
int subF(int rd, int rs, int rt, int big);
int mulF(int rd, int rs, int rt, int big);
int andF(int rd, int rs, int rt, int big);
int orF(int rd, int rs, int rt, int big);
int xorF(int rd, int rs, int rt, int big);
int sllF(int rd, int rs, int rt, int big);
int sraF(int rd, int rs, int rt, int big);
int srlF(int rd, int rs, int rt, int big);
int beqF(int rd, int rs, int rt, int big);
int bneF(int rd, int rs, int rt, int big);
int bltF(int rd, int rs, int rt, int big);
int bgtF(int rd, int rs, int rt, int big);
int bleF(int rd, int rs, int rt, int big);
int bgeF(int rd, int rs, int rt, int big);
int jalF(int rd, int rs, int rt, int big);
int lwF(int rd, int rs, int rt, int big);
int swF(int rd, int rs, int rt, int big);
int retiF(int rd, int rs, int rt, int big);
int inF(int rd, int rs, int rt, int big);
int outF(int rd, int rs, int rt, int big);
int regAssign(int* op, int* rd, int* rs, int* rt, int* imm, int* bigmim);
int trace(char* argv_7);
int instructionOp(int op, int rd, int rs, int rt, int bigmim);
int createLeds(char* ledsF);
int openIrq2n(char* irq2inF);
int createCycles(char* cyc);
int createDisplay7Seg(char* display7segF);
int createMonitor(char* monitortxt, char* monitoryuv);
int createRegout(char* regoutF);
int createHwregtrace(char* hw, int op, int addr);
int writeDisk();
int readDisk();
int openDISKIN(char* diskinF);
int createDISKOUT(char* diskoutF);
int openFile(char* File);
int from_char_hex_to_deci(char hex);
void from_char_hex_to_int(char* data, int* address);

int main(int argc, char* argv[])

{
	
	int irq2c = 0;
	printf("try to open MEMIN, the argumet is :%s\n", argv[1]);
	openMEMIN(argv[1]);
	printf("try to open diskin, the argumet is :%s\n", argv[2]);
	openDISKIN(argv[2]);
	printf("try to open Irq2n, the argumet is :%s\n", argv[3]);
	openIrq2n(argv[3]);
	printf("try to open file, the argumet is :%s\n", argv[6]);
	openFile(argv[6]);
	printf("try to open file, the argumet is :%s\n", argv[7]);
	openFile(argv[7]);
	printf("try to open file, the argumet is :%s\n", argv[8]);
	openFile(argv[8]);
	printf("try to open file, the argumet is :%s\n", argv[10]);
	openFile(argv[10]);
	printf("try to open file, the argumet is :%s\n", argv[11]);
	openFile(argv[11]);

	while (pc < 4096)
	{
		printf("the cur pc is :%d\n", pc);

		if (IORegister[17] == 1) {
			busyTime++; //disk is busy
			if (busyTime == 1024) {
				busyTime = 0;
				IORegister[17] = 0;
				IORegister[14] = 0;
				IORegister[4] = 1;
			}
		}

		if (((IORegister[0] && IORegister[3]) || (IORegister[1] && IORegister[4]) || (IORegister[2] && IORegister[5])) && interaptMode)
		{
			IORegister[7] = pc; // save current PC
			pc = IORegister[6]; // jump to interrupt handler
			interaptMode = 0;
			IORegister[5] = 0; // clear irq2status
			if (IMEM[pc][5] == '1') {
				IORegister[8] = IORegister[8] + 2;
			}
			else
				IORegister[8]++;
			continue;
		}

		IORegister[5] = 0;

		if (irq2inMem[irq2c] <= IORegister[8])
		{
			IORegister[5] = 1;
			irq2c++;
		}
	

		int op, rd, rs, rt, imm, bigmim = 0;

		int prev_pc = pc; // שמור את ה-PC לפני פענוח ההוראה

		regAssign(&op, &rd, &rs, &rt, &imm, &bigmim);


		trace(argv[6]);

		int cyc = IORegister[8];

		if (instructionOp(op, rd, rs, rt, bigmim) == 1) {

			break; // HALT
		}
		if (!(((IORegister[0] && IORegister[3]) || (IORegister[1] && IORegister[4]) || (IORegister[2] && IORegister[5])) && interaptMode))
		{
			if (IMEM[pc][5] == '1') {
				IORegister[8] = IORegister[8] + 2;
			}
			else
				IORegister[8]++;
		}
		
		// hwregtrace - מעקב אחר קריאות/כתיבות לרגיסטר חומרה
		if (op == 19 || op == 20)
		{
			createHwregtrace(argv[7], op, registerMemory[rs] + registerMemory[rt], cyc);
		}

		// עדכון קובץ LEDs
		if (op == 20 && (registerMemory[rs] + registerMemory[rt]) == 9)
		{
			createLeds(argv[9],cyc);
		}

		// עדכון תצוגת 7seg
		if (op == 20 && (registerMemory[rs] + registerMemory[rt]) == 10)
		{
			createDisplay7Seg(argv[10]);
		}

		// עדכון טיימר
		if (IORegister[11] == 1)
		{
			if (IORegister[12] == IORegister[13])
			{
				IORegister[3] = 1;
				IORegister[12] = 0;
			}
			else
			{
				IORegister[12]++;
			}
		}

		// איפוס monitorcmd
		IORegister[22] = 0;
	}
	IORegister[8]++;// the last cyc dont write in the while so we add one 
	createRegout(argv[5]);
	printf("write to %s\n", argv[5]);
	createCycles(argv[8]);
	printf("write to %s\n", argv[8]);
	createMonitor(argv[12], argv[13]);
	printf("write to %s   %s\n", argv[12],argv[13]);
	createMemout(argv[4]);
	printf("write to %s\n", argv[4]);
	createDISKOUT(argv[11]);
	printf("write to %s\n", argv[11]);
	printf("Done");
	return 0;

}
int openMEMIN(char* MEMIN) {
	FILE* memin = fopen(MEMIN, "r");
	if (memin == NULL) {
		printf("Failed to open MEMIN\n");
		exit(1);
	}
	char line[10]; // 8 תווים + \n + \0
	for (int i = 0; i < 4096; i++) {
		if (fgets(line, 10, memin) == NULL) {
			// אם הקובץ נגמר, מלא את שאר ה-IMEM ב-"00000000"
			strcpy(IMEM[i], "00000000");
			continue;
		}
		// וודא שהשורה באורך 8 תווים (ללא \n)
		line[8] = '\0'; // חותך את התווים מעבר ל-8
		if (strlen(line) < 8) {
			// אם השורה קצרה, הוסף אפסים מצד שמאל
			char padded[9] = "00000000";
			strcpy(padded + (8 - strlen(line)), line);
			strcpy(IMEM[i], padded);
		}
		else {
			strcpy(IMEM[i], line);
		}
	}
	fclose(memin);
	return 0;
}

int openDISKIN(char* diskinF)

{
	FILE* diskin = NULL;
	diskin = fopen(diskinF, "r");
	if (diskin == NULL)
	{
		printf("failed\n");
		exit(1);
	}
	int i = 0;
	while (1)
	{
		fgets(diskArr[i], 10, diskin);
		diskArr[i][8] = '\0';
		i++;
		if (feof(diskin)) break;
	}
	diskArr[i][0] = '\0';
	fclose(diskin);
	return 0;
}

int openIrq2n(char* irq2inF)

{

	int i;

	for (i = 0; i < 4096; i++)

		irq2inMem[i] = -1;

	FILE* irqin = NULL;

	int* irq_arr = irq2inMem;

	irqin = fopen(irq2inF, "r");

	if (irqin == NULL)

	{

		printf("failed to open Irq2in\n");

		exit(1);

	}

	while (1)

	{

		if (feof(irqin))break;

		fscanf(irqin, "%u", irq_arr);

		irq_arr++;

	}

	fclose(irqin);

	return 0;

}

int openFile(char* File) {

	FILE* f = NULL;

	f = fopen(File, "w");

	if (f == NULL) exit(1);

	fclose(f);

	return 0;

}

int regAssign(int* op, int* rd, int* rs, int* rt, int* imm, int* big) {
	// פורמט ההוראה לפי 2025:
	// תא ראשון: | opcode (8) | rd (4) | rs (4) | rt (4) | reserved (3) | bigimm (1) | imm8 (8) |
	// תא שני (אם bigimm=1): | imm32 (32) |

	char opC[3] = { IMEM[pc][0], IMEM[pc][1], '\0' };        // בייטים 0-1 (opcode)
	char rdC[2] = { IMEM[pc][2], '\0' };                    // בייט 2 (rd)
	char rsC[2] = { IMEM[pc][3], '\0' };                   // בייט 3 (rs)
	char rtC[2] = { IMEM[pc][4], '\0' };                  // בייט 4 (rt)
	char reservedC[2] = { IMEM[pc][5], '\0' };           // בייט 5 (reserved + bigimm)
	char imm8C[3] = { IMEM[pc][6], IMEM[pc][7], '\0' }; // בייטים  6 - 7 (imm8)

	// פענוח השדות
	*op = (int)strtol(opC, NULL, 16);
	*rd = (int)strtol(rdC, NULL, 16);
	*rs = (int)strtol(rsC, NULL, 16);
	*rt = (int)strtol(rtC, NULL, 16);

	// פענוח bigimm (הביט האחרון ב-reservedC)
	int bigimm = (reservedC[0] - '0') & 0x1; // לקחת רק את הביט האחרון
	if (bigimm) {
		// הוראה עם 2 תאים - לקרוא את התא השני (imm32)
		char imm32C[9];
		strncpy(imm32C, IMEM[pc + 1], 8);
		imm32C[8] = '\0';
		*imm = (int)strtol(imm32C, NULL, 16);
	
		*big = 1 ;
	}
	else {
		// הוראה עם תא בודד - להשתמש ב-imm8
		*imm = (int)strtol(imm8C, NULL, 16);
		// הרחבת סימן ל-32 ביט
		if (*imm & 0x80) { // אם הביט העליון ב-imm8 מוגדר
			*imm |= 0xFFFFFF00; // הרחבת סימן
		}
		
		*big = 0 ;
	}

	registerMemory[1] = *imm; // עדכון $imm
	return 0;
}

int trace(char* argv_7) {
	FILE* traceF = fopen(argv_7, "a");
	if (traceF == NULL) exit(1);

	

	// כתיבת הנתונים
	fprintf(traceF, "%08X %03X %.8s %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X\n",
		IORegister[8],  // CYCLE (8 ספרות הקסדצימלי)
		pc,             // PC (3 ספרות הקסדצימלי)
		IMEM[pc],       // INST (8 תווים)
		registerMemory[0], registerMemory[1], registerMemory[2],
		registerMemory[3], registerMemory[4], registerMemory[5],
		registerMemory[6], registerMemory[7], registerMemory[8],
		registerMemory[9], registerMemory[10], registerMemory[11],
		registerMemory[12], registerMemory[13], registerMemory[14],
		registerMemory[15]);

	fclose(traceF);
	return 0;
}

int instructionOp(int op, int rd, int rs, int rt, int big) {
	

	if (op == 0) {

		addF(rd, rs, rt, big);

		return 0;

	}

	if (op == 1) {

		subF(rd, rs, rt, big);

		return 0;

	}

	if (op == 2) {

		mulF(rd, rs, rt, big);

		return 0;

	}

	if (op == 3) {

		andF(rd, rs, rt, big);

		return 0;

	}

	if (op == 4) {

		orF(rd, rs, rt, big);

		return 0;

	}

	if (op == 5) {

		xorF(rd, rs, rt, big);

		return 0;

	}

	if (op == 6) {

		sllF(rd, rs, rt, big);

		return 0;

	}

	if (op == 7) {

		sraF(rd, rs, rt, big);

		return 0;

	}

	if (op == 8) {

		srlF(rd, rs, rt, big);

		return 0;

	}

	if (op == 9) {

		beqF(rd, rs, rt, big);

		return 0;

	}

	if (op == 10) {

		bneF(rd, rs, rt, big);

		return 0;

	}

	if (op == 11) {

		bltF(rd, rs, rt, big);

		return 0;

	}

	if (op == 12) {

		bgtF(rd, rs, rt, big);

		return 0;

	}

	if (op == 13) {

		bleF(rd, rs, rt, big);

		return 0;

	}

	if (op == 14) {

		bgeF(rd, rs, rt, big);

		return 0;

	}

	if (op == 15) {

		jalF(rd, rs, rt, big);

		return 0;

	}

	if (op == 16) {

		lwF(rd, rs, rt, big);

		return 0;

	}

	if (op == 17) {

		swF(rd, rs, rt, big);

		return 0;

	}

	if (op == 18) {

		retiF(rd, rs, rt, big);

		return 0;

	}

	if (op == 19) {

		inF(rd, rs, rt, big);

		return 0;

	}

	if (op == 20) {

		outF(rd, rs, rt, big);

		return 0;

	}

	if (op == 21) {

		return 1;

	}


}

int addF(int rd, int rs, int rt, int big) {

	if (rd >= 2) {

		registerMemory[rd] = registerMemory[rs] + registerMemory[rt];

	}

	else
		printf("cant save here add\n");
	if (big)
		pc = pc + 2;
	else
		pc++;
	

	return 0;

}

int subF(int rd, int rs, int rt, int big) {

	if (rd >= 2) {

		registerMemory[rd] = registerMemory[rs] - registerMemory[rt];

	}

	else

		printf("cant save here sub\n");
	if (big)
		pc = pc + 2;
	else
		pc++;

	

	return 0;

}

int mulF(int rd, int rs, int rt, int big) {

	if (rd >= 2) {

		registerMemory[rd] = (registerMemory[rs] * registerMemory[rt]);

	}

	else

		printf("cant save here mac\n");
	if (big)
		pc = pc + 2;
	else
		pc++;

	

	return 0;

}

int andF(int rd, int rs, int rt, int big) {

	if (rd >= 2) {

		registerMemory[rd] = registerMemory[rs] & registerMemory[rt];

	}

	else

		printf("cant save here and\n");
	if (big)
		pc = pc + 2;
	else
		pc++;

	
	return 0;

}

int orF(int rd, int rs, int rt, int big) {

	if (rd >= 2) {

		registerMemory[rd] = registerMemory[rs] | registerMemory[rt];

	}

	else

		printf("cant save here or\n");
	if (big)
		pc = pc + 2;
	else
		pc++;

	

	return 0;

}

int xorF(int rd, int rs, int rt, int big) {

	if (rd >= 2) {

		registerMemory[rd] = registerMemory[rs] ^ registerMemory[rt];

	}

	else

		printf("cant save here xor\n");
	if (big)
		pc = pc + 2;
	else
		pc++;

	

	return 0;

}

int sllF(int rd, int rs, int rt, int big) {

	if (rd >= 2) {

		registerMemory[rd] = registerMemory[rs] << registerMemory[rt];

	}

	else

		printf("cant save here sll\n");
	if (big)
		pc = pc + 2;
	else
		pc++;

	

	return 0;

}

int sraF(int rd, int rs, int rt, int big) {

	if (rd >= 2) {

		int MSB = (0x80000000 & registerMemory[rs]) >> 31;

		int mask = (MSB * 0xFFFFFFFF) << (32 - registerMemory[rt]);

		registerMemory[rd] = (registerMemory[rs] >> registerMemory[rt]) | mask;

	}

	else

		printf("cant save here sra\n");
	if (big)
		pc = pc + 2;
	else
		pc++;

	
	return 0;

}

int srlF(int rd, int rs, int rt, int big) {

	if (rd >= 2) {

		registerMemory[rd] = registerMemory[rs] >> registerMemory[rt];

	}

	else

		printf("cant save here srl\n");
	if (big)
		pc = pc + 2;
	else
		pc++;

	

	return 0;

}

int beqF(int rd, int rs, int rt, int big) {

	if (registerMemory[rs] == registerMemory[rt]) {

		pc = registerMemory[rd];

	}

	else
	{
		if (big)
			pc = pc + 2;
		else
			pc++;
	}

		

	return 0;

}

int bneF(int rd, int rs, int rt, int big) {

	if (registerMemory[rs] != registerMemory[rt])

		pc = registerMemory[rd];

	else{
		if (big)
			pc = pc + 2;
		else
			pc++;
	}

		

	return 0;

}

int bltF(int rd, int rs, int rt, int big) {

	if (registerMemory[rs] < registerMemory[rt])

		pc = registerMemory[rd];

	else{
		if (big)
			pc = pc + 2;
		else
			pc++;
	}

		

	return 0;

}

int bgtF(int rd, int rs, int rt, int big) {

	if (registerMemory[rs] > registerMemory[rt])

		pc = registerMemory[rd];


	else{
		if (big)
			pc = pc + 2;
		else
			pc++;
	}

		

	return 0;

}

int bleF(int rd, int rs, int rt, int big) {

	if (registerMemory[rs] <= registerMemory[rt])
		pc = registerMemory[rd];

	else{
		if (big)
			pc = pc + 2;
		else
			pc++;
	}

		

	return 0;

}

int bgeF(int rd, int rs, int rt, int big) {

	if (registerMemory[rs] >= registerMemory[rt])

		pc = registerMemory[rd];

	else{
		if (big)
			pc = pc + 2;
		else
			pc++;
	}

	

	return 0;

}

int jalF(int rd, int rs, int rt, int big) {

	if (rd >= 2) {
		if (big)
			registerMemory[rd] = pc + 2;
		else
			registerMemory[rd] = pc + 1;

		pc = registerMemory[rs];
		
	}

	else {

		

		printf("cant save here jal\n");

	}

	return 0;

}

int lwF(int rd, int rs, int rt, int big) {

	if (rd >= 2)
	{
		int address = 0;
		from_char_hex_to_int(IMEM[registerMemory[rs] + registerMemory[rt]], &address);
		registerMemory[rd] = address;
	}

	else

		printf("cant save here lw\n");
	if (big)
		pc = pc + 2;
	else
		pc++;

	

	return 0;

}

int swF(int rd, int rs, int rt, int big) {
	if (rd >= 2)
	{
		int x = registerMemory[rd];
		char hex[9];
		sprintf(hex, "%08X", x);
		hex[8] = '\0';
		strcpy(IMEM[registerMemory[rs] + registerMemory[rt]], hex);

	}
	if (big)
		pc = pc + 2;
	else
		pc++;



	return 0;

}

int retiF(int rd, int rs, int rt, int big) {

	pc = IORegister[7];

	interaptMode = 1;

	IORegister[3] = 0;

	IORegister[4] = 0;

	IORegister[5] = 0;

	return 0;

}

int inF(int rd, int rs, int rt, int big) {

	if (registerMemory[rs] + registerMemory[rt] == 22)
		registerMemory[rd] = 0;

	if (rd > 2)

		registerMemory[rd] = IORegister[registerMemory[rs] + registerMemory[rt]];

	else

		printf("cant save here in\n");
	if (big)
		pc = pc + 2;
	else
		pc++;



	

	return 0;

}

int outF(int rd, int rs, int rt, int big) {

	if (((registerMemory[rs] + registerMemory[rt]) == 22) && (registerMemory[rd] == 1))

		//if monitorcmd is triggered and registerMemory[rd]=1

	{

		frameBuffer[IORegister[20] / 256][IORegister[20] % 256] = IORegister[21];

		IORegister[registerMemory[rs] + registerMemory[rt]] = registerMemory[rd];
		f++;

	}

	else if (registerMemory[rs] + registerMemory[rt] == 14)//if diskcmd triggered 

	{

		if (IORegister[17] == 0) {//if free

			IORegister[registerMemory[rs] + registerMemory[rt]] = registerMemory[rd];

			IORegister[17] = 1;//change to busy

			if (registerMemory[rd] == 1)//if diskcmd=1 read

				readDisk();

			if (registerMemory[rd] == 2)//if diskcmd=2 write

				writeDisk();

		}

	}

	else {

		IORegister[registerMemory[rs] + registerMemory[rt]] = registerMemory[rd];

	}
	if (big)
		pc = pc + 2;
	else
		pc++;

	

	return 0;

}

int readDisk() {

	int sectorAdd = 128 * IORegister[15];

	int bufferAdd = IORegister[16];

	int i;

	int j = bufferAdd;

	int limit = sectorAdd + 127;

	for (i = sectorAdd; i < limit; i++) {

		if (j > 4095)

			break;

		strcpy(IMEM[j], diskArr[i]);

		//	strcpy(IMEM[j] ,strtol(diskArr[i], NULL, 16));

		j++;

	}

	return 0;

}

int writeDisk() {

	int sectorAdd = 128 * IORegister[15];

	int bufferAdd = IORegister[16];

	int i;

	int j = bufferAdd;

	int limit = sectorAdd + 127;

	for (i = sectorAdd; i < limit; i++) {

		if (j > 4095)

			break;

		strcpy(diskArr[i], IMEM[j]);



		j++;

	}

	return 0;

}

int createHwregtrace(char* hw, int op, int addr, int c)
{
	hwregtrace = fopen(hw, "a");

	if (hwregtrace == NULL) exit(1);

	if (op == 19) {//if in triggered

		if (addr == 0)

			fprintf(hwregtrace, "%08X READ irq0enable %08X\n", c, IORegister[addr]);

		if (addr == 1)

			fprintf(hwregtrace, "%08X READ irq1enable %08X\n", c, IORegister[addr]);

		if (addr == 2)

			fprintf(hwregtrace, "%08X READ irq2enable %08X\n", c, IORegister[addr]);

		if (addr == 3)

			fprintf(hwregtrace, "%08X READ irq0status %08X\n", c, IORegister[addr]);

		if (addr == 4)

			fprintf(hwregtrace, "%08X READ irq1status %08X\n", c, IORegister[addr]);

		if (addr == 5)

			fprintf(hwregtrace, "%08X READ irq2status %08X\n", c, IORegister[addr]);

		if (addr == 6)

			fprintf(hwregtrace, "%08X READ irqhandler %08X\n", c, IORegister[addr]);

		if (addr == 7)

			fprintf(hwregtrace, "%08X READ irqreturn %08X\n", c, IORegister[addr]);

		if (addr == 8)

			fprintf(hwregtrace, "%08X READ clks %08X\n", c, IORegister[addr]);

		if (addr == 9)

			fprintf(hwregtrace, "%08X READ leds %08X\n", c, IORegister[addr]);

		if (addr == 10)

			fprintf(hwregtrace, "%08X READ display7seg %08X\n", c, IORegister[addr]);

		if (addr == 11)

			fprintf(hwregtrace, "%08X READ timerenable %08X\n", c, IORegister[addr]);

		if (addr == 12)

			fprintf(hwregtrace, "%08X READ timercurrent %08X\n", c, IORegister[addr]);

		if (addr == 13)

			fprintf(hwregtrace, "%08X READ timermaX %08X\n", c, IORegister[addr]);

		if (addr == 14)

			fprintf(hwregtrace, "%08X READ diskcmd %08X\n", c, IORegister[addr]);

		if (addr == 15)

			fprintf(hwregtrace, "%08X READ disksector %08X\n", c, IORegister[addr]);

		if (addr == 16)

			fprintf(hwregtrace, "%08X READ diskbuffer %08X\n", c, IORegister[addr]);

		if (addr == 17)

			fprintf(hwregtrace, "%08X READ diskstatus %08X\n", c, IORegister[addr]);

		if (addr == 18)

			fprintf(hwregtrace, "%08X READ reserved %08X\n", c, IORegister[addr]);

		if (addr == 19)

			fprintf(hwregtrace, "%08X READ reserved %08X\n", c, IORegister[addr]);

		if (addr == 20)

			fprintf(hwregtrace, "%08X READ monitoraddr %08X\n", c, IORegister[addr]);

		if (addr == 21)

			fprintf(hwregtrace, "%08X READ monitordata %08X\n", c, IORegister[addr]);

		if (addr == 22)

			fprintf(hwregtrace, "%08X READ monitorcmd %08X\n", c, IORegister[addr]);

	}

	if (op == 20) {//if out triggered

		if (addr == 0)

			fprintf(hwregtrace, "%08X WRITE irq0enable %08X\n", c, IORegister[addr]);


		if (addr == 1)

			fprintf(hwregtrace, "%08X WRITE irq1enable %08X\n", c, IORegister[addr]);

		if (addr == 2)

			fprintf(hwregtrace, "%08X WRITE irq2enable %08X\n", c, IORegister[addr]);

		if (addr == 3)

			fprintf(hwregtrace, "%08X WRITE irq0status %08X\n", c, IORegister[addr]);

		if (addr == 4)

			fprintf(hwregtrace, "%08X WRITE irq1status %08X\n", c, IORegister[addr]);

		if (addr == 5)

			fprintf(hwregtrace, "%08X WRITE irq2status %08X\n", c, IORegister[addr]);

		if (addr == 6)

			fprintf(hwregtrace, "%08X WRITE irqhandler %08X\n", c, IORegister[addr]);

		if (addr == 7)

			fprintf(hwregtrace, "%08X WRITE irqreturn %08X\n", c, IORegister[addr]);

		if (addr == 8)

			fprintf(hwregtrace, "%08X WRITE clks %08X\n", c, IORegister[addr]);

		if (addr == 9)

			fprintf(hwregtrace, "%08X WRITE leds %08X\n", c, IORegister[addr]);

		if (addr == 10)

			fprintf(hwregtrace, "%08X WRITE display7seg %08X\n", c, IORegister[addr]);

		if (addr == 11)

			fprintf(hwregtrace, "%08X WRITE timerenable %08X\n", c, IORegister[addr]);

		if (addr == 12)

			fprintf(hwregtrace, "%08X WRITE timercurrent %08X\n", c, IORegister[addr]);

		if (addr == 13)

			fprintf(hwregtrace, "%08X WRITE timermax %08X\n", c, IORegister[addr]);

		if (addr == 14)

			fprintf(hwregtrace, "%08X WRITE diskcmd %08X\n", c, IORegister[addr]);

		if (addr == 15)

			fprintf(hwregtrace, "%08X WRITE disksector %08X\n", c, IORegister[addr]);

		if (addr == 16)

			fprintf(hwregtrace, "%08X WRITE diskbuffer %08X\n", c, IORegister[addr]);

		if (addr == 17)

			fprintf(hwregtrace, "%08X WRITE diskstatus %08X\n", c, IORegister[addr]);

		if (addr == 18)

			fprintf(hwregtrace, "%08X WRITE reserved %08X\n", c, IORegister[addr]);

		if (addr == 19)

			fprintf(hwregtrace, "%08X WRITE reserved %08X\n", c, IORegister[addr]);

		if (addr == 20)

			fprintf(hwregtrace, "%08X WRITE monitoraddr %08X\n", c, IORegister[addr]);

		if (addr == 21)

			fprintf(hwregtrace, "%08X WRITE monitordata %08X\n", c, IORegister[addr]);

		if (addr == 22)

			fprintf(hwregtrace, "%08X WRITE monitorcmd %08X\n", c, IORegister[addr]);

	}

	fclose(hwregtrace);

	return 0;

}

int createLeds(char* ledsF, int c)

{

	FILE* leds = NULL;

	leds = fopen(ledsF, "a");

	if (leds == NULL) exit(1);

	fprintf(leds, "%08X %08X\n", c , IORegister[9]);

	fclose(leds);

	return 0;

}

int createDisplay7Seg(char* display7segF) {

	FILE* display = NULL;

	display = fopen(display7segF, "a");

	if (display == NULL)

	{

		printf("failed to write in display\n");

		exit(1);

	}

	fprintf(display, "%08X %08X\n", IORegister[8]-1, IORegister[10]);

	fclose(display);

	return 0;

}

int createRegout(char* regoutF) {

	FILE* Regout = NULL;

	Regout = fopen(regoutF, "w");

	if (Regout == NULL)

	{

		printf("failed to write in display\n");

		exit(1);

	}

	int i;

	for (i = 2; i < 16; i++)

		fprintf(Regout, "%08X\n", registerMemory[i]);

	fclose(Regout);

	return 0;

}

int createCycles(char* cyc)

{

	FILE* cycF = NULL;

	cycF = fopen(cyc, "w");

	if (cycF == NULL) exit(1);

	fprintf(cycF, "%08X", IORegister[8]);

	putc('\n', cycF);

	fclose(cycF);

	return 0;

}

int createMonitor(char* monitortxt, char* monitoryuv)

{

	FILE* monitort = NULL;

	FILE* monitory = NULL;

	monitort = fopen(monitortxt, "w");

	monitory = fopen(monitoryuv, "w");

	if (monitort == NULL) exit(1);

	if (monitory == NULL) exit(1);
	int i, j;

	for (i = 0; i < 256 ; i++)

	{

		for (j = 0; j < 256; j++)

		{

			fprintf(monitort, "%02X\n", frameBuffer[i][j]);

			fprintf(monitory, "%c", frameBuffer[i][j]);

		}

	}
	fprintf(monitort, "%s", "FF\n");

	fprintf(monitory, "%s", "FF\n");

	fclose(monitory);

	fclose(monitort);

	return 0;

}

int createDISKOUT(char* diskoutF) {

	FILE* diskout = NULL;

	diskout = fopen(diskoutF, "w");

	if (diskout == NULL) exit(1);

	int n = 16384;

	while ('\0' == diskArr[n][0])

		n--;

	int i = 0;

	while (i < (n + 1))

	{
		if (diskArr[i][0] == '\0')
			fprintf(diskout, "%s\n", "00000000");
		else
			fprintf(diskout, "%s\n", diskArr[i]);

		i++;

	}

	fclose(diskout);

	return 0;

}

int createMemout(char* memoutF) {
	FILE* memout = fopen(memoutF, "w");
	if (memout == NULL) {
		printf("Failed to open memout.txt\n");
		exit(1);
	}

	// מצא את האינדקס האחרון שבו יש תוכן ב-IMEM
	int lastIndex = 4095;
	while (lastIndex >= 0 && strcmp(IMEM[lastIndex], "00000000") == 0) {
		lastIndex--;
	}

	// כתוב רק עד האינדקס האחרון שבו יש תוכן
	for (int i = 0; i <= lastIndex; i++) {
		fprintf(memout, "%.8s\n", IMEM[i]);
	}


	fclose(memout);
	return 0;
}

int from_char_hex_to_deci(char hex)
{
	if ('0' <= hex && hex <= '9')
		return hex - '0';
	else if ('A' <= hex && hex <= 'F')
	{
		return 10 + (hex - 'A');
	}
	else
	{
		return 10 + (hex - 'a');
	}
}

void from_char_hex_to_int(char* data, int* address)
{
	int sign = 1;
	*address = from_char_hex_to_deci(*data);
	data++;
	while (*data != ' ' && *data != '\t' && *data != '\0' && *data != '\n')
	{
		if (*data == '-')
		{
			sign = -1;
			data++;
			continue;
		}
		*address = (*address) * 16;
		*address = *address + from_char_hex_to_deci(*data);
		data++;
	}
	*address = sign * (*address);

}










