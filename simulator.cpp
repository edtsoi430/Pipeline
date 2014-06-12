//Ryan Kuhl
//CDA3101
//Project 1
//LC3101
//
//The following code is heavily based on code provided in the write-up for
//LC3101 by Dr. Tyson.

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define NUMMEMORY 0x10000 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000
#define BINARYLENGTH 24

#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define CMOV 5
#define HALT 6
#define NOOP 7

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType, *ptrState;

/*typedef struct DecodedNum{
    int opcode;
    int regA;
    int regB;
    int destReg;
    int offsetField;
}DecodedNum;
*/
void printState(stateType *);	//ptrState?
stateType run (stateStruct state);
int extendNum(int num);	//SUN INT
//DecodedNum decoder(DecodedNum DCD, int temp);

int count = 0;

int main(int argc, char *argv[])
{
    FILE *filePtr;
    char binary[33], line[MAXLINELENGTH];
    int isNum = 0, temp = 0, i = 0;// opcode = 0, regB = 0, regA = 0, destReg = 0, offsetField = 0;
    stateStruct state;	//initialize stateStruct
    state.pc = 0;	//initialize the program counter

//    DecodedNum DCD[200];
//    string decoded;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
	exit(1);
    }
    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
	perror("fopen");
	
	exit(1);
    }
    /* read in the entire machine-code file into
     * memory */
    for(int i = 0; i < 0x7FFF; i++){
	state.mem[i]=0;
    }
    for(int i = 0; i < NUMREGS; i++){
	state.reg[i] = 0;
    }

    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
	if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
		printf("error in reading address %d\n", state.numMemory);
		exit(1);
	}
//    printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
    rewind(filePtr);
//    	int j = 0;
/*    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
	temp = state.mem[state.numMemory];
	DCD[j] = decoder(DCD[j], temp);
	printf("\n----------------------------------------------------------------");
	printf("--------------------\n");
	printf("opcode: %d\nregA: %d\nregB: %d\ndestField: %d\noffsetField: %d\n",
	      DCD[j].opcode, DCD[j].regA, DCD[j].regB, DCD[j].destReg, DCD[j].offsetField);
	j++;
    }*/
    state = run(state);	    //send state into the run function
    printState(&state);	    //send state reference into the print function
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
    fflush(stdout);
}

stateType run (stateType state){
    int opcode, arg0, arg1, arg2, offsetField, _pc, temp; //arg2 = destReg on writeup
    for(; 1; count++){
	count ++;
	//This parsing function and some portions of the state modifying
	//functions were inspired by public code by author yeung on
	//ftp://ftp.heanet.ie/mirrors/sourceforge/p/pi/pipelineproject/5StageProject/mips-small.c.
	opcode = (state.mem[_pc] >> 22);
	printf("\nOPCODE: %d\n", opcode);
	arg0 = (state.mem[_pc] >> 19) & 0x7;
	printf("ARG0: %d\n", arg0);
	arg1 = (state.mem[_pc] >> 16) & 0x7;
	printf("ARG1: %d\n", arg1);
	arg2 = (state.mem[_pc]) & 0x7;
	printf("ARG2: %d\n", arg2);
	offsetField = extendNum(state.mem[_pc]) & 0xFFFF;
	if(offsetField >= 32767){
	    offsetField = extendNum(offsetField);
	}
	printf("OFFSET: %d\n", offsetField);
        _pc++;    // program counter is going ahead one command (+4)

	++state.pc;
	if(opcode == ADD){    //add
	    state.reg[arg2] = state.reg[arg0] + state.reg[arg1];    //add together registers, store in arg2
	    printf("\nADD");
	}
	else if (opcode == NAND){
	    state.reg[arg2] = ~(0xFFFFFFFF - state.reg[arg0]) & (0xFFFFFFFF - state.reg[arg1]);	    //performs NAND operation c = ~(a&b), bitwise
	    printf("\nNAND");
	}
	else if (opcode == LW){
	    state.reg[arg1] = state.mem[(state.reg[arg0] + offsetField)];	    //load memory address of arg0 + offset, store in reg location arg1
	    printf("\nLW");
	}
	else if (opcode == SW){
	    state.mem[(state.reg[arg0] + offsetField)] = state.reg[arg1];	    //opposite of LW
	    printf("\nSW");
	}
	else if (opcode == BEQ){
	    printf("\nBEQ");
	    if(state.reg[arg0] == state.reg[arg1])
		_pc = _pc + offsetField;
	}
	else if (opcode == CMOV){
	    printf("\nCOMV");
	    if(state.reg[arg1] != 0)
		state.reg[arg2] = state.reg[arg0];
	}
	else if (opcode == NOOP){
	    printf("\nNOOP");
	}
	else if (opcode == HALT){
	    printf("\nHALT");
	    printState(&state);
	    exit(0);
	}
	else{ 
	    printf("error: usage: %x is an undefined command\n", opcode);
	    exit(1);
	}
    printState(&state);
    }

    state.reg[0] = 0;
    return state;

}
int extendNum(int num){
    /*convert a 16-bit number into a 32 bit Sun integer */
    if (num & (1<<15)){
	num -= (1<<16);
    }
    return (num);
}
/*string toBinary(int isNum){
	int remainder;	    //desired size for the int
	string final = ""; 
	string temp;
	const string z = "0";
	long long chk = 1;
	bool isNeg = false;
	printf("in toBinary\n");
	for(int i = 0; i < BINARYLENGTH; ++i)
	   chk = chk*2;
	if(isNum > chk || isNum < (chk * -1)-1){
	    printf("%d will not fit in bitstring\n", isNum);
	    exit(1);
	}
	if(isNum < 0){
	    isNeg = true;
	    isNum =  (isNum * -1);
	}
	while(isNum > 0){
	    if(isNum%2)
		final = '1' + final;
	    else
		final = '0' + final;
	}
	while(final.size() < BINARYLENGTH)
	    final.insert(0, z);
	if(isNeg == true){
	    for( int i = 0; i < final.size(); i++){
		if(final[i] == '0')
		    final[i] == '1';
		else if(final[i] == '1')
		    final[i] == '0';

	    }
	    final[0] = 1;
	    temp = BiOne(final);
	    final = sprintf(final, temp);
	    return final;
	}
}
*/
/*
string BiOne(string final){
    const string one = "1";
    const string zero = "0";
    bool carry = false;
	printf("in biOne\n");
	if(final[final.size()] == '0'){
	    final[final.size()] == '1';
	}
	else if(final[final.size()]=='1'){
	    carry = true;
	    for(int i = final.size(); i >=0 ; i--){
		if(final[i] == '1' && carry == true){
		    final[i] == '0';
		    carry = true;
		}
		else if(final[i] == '1' && carry == false){
		    final[i] = '1';
		    carry = false;
		}
		else if(final[i] == '0' && carry == true){
		    final[i] = '1';
		    carry = false;
		}
		else if(final[i] == '0' && carry == false){
		    final[i] = '0';
		    carry = false;
		}
	    }
	}
	return final;
}
*/
/*DecodedNum decoder(DecodedNum DCD, int temp){
    int opcode, regB, regA, destReg, offsetField;
	//printf("Final: %s\n\n", final);
	DCD.opcode = temp >> 22;

	if(DCD.opcode == 0 || DCD.opcode == 1 || DCD.opcode == 5){	    //add, Rtype
	    DCD.regA = temp & 0x1C0000 >> 19; 
	    DCD.regB = temp & 0x38000 >> 16;
	    DCD.destReg = temp & 0x3 >> 0;
	    DCD.offsetField = 0;
	}
	else if(DCD.opcode == 1){	    //nand, Rtype
	    //printf("in add\n");
	    DCD.regA = temp & 0x1C0000 >> 19; 
	    DCD.regB = temp & 0x38000 >> 16;
	    DCD.destReg = temp & 0x7FFF >> 0;
	    DCD.offsetField = 0;
	}
	else if(DCD.opcode ==2 || DCD.opcode == 3 || DCD.opcode == 4){	    //lw, Itype
//	    printf("in lw\n");
	    printf("%d\n", temp);
	    DCD.regA = temp & 0x1C0000 >> 19;
	    DCD.regB = temp & 0x38000 >> 16;
	    DCD.destReg = 0;
	    DCD.offsetField = temp & 0x7FFF >> 0;   
	}
	else if(DCD.opcode == 3){	    //sw, Itype
//	    printf("in sw\n");
	    DCD.regA = temp >> 19;
	    DCD.regB = temp >> 16;
	    DCD.destReg = 0;
	    DCD.offsetField = temp >> 0;   
	}
	else if(DCD.opcode == 4){	    //beq, Itype
//	    printf("in beq\n");
	    DCD.regA = temp >> 19;
	    DCD.regB = temp >> 16;
	    DCD.destReg = 0;
	    DCD.offsetField = temp >> 0;   
	}
	else if(DCD.opcode == 5){	    //cmov, Rtype
//	    printf("in cmov\n");
	    DCD.regA = temp >> 19;
	    DCD.regB = temp >> 16;
	    DCD.destReg = temp >> 0;
	    DCD.offsetField = 0;
	}
	else if(DCD.opcode == 6){	    //halt, Otype
	    printf("in halt\n");

	}
	else if(DCD.opcode == 7){	    //noop, Otype
	    printf("in noop\n");
	}
	return DCD;
}
*/
