/**************************************************************************************************
 * Filename: LegV8_Disassmbler_gcox.c
 * Summary: Program to take binary strings from user input and translate them to LegV8
 * Author: Gage Cox
 * Date: 6/4/24
**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> //pow()

// function prototypes
void dataIn();
void lengthCheck(char[], int);
int splitBin(char[], char[], char[], char[], char[], char[], int);
int binToDec(char[]);
void sanityCheck(char[], char[], char[], char[], int, int);
void binToLeg(char[], char[], char[], char[], char[], int);


int main()
{
    FILE* fpointer;
    char binInstruction[50];
    char instruction[50];
    char reg1[50];
    char reg2[50];
    char destReg[50];
    char immediate[50];
    int counter = 0;  // track number of lines read
    int immCheck;     // check if immediate, 0 if yes 1 if no


    dataIn();
    fpointer = fopen("binaryInstructions.txt", "r");
    while (fgets(binInstruction, 50, fpointer)!=NULL)
    {
        counter++;
        lengthCheck(binInstruction, counter);
        immCheck = splitBin(binInstruction, instruction, reg1, reg2, destReg, immediate, counter);
        sanityCheck(reg1, reg2, destReg, immediate, immCheck, counter);
        binToLeg(instruction, reg1, reg2, destReg, immediate, immCheck);
    }
    fclose(fpointer);

    return 0;
}

void dataIn()
{
    char loop = 'y';
    char buffer;
    char binInstruction[50];
    char temp[50];
    FILE* filePointer;
    int loopCount = 0; // count number of for loops

    filePointer = fopen("binaryInstructions.txt", "w");
    if (filePointer == NULL)
    {
        printf("binaryInstructions.txt failed to open.\n");
        exit(0);
    }

    while (loop == 'y')
    {
        printf("Enter a binary instruction: ");
        fgets(temp, 50, stdin);
        for (int i = 0, j = 0; i < strlen(temp); i++, j++)
        {
            if (temp[i] != ' ')
            {
                binInstruction[j] = temp[i];
                loopCount = j;
            }
            else
            {
                j--;
                loopCount = j;
            }
        }
        binInstruction[loopCount] = '\0';
        fputs(binInstruction, filePointer);
        fputs("\n", filePointer);
        printf("Would you like to enter another instruction? (y/n)");
        scanf("%c", &loop);
        scanf("%c", &buffer);
    }

    fclose(filePointer);
}

void lengthCheck(char bin[], int count)
{
    int length = strlen(bin);
    bin[length-1] = '\0';
    length = strlen(bin);
    if (length != 32)
    {
        printf("Invalid Instruction on line %d", count);
        exit(0);
    }

}

int splitBin(char bin[],  char instruction[], char reg1[], char reg2[], char destReg[], char immediate[], int counter)
{
    char nonI[32];
    char imm[32];

    strncpy(nonI, bin, 11);
    nonI[11] = '\0';
    strncpy(imm, bin, 10);
    imm[10] = '\0';
    
    // ADDI or SUBI
    if (strcmp(imm, "1001000100") == 0 || strcmp(imm, "1101000100") == 0) 
    {
        strncpy(instruction, bin, 10);
        instruction[10] = '\0';
        strncpy(immediate, bin + 10, 12);
        immediate[12] = '\0';
        strncpy(reg1, bin + 22, 5);
        reg1[5] = '\0';
        strncpy(destReg, bin + 27, 5);
        destReg[5] = '\0';
        return 0;
    }

    // ADD, SUB, LDUR, STUR
    else if (strcmp(nonI, "10001011000") == 0 || strcmp(nonI, "11001011000") == 0 || strcmp(nonI, "11111000010") == 0 || strcmp(nonI, "11111000000") == 0)
    {
        strncpy(instruction, bin, 11);
        instruction[11] = '\0';
        strncpy(reg2, bin + 11, 5);
        reg2[5] = '\0';
        strncpy(immediate, bin + 16, 6);
        immediate[6] = '\0';
        strncpy(reg1, bin + 22, 5);
        reg1[5] = '\0';
        strncpy(destReg, bin + 27, 5);
        destReg[5] = '\0';
        return 1;
    }

    else
    {
        printf("Invalid opcode on line %d", counter);
        exit(0);
    }

}

int binToDec(char bin[])
{
    int toDec = atoi(bin);
    int dec = 0;
    int i = 0;
    int rem;

    while (toDec != 0)
    {
        rem = toDec % 10;
        toDec /= 10;
        dec += rem * pow(2, i);
        ++i;
    }
    return dec;
}

void sanityCheck(char reg1[], char reg2[], char destReg[], char immediate[], int immCheck, int counter)
{
    int temp;

    if (immCheck == 0)
    {
        temp = binToDec(reg1);
        if (temp > 31)
        {
            printf("Invalid source register on line %d", counter);
            exit(0);
        }
        temp = binToDec(destReg);
        if (temp > 30)
        {
            printf("Invaled destination register on line %d", counter);
            exit(0);
        }
    }       

    else if (immCheck == 1)
    {
        temp = binToDec(reg2);
        if (temp > 31)
        {
            printf("Invalid second register on line %d", counter);
            exit(0);
        }
        temp = binToDec(reg1);
        if (temp > 31)
        {
            printf("Invalid first register on line %d", counter);
            exit(0);
        }
        temp = binToDec(destReg);
        if (temp > 30)
        {
            printf("Invalid destination register on line %d", counter);
            exit(0);
        }
    }

}

void binToLeg(char instruction[], char reg1[], char reg2[], char destReg[], char immediate[], int immCheck)
{
    int first;
    int second;
    int dest;
    int imm;
    if (immCheck == 0) // ADDI or SUBI
    {
        first = binToDec(reg1);
        dest = binToDec(destReg);
        imm = binToDec(immediate);
        if (strcmp(instruction, "1001000100") == 0) // Compare to ADDI opcode
        {
            if (first == 31)
                printf("ADDI X%d, XZR, #%d\n", dest, imm); // Check if reg 31 and print XZR instead
            else
                printf("ADDI X%d, X%d, #%d\n", dest, first, imm);
        }
        else if (strcmp(instruction, "1101000100") == 0) // Compare to SUBI opcode
        {
            if (first == 31)
                printf("SUBI X%d, XZR, #%d\n", dest, imm); // Check if reg 31 and print XZR instead
            else
                printf("SUBI X%d, X%d, #%d\n", dest, first, imm);
        }
    }

    else if (immCheck == 1) // ADD, SUB, LDUR, STUR
    {
        first = binToDec(reg1);
        second = binToDec(reg2);
        dest = binToDec(destReg);
        imm = binToDec(immediate);
        if(strcmp(instruction, "10001011000") == 0) // compare to ADD opcode
        {
            if (first == 31)
                printf("ADD X%d, XZR, X%d\n", dest, second); // Check if reg is 31 and print XZR instead
            else
                printf("ADD X%d, X%d, X%d\n", dest, first, second);
        }
        else if (strcmp(instruction, "11001011000") == 0) // Compare to SUB opcode
        {
            if (first == 31)
                printf("SUB X%d, XZR, X%d\n", dest, second); // Check if reg 31 and print XZR instead
            else
                printf("SUB X%d, X%d, X%d\n", dest, first, second);
        }
        else if (strcmp(instruction, "11111000010") == 0)   // Compare to LDUR opcode
            printf("LDUR X%d, [X%d, #%d]\n", dest, first, imm);
        else if (strcmp(instruction, "11111000000") == 0)   // Compare to STUR opcode
            printf("STUR X%d, [X%d, #%d]\n", dest, first, imm);
    }
}