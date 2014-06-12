//Ryan Kuhl
//CDA3101 SP'13
//Project 1
//LC3101
//
//The following program is heavily based on code provided in the LC3101 project
//write-up provided by Dr. Tyson. The code has been individualized and adapted
//to the needs of this program.
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;
#define MAXNUMLABELS 65536
#define MAXLINELENGTH 1000
#define MAXLABELLENGTH 7
typedef struct TableIndex{
    char label[MAXLABELLENGTH];
    int labelCount;
}TableIndex;
typedef struct Result{
    int arg0R;
    int arg1R;
    int arg2R;
}Result;
typedef struct Binary{
    int bi;
}Binary;
int convertToBinary(char* opcode);
int convertToDecimal(string binary);
int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH], aTemp[MAXLINELENGTH];
    int labelCount = 0, size = 0, result = 0;
    TableIndex List[200];    
    Result ResultInt[200];
    Binary B[200];
    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");		//going to open the input file
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");		//going to open the output file
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    /* here is an example for how to use readAndParse to read a line from
        inFilePtr */
//  The following error checking code was adapted from code publicaly available
//  on the website http://www.cs.fsu.edu/~tyson/courses/CDA5155/files/a.c by Dr.
//  Tyson
    while ( readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
        /* reached end of file */
	if(strcmp(opcode, "add") && strcmp(opcode, "nand")
		&& strcmp(opcode, "lw") && strcmp(opcode, "sw")
		&& strcmp(opcode, "beq") && strcmp(opcode, "cmov")
		&& strcmp(opcode, "halt") && strcmp(opcode, "noop")
		&& strcmp(opcode, ".fill")){
		   printf("error: incompatible opcode %s\n", opcode);
		   exit(1);
	} 
	if((strcmp(opcode, "halt") && strcmp(opcode, "noop")
		&& strcmp(opcode, ".fill") && arg2[0]=='\0')
		|| (!strcmp(opcode, ".fill") && arg0[0]=='\0')){
	    printf("Not enough arguments\n");
	    exit(2);
	}

	if (label[0] != '\0') {
	    if(strlen(label) >= MAXLABELLENGTH){
		printf("Exceeding Max Label Length\n");
		exit(2);
	    }
	    if(!sscanf(label, "%[a-zA-Z]", aTemp)){
		printf("Label does not start with alpha\n");
		exit(2);
	    }
	    sscanf(label, "%[a-zA-Z0-9]", aTemp);
	    if(strcmp(aTemp, label)){
		printf("Label contains illegal characters\n");
		exit(2);
	    }
	    for(int i=0; i < size; i++){
		if(!strcmp(label, List[i].label)){
		   printf("Duplicate Label\n");
		  exit(1);
		}
	    }
	if(size >= MAXNUMLABELS){
	    printf("Label maximum exceeded\n");
	    exit(2);
	}	    

	    strcpy(List[size].label, label);
	    List[size].labelCount = labelCount;
	    ++size;
	}
	++labelCount;
    }
//	fprintf(outFilePtr, "LIST: \n\n");
        	
    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);
    int counter = 0;
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
	    if(isNumber(arg2) == 0){	    //means arg2 is not a number
	    for(int i = 0; i < labelCount; ++i){
		if(strcmp(arg2, List[i].label)==0)
		{
		    ResultInt[counter].arg0R = atoi(arg0);
		    ResultInt[counter].arg1R = atoi(arg1);
		    ResultInt[counter].arg2R = (List[i].labelCount - counter);
		}
	    }
	    }
	    else if(isNumber(arg2) == 1){
		    ResultInt[counter].arg0R = atoi(arg0);
		    ResultInt[counter].arg1R = atoi(arg1);
		    ResultInt[counter].arg2R = atoi(arg2);
	    }
	    
	    if(!strcmp(opcode, "add"))
		B[counter].bi = ((0 << 22) | (ResultInt[counter].arg0R << 19) 
		    | (ResultInt[counter].arg1R << 16) | (0xFFFF & (ResultInt[counter].arg2R << 0)));
	    if(!strcmp(opcode, "nand"))
		B[counter].bi = ((1 << 22) | (ResultInt[counter].arg0R << 19)
		    | (ResultInt[counter].arg1R << 16) | (0xFFFF & (ResultInt[counter].arg2R << 0)));
	    if(!strcmp(opcode, "cmov"))
		B[counter].bi = ((5 << 22) | (ResultInt[counter].arg0R << 19)
		    | (ResultInt[counter].arg1R << 16) | (0xFFFF & (ResultInt[counter].arg2R << 0)));
	    if(!strcmp(opcode, "lw"))
		B[counter].bi = ((2 << 22) | (ResultInt[counter].arg0R << 19)
		    | (ResultInt[counter].arg1R << 16) | (0xFFFF & (ResultInt[counter].arg2R << 0)));
	    if(!strcmp(opcode, "sw"))
		B[counter].bi = ((3 << 22) | (ResultInt[counter].arg0R << 19)
		    | (ResultInt[counter].arg1R << 16) | (0xFFFF & (ResultInt[counter].arg2R << 0)));
	    if(!strcmp(opcode, "beq"))
		if(isNumber(arg2) == 0){
		B[counter].bi = ((4 << 22) | (ResultInt[counter].arg0R << 19)
		    | (ResultInt[counter].arg1R << 16) | (0xFFFF & (ResultInt[counter].arg2R-1 << 0)));
		}
		else{
		B[counter].bi = ((4 << 22) | (ResultInt[counter].arg0R << 19)
		    | (ResultInt[counter].arg1R << 16) | (0xFFFF & (ResultInt[counter].arg2R << 0)));
		}
	    if(!strcmp(opcode, "halt"))
		B[counter].bi = ((6 << 22));
	    if(!strcmp(opcode, "noop"))
		B[counter].bi = ((7 << 22));
	    if(!strcmp(opcode, ".fill"))
		if(isNumber(arg0) == 0){
		    for(int i = 0; i < labelCount; ++i){
		    if(!strcmp(arg0, List[i].label))
		    {
			B[counter].bi = (List[i].labelCount);
		    }
		    }
		}
		else
		B[counter].bi = ResultInt[counter].arg0R;
	    if(!strcmp(opcode, ""))
	       B[counter].bi = '\0';	
	    ++counter;
    }
    rewind(inFilePtr);
    for(int j = 0; j < counter; j++){	    
	    fprintf(outFilePtr, "%d\n", B[j].bi);
	}
    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
  //  if (!strcmp(opcode, "lw")) {
    //    fprintf(outFilePtr, "010");
//	exit(1);
  //  }

}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;
    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
	printf("error: line too long\n");
	exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */

    sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
        opcode, arg0, arg1, arg2);
    if(!strcmp(opcode, "halt")){
	arg0[0] = '\0';
	arg1[0] = '\0';
	arg2[0] = '\0';
    }
    else if(!strcmp(opcode, "noop")){
	arg0[0] = '\0';
	arg1[0] = '\0';
	arg2[0] = '\0';
    }
    else if(!strcmp(opcode, ".fill")){
	arg1[0] = '\0';
	arg2[0] = '\0';
    }
//   printf("%s\t %s\t %s\t %s\t %s\t \n \n", label, opcode, arg0, arg1, arg2); 
    return(1);
}

int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}
/*void addToTable(char*, int labelCount, int size)
{
    strcpy(Table[size].labelm label);
    Table[size].labelCount = labelCount;
    
}*/
/*
int convertToDecimal(string binary)
{
    string number = binary;
    int result = 0, pow = 1;
    for(int i = number.length()-1; i>=0; i--)
	result += (number[i]-'0')*pow;
return result;
}

int convertToBinary(char* opcode)
{
    string binary;
    if(opcode == "add")
    {
	binary = "000";
    }
    else if(opcode == "nand")
    {
	binary = "001";
    }
    else if(opcode == "lw")
    {
	binary = "010";
    }
    else if(opcode == "sw")
    {
	binary = "011";
    }
    else if(opcode == "beq")
    {
	binary = "100";
    }
    else if(opcode == "cmov")
    {
	binary = "101";
    }
    else if(opcode == "halt")
    {
	binary = "110";
    }
    else if(opcode == "noop")
    {
	binary = "111";
    }
    else
	exit(1);

}*/
