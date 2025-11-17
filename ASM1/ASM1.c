#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include<string.h>


#define MAX_LINE_SIZE 500
#define MAX_LABEL_SIZE 50
#define MEMORY_SIZE 4096
#define MAX_INST_NUM 4096


int MEM[MEMORY_SIZE] = { 0 };
char IMEM[MAX_INST_NUM][32];
int is_there_a_label_in_imm[MAX_INST_NUM] = { 0 };
char label_in_imm[MAX_INST_NUM][MAX_LABEL_SIZE];
char labels_names[MAX_INST_NUM][MAX_LABEL_SIZE];
int pc_of_labels[MAX_INST_NUM] = { 0 };
int pc = 0;
int max_pc = 0;
int labels_counter = 0;



void removeComment(char line[]);
int lineType(char line[]);
void get_label(char line[]);
void get_label_and_inst(char line[]);
int imm_is_a_label(char line[]);
void get_imm(char line[], char* imm);
void DecToHex(char* number, char* dec, int n);
void get_opcode(char line[], char* opcode);
void replaceReg(char* token, char* reg);
void put_data_in_memory(char line[]);
void fill_pc_of_labels();
void write_memin(char* argv);
void put_imm_in_arr(char line[], int n);
int from_char_hex_to_deci(char hex);
void get_inst(char line[]);
void get_rd(char line[], char* reg);
void get_rs(char line[], char* reg);
void get_rt(char line[], char* reg);
void from_char_hex_to_int(char* data, int* address);
int from_char_to_deci(char dec);
void from_char_deci_to_int(char* data, int* address);


int main(int argc, char* argv[])
{
	for (int i = 0; i < MEMORY_SIZE; i++) { MEM[i] = -1; }
	FILE* program = NULL;
	program = fopen(argv[1], "r");
	if (program == NULL)
	{
		printf("can't open the file %s",argv[1]);
		exit(1);
	}
	char line[MAX_LINE_SIZE];
	int line_type = 0;
	while (1)
	{
		line_type = 0;
		if (feof(program)) break;
		fgets(line, MAX_LINE_SIZE, program);
		removeComment(line);
		line_type = lineType(line);
		if (line_type == 0)
		{
			exit(1);
		}
		else if (line_type == 1)// label line
		{
			get_label(line);
			continue;
		}
		else if (line_type == 2)// empty line
		{
			continue;
		}
		else if (line_type == 3)//  label and an instruction line
		{
			get_label_and_inst(line);
		}
		else if (line_type == 4)//  instruction line
		{
			get_inst(line);
		}
		else if (line_type == 5)// .word line 
		{
			put_data_in_memory(line);
			continue;
		}

		
		 pc++;

	}
	fill_pc_of_labels();
	fclose(program);
	write_memin(argv[2]);
	printf("Done");

	return 0;

}

void removeComment(char line[])
{
	char L = line[0];
	int i;
	for (i = 1; i < MAX_LINE_SIZE; i++)
	{
		if (L != '#')
		{
			L = line[i];
		}
		else
			break;
	}
	line[i - 1] = '\0';

}

int lineType(char line[])
{
	char* comma = ",";
	char* dots = ":";
	if (strstr(line, dots) != NULL && strstr(line, comma) == NULL)// label
		return 1;
	if (strstr(line, dots) == NULL && strstr(line, ",") == NULL && strstr(line, "$") == NULL && strstr(line, ".word") == NULL)// empty line
		return 2;
	if (strstr(line, dots) != NULL && strstr(line, comma) != NULL)// instruction and a label
		return 3;
	if (strstr(line, dots) == NULL && strstr(line, comma) != NULL)// instruction
		return 4;
	if (strstr(line, ".word") != NULL)// .word line
		return 5;
	return 0;//error
}

void get_label(char line[])
{
	int i = 0;
	while (*line != ':')
	{
		if (*line == ' ' || *line == '\t') { line++; continue; }
		labels_names[labels_counter][i] = *line;
		i++;
		line++;

	}
	pc_of_labels[labels_counter] = pc;
	labels_counter++;
}

void get_label_and_inst(char line[])
{
	char label[MAX_LABEL_SIZE];
	char* inst = strstr(*line, ':') + 1;
	int i = 0;
	while (line[i] != ':')
	{
		label[i] = line[i];
		i++;
	}
	label[i] = ':';
	i++;
	label[i] = '\0';
	get_label(label);
	get_inst(inst);
}

void get_inst(char line[])
{
	char opcode[8];
	get_opcode(line, opcode);
	char rd[4];
	get_rd(line, rd);
	char rs[4];
	get_rs(line, rs);
	char rt[4];
	get_rt(line, rt);
	
	
	int is_label = imm_is_a_label(line);
	char imm8[8];
	char imm32[32];
	int big = get_bigimm(line);
	char bigimm[4];
	snprintf(bigimm, sizeof(bigimm), "%d", big);
	if (big || is_label) {

		if (!is_label) get_imm32(line, imm32);
		else
		{
			is_there_a_label_in_imm[pc] = 1;
			put_imm_in_arr(line, 1);
			char* c = label_in_imm[pc];
			char num[32];
			DecToHex(num, c, 8);
			sprintf(imm32, "%s", num);
		}

		IMEM[pc][0] = opcode[0];
		IMEM[pc][1] = opcode[1];
		IMEM[pc][2] = rd[0];
		IMEM[pc][3] = rs[0];
		IMEM[pc][4] = rt[0];
		IMEM[pc][5] = bigimm[0];
		IMEM[pc][6] = '0';
		IMEM[pc][7] = '0';
		IMEM[pc][8] = '\0';

		if (strstr(line, "$imm") != NULL) { strcpy(IMEM[pc + 1], imm32); }
		pc++;
	}
	else
	{
		get_imm8(line, imm8);
		IMEM[pc][0] = opcode[0];
		IMEM[pc][1] = opcode[1];
		IMEM[pc][2] = rd[0];
		IMEM[pc][3] = rs[0];
		IMEM[pc][4] = rt[0];
		IMEM[pc][5] = bigimm[0];
		IMEM[pc][6] = imm8[0];
		IMEM[pc][7] = imm8[1];
		IMEM[pc][8] = '\0';
	}

}

int imm_is_a_label(char line[]) {
	char c_line[MAX_LINE_SIZE];
	strcpy(c_line, line);
	char* token = NULL;
	token = strtok(c_line, ",");
	int i = 0;
	for (; i < 3; i++)
	{
		token = strtok(NULL, ",");
	}

	while (*token == ' ' || *token == '\t') token++;
	if (*token <= 'z' && *token >= 'a') {
		return 1;
	}
	else if (*token <= 'Z' && *token >= 'A')
		return 1;
	return 0;
}

void get_imm(char line[], char* imm) {
	char c_line[MAX_LINE_SIZE];
	strcpy(c_line, line);
	char* token = NULL;
	token = strtok(c_line, ",");
	int i = 0;
	for (; i < 3; i++)
		token = strtok(NULL, ",");
	DecToHex(imm, token, 5);
}

void DecToHex(char* number, char* dec, int n)
{
	int num = 0;
	sprintf(number, "%s", "00000");
	while (*dec == ' ' || *dec == '\t') dec++;
	if (*dec == '0' && *(dec + 1) == 'x')
	{
		dec += 2;
		char c[20] = { 0 };
		int j = 0;
		while (*dec != ' ' && *dec != '\t' && *dec != '\n' && *dec != '\0') { c[j] = *dec; dec++; j++; }
		sprintf(number, "%05s", c);
		return;
	}
	if (dec[0] == '-') num -= atoi(dec + 1);	//if the number is negative, convert it to negative int.
	else num = atoi(dec);						//converting the string to positive int number
	char m[17] = { '0' };
	char digit = '0';
	char c = 'A';
	for (int i = 0; i < 16; i++) {
		if (i < 10) m[i] = digit++;
		else m[i] = c++;
	}

	int j = 1;
	if (num == 0) {
		while (n >= j) {
			number[n - j] = '0';	//if number=0; we fill the n-hex digits with zeros.
			j++;
		}
	}
	else if (num > 0) {			//when number is positive, convert it to hex value.
		while (num > 0) {
			number[n - j] = m[num % 16];
			j++;
			num /= 16;
		}
		while (n >= j) {
			number[n - j] = '0';
			j++;
		}
	}
	else {
		unsigned int x = num;		//when the number is negative, convert it to 2-s complement hex value.
		while (x > 0) {
			if (n - j < 0) break;
			number[n - j] = m[x % 16];
			j++;
			x /= 16;
		}
		while (n >= j) {
			number[n - j] = '0';
			j++;
		}
	}
	number[n] = '\0';
}

void get_opcode(char line[], char* opcode) {
	char c_line[MAX_LINE_SIZE];
	strcpy(c_line, line);
	char* token = strtok(c_line, "$");
	if (NULL != strstr(token, "add"))
		sprintf(opcode, "%s", "00");
	else if (NULL != strstr(token, "sub"))
		sprintf(opcode, "%s", "01");
	else if (NULL != strstr(token, "mul"))
		sprintf(opcode, "%s", "02");
	else if (NULL != strstr(token, "and"))
		sprintf(opcode, "%s", "03");
	else if (NULL != strstr(token, "or"))
		sprintf(opcode, "%s", "04");
	else if (NULL != strstr(token, "xor"))
		sprintf(opcode, "%s", "05");
	else if (NULL != strstr(token, "sll"))
		sprintf(opcode, "%s", "06");
	else if (NULL != strstr(token, "sra"))
		sprintf(opcode, "%s", "07");
	else if (NULL != strstr(token, "srl"))
		sprintf(opcode, "%s", "08");
	else if (NULL != strstr(token, "beq"))
		sprintf(opcode, "%s", "09");
	else if (NULL != strstr(token, "bne"))
		sprintf(opcode, "%s", "0A");
	else if (NULL != strstr(token, "blt"))
		sprintf(opcode, "%s", "0B");
	else if (NULL != strstr(token, "bgt"))
		sprintf(opcode, "%s", "0C");
	else if (NULL != strstr(token, "ble"))
		sprintf(opcode, "%s", "0D");
	else if (NULL != strstr(token, "bge"))
		sprintf(opcode, "%s", "0E");
	else if (NULL != strstr(token, "jal"))
		sprintf(opcode, "%s", "0F");
	else if (NULL != strstr(token, "lw"))
		sprintf(opcode, "%s", "10");
	else if (NULL != strstr(token, "sw"))
		sprintf(opcode, "%s", "11");
	else if (NULL != strstr(token, "reti"))
		sprintf(opcode, "%s", "12");
	else if (NULL != strstr(token, "in"))
		sprintf(opcode, "%s", "13");
	else if (NULL != strstr(token, "out"))
		sprintf(opcode, "%s", "14");
	else if (NULL != strstr(token, "halt"))
		sprintf(opcode, "%s", "15");
}

void get_rd(char line[], char* reg) {
	char c_line[MAX_LINE_SIZE];
	strcpy(c_line, line);
	char* token = NULL;
	token = strtok(c_line, ",");

	replaceReg(token, reg);
}

void get_rs(char line[], char* reg) {
	char c_line[MAX_LINE_SIZE];
	strcpy(c_line, line);
	char* token = NULL;
	token = strtok(c_line, ",");
	int i = 0;
	for (; i < 1; i++)
		token = strtok(NULL, ",");

	replaceReg(token, reg);

}

void get_rt(char line[], char* reg) {

	char c_line[MAX_LINE_SIZE];
	strcpy(c_line, line);
	char* token = NULL;
	token = strtok(c_line, ",");
	int i = 0;
	for (; i < 2; i++)
		token = strtok(NULL, ",");

	replaceReg(token, reg);
}

int get_bigimm(char line[]) {
	char c_line[MAX_LINE_SIZE];
	strcpy(c_line, line);
	char* token = NULL;

	// מציאת הארגומנט הרביעי (ה-imm)
	token = strtok(c_line, ",");
	for (int i = 0; i < 3; i++) {
		token = strtok(NULL, ",");
		if (token == NULL) {
			return 0; // ברירת מחדל
		}
	}

	// דילוג על רווחים
	while (*token == ' ' || *token == '\t') token++;

	// לייבל תמיד מצריך bigimm = 1
	if (isalpha(*token)) {
		return 1;
	}

	// המרה להקס או עשרוני
	int value = 0;
	if (token[0] == '0' && token[1] == 'x') {
		value = (int)strtol(token, NULL, 16);
	}
	else {
		value = atoi(token);
	}

	// אם לא מתאים ל-8 ביטים עם סימן -> צריך bigimm
	if (value < -128 || value > 127) {
		return 1;
	}

	return 0;
}


int get_imm8(char line[], char* imm8) {
	char c_line[MAX_LINE_SIZE];
	strcpy(c_line, line);
	char* token = NULL;

	// מציאת הקבוע - הפרמטר הרביעי
	token = strtok(c_line, ",");
	for (int i = 0; i < 3; i++) {
		token = strtok(NULL, ",");
		if (token == NULL) {
			strcpy(imm8, "00");
			return;
		}
	}

	// דילוג על רווחים
	while (*token == ' ' || *token == '\t') token++;

	// בדיקה אם זה לייבל (לא חוקי ל-imm8)
	if (isalpha(*token)) {
		fprintf(stderr, "ERROR: Label cannot be used with imm8 (must be bigimm=1)\n");
		exit(1);
	}

	// המרה למספר
	int value;
	if (token[0] == '0' && token[1] == 'x') {
		value = (int)strtol(token, NULL, 16);
	}
	else {
		value = atoi(token);
	}

	// חיתוך ל-8 ביטים עם הרחבת סימן
	if (value < -128 || value > 127) {
		fprintf(stderr, "ERROR: imm8 value out of range (-128 to 127): %d\n", value);
		exit(1);
	}

	// המרה להקסה (שתי ספרות, כולל ערכים שליליים)
	unsigned char imm_byte = (unsigned char)(signed char)value;
	sprintf(imm8, "%02X", imm_byte);

	return 0;
	
}

int get_imm32(char* line, char* imm32) {
	char c_line[MAX_LINE_SIZE];
	strcpy(c_line, line);
	char* token = NULL;

	// מציאת הפרמטר הרביעי (imm)
	token = strtok(c_line, ",");
	for (int i = 0; i < 3; i++) {
		token = strtok(NULL, ",");
		if (token == NULL) {
			strcpy(imm32, "00000000");
			return;
		}
	}

	// דילוג על רווחים
	while (*token == ' ' || *token == '\t') token++;

	// בדיקה אם מדובר בלייבל – לא מטופל כאן, יש לטפל חיצונית
	if (isalpha(*token)) {
		fprintf(stderr, "ERROR: Label detected in get_imm32 (should be handled separately).\n");
		exit(1);
	}

	int value;

	// המרה למספר
	if (token[0] == '0' && token[1] == 'x') {
		value = (int)strtol(token, NULL, 16);
	}
	else {
		value = atoi(token);
	}

	// המרה ל־imm32 בפורמט 8 ספרות הקסא (2's complement אם שלילי)
	unsigned int uvalue = (unsigned int)value;
	sprintf(imm32, "%08X", uvalue);

	return 0;
}


void replaceReg(char* token, char* reg) {
	if (NULL != strstr(token, "$zero"))
		sprintf(reg, "%s", "0");

	if (NULL != strstr(token, "$imm"))
		sprintf(reg, "%s", "1");

	if (NULL != strstr(token, "$v0"))
		sprintf(reg, "%s", "2");

	if (NULL != strstr(token, "$a0"))
		sprintf(reg, "%s", "3");

	if (NULL != strstr(token, "$a1"))
		sprintf(reg, "%s", "4");

	if (NULL != strstr(token, "$a2"))
		sprintf(reg, "%s", "5");

	if (NULL != strstr(token, "$a3"))
		sprintf(reg, "%s", "6");

	if (NULL != strstr(token, "$t0"))
		sprintf(reg, "%s", "7");

	if (NULL != strstr(token, "$t1"))
		sprintf(reg, "%s", "8");

	if (NULL != strstr(token, "$t2"))
		sprintf(reg, "%s", "9");

	if (NULL != strstr(token, "$s0"))
		sprintf(reg, "%s", "A");

	if (NULL != strstr(token, "$s1"))
		sprintf(reg, "%s", "B");

	if (NULL != strstr(token, "$s2"))
		sprintf(reg, "%s", "C");

	if (NULL != strstr(token, "$gp"))
		sprintf(reg, "%s", "D");

	if (NULL != strstr(token, "$sp"))
		sprintf(reg, "%s", "E");

	if (NULL != strstr(token, "$ra"))
		sprintf(reg, "%s", "F");

}

void put_data_in_memory(char line[])
{
	int address = 0;
	int data = 0;
	while (*line == ' ' || *line == '\t') line++;

	if (*line == '.') line += 5;

	while (*line == ' ' || *line == '\t') line++;

	if (*line == '0' && *(line + 1) == 'x')
	{
		line += 2;
		from_char_hex_to_int(line, &address);
	}
	else
	{
		from_char_deci_to_int(line, &address);
	}
	while (*line != ' ' && *line != '\t') line++;
	while (*line == ' ' || *line == '\t') line++;
	if (*line == '0' && *(line + 1) == 'x')
	{
		line += 2;
		from_char_hex_to_int(line, &data);
	}
	else from_char_deci_to_int(line, &data);
	if (address > max_pc) { max_pc = address; }
	MEM[address] = data;
} //

void fill_pc_of_labels()
{
	char imm[32];
	int pc_of_label = -1;
	int i = 0;
	while (i < pc)
	{
		if (is_there_a_label_in_imm[i] == 0)
		{
			i++;
			continue;
		}

		else if (is_there_a_label_in_imm[i] == 1)
		{
			int j = 0;
			int result = strcmp(labels_names[j], label_in_imm[i]);//if equals result=0
			while (result != 0 && j < labels_counter)
			{
				j++;
				result = strcmp(labels_names[j], label_in_imm[i]);
			}
			pc_of_label = pc_of_labels[j];
			sprintf(imm, "%08X", pc_of_label);
			strcpy(IMEM[i + 1], imm);
		}
		//else if (is_there_a_label_in_imm[i] == 2)
		//{
		//	int j = 0;
		//	int result = strcmp(labels_names[j], label_in_imm[i]);//if equals, result=0
		//	while (result != 0 && j < labels_counter)
		//	{
		//		j++;
		//		result = strcmp(labels_names[j], label_in_imm[i]);
		//	}
		//	pc_of_label = pc_of_labels[j];
		//	sprintf(imm, "%05X", pc_of_label);
		//	IMEM[i][9] = imm[0];
		//	IMEM[i][10] = imm[1];
		//	IMEM[i][11] = imm[2];
		//}
		i++;
	}
}

void write_memin(char* argv)
{
	FILE* memin = fopen(argv, "w");
	if (memin == NULL) { exit(1); printf("cant open"); }
	int j = 0;
	while (j < pc)
	{
		fprintf(memin, "%s", IMEM[j]);
		putc('\n', memin);
		j++;
	}
	while (j < max_pc + 1)
	{
		if (MEM[j] == -1)
		{
			fprintf(memin, "%s", "00000000");
		}
		else {
			char number[32];
			char dec[32];
			sprintf(dec, "%d", MEM[j]);
			DecToHex(number, dec, 8);
			fprintf(memin, "%s", number);
		}
		putc('\n', memin);
		j++;
	}

	fclose(memin);
}

void put_imm_in_arr(char line[], int n) {
	char c_line[MAX_LINE_SIZE];
	strcpy(c_line, line);
	char* token = NULL;
	if (n == 1) {
		int i = 0;
		token = strtok(c_line, ",");
		for (; i < 3; i++)
			token = strtok(NULL, ",");
		while (*token == ' ' || *token == '\t') token++;
		int j = 0;
		while (token[j] != ' ' && token[j] != '\t' && token[j] != '\0')
		{
			label_in_imm[pc][j] = token[j];
			j++;
		}
		label_in_imm[pc][j] = '\0';
		char*s=strstr(label_in_imm[pc], "\n");
		if (s != NULL) { *s = '\0'; }
	}
	if (n == 2) {
		int i = 0;
		token = strtok(c_line, ",");
		for (; i < 3; i++)
			token = strtok(NULL, ",");
		while (*token == ' ' || *token == '\t') token++;
		int j = 0;
		while (token[j] != ' ' && token[j] != '\t' && token[j] != '\n' && token[j] != '\0')
		{
			label_in_imm[pc][j] = token[j];
			j++;
		}
		label_in_imm[pc][j] = '\0';
	}

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

int from_char_to_deci(char dec)
{
	if ('0' <= dec && dec <= '9')
		return dec - '0';
}

void from_char_deci_to_int(char* data, int* address)
{
	int sign = 1;
	if (*data == '-')
	{
		sign = -1;
		data++;
	}
	*address = (unsigned int)from_char_to_deci(*data);
	data++;
	while (*data != ' ' && *data != '\t' && *data != '\0' && *data != '\n')
	{
		*address = (unsigned int)*address * 10;
		*address = (unsigned int)*address + from_char_to_deci(*data);
		data++;
	}
	*address = sign * (*address);
}










