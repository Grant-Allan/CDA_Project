/*
Project Group Members:
Grant Allan
Dave Dominique
Korey Fletcher
*/


#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A, unsigned B, char ALUControl, unsigned *ALUresult, char *Zero)
{

    switch (ALUControl)
    {
    // Z = A + B
    case 0x0:
        *ALUresult = A + B;
        break;
        
    // Z = A - B
    case 0x1:
        *ALUresult = A - B;
        break;

    // If A < B, Z = 1, else Z = 0
    // We have to cast this to int (or signed) because the stl instruction isn't unsigned.
    case 0x2:
        if ((int)A < (int)B) {
            *ALUresult = 1;
        } else {
            *ALUresult = 0;
        }
        break;
        
    // If A < B, Z = 1, else Z = 0
    case 0x3:
        if (A < B) {
            *ALUresult = 1;
        } else {
            *ALUresult = 0;
        }
        break;

    // Z = A AND B
    case 0x4:
        *ALUresult = A & B;
        break;
            
    // Z = A OR B
    case 0x5:
        *ALUresult = A | B;
        break;

    // Shift left B by 16 bits
    case 0x6:
        *ALUresult = B << 16;
        break;

    // Z = NOT A
    case 0x7:
        *ALUresult = ~A;
        break;
    }

    // If *ALUresult is equal to 0, set *Zero to 1. Else, set *Zero to 0.
    if (*ALUresult == 0) {
        *Zero = 1;
    } else {
        *Zero = 0;
    }
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction)
{
    // Use mod to check for if PC is a multiple of 4
    if (PC % 4 0 = 0) {
        *instruction = Mem[PC >> 2];
        return 0;
    }

    // If PC is not divisible by 4, return 1
    return 1;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1, unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // 1111 1100 0000 0000 0000 0000 0000 0000
    *op = (instruction & 0xfc000000) >> 26;

    // 0000 0011 1110 0000 0000 0000 0000 0000
    *r1 = (instruction & 0x03e00000) >> 21;

    // 0000 0000 0001 1111 0000 0000 0000 0000
    *r2 = (instruction & 0x001f0000) >> 16;

    // 0000 0000 0000 0000 1111 1000 0000 0000
    *r3 = (instruction & 0x0000f800) >> 11;

    // 0000 0000 0000 0000 0000 0000 0011 1111
    *funct = instruction & 0x0000003f;

    // 0000 0000 0000 0000 1111 1111 1111 1111
    *offset = instruction & 0x0000ffff;

    // 0000 0011 1111 1111 1111 1111 1111 1111
    *jsec = instruction & 0x03ffffff;
}


/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op, struct_controls* controls)
{
    switch (op)
    {
    // R-type instruction
    case 0x0:
        *controls = (struct_controls){ 1,0,0,0,0,7,0,0,1 };
        break;

    // Add immediate
    case 0x8:
        *controls = (struct_controls){ 0,0,0,0,0,0,0,1,1 };
        break;

    // Load word
    case 0x23:
        *controls = (struct_controls){ 0,0,0,1,1,0,0,1,1 };
        break;

    // Store word
    case 0x2b:
        *controls = (struct_controls){ 0,0,0,0,0,0,1,1,0 };
        break;

    // Load upper immediate
    case 0xf:
        *controls = (struct_controls){ 0,0,0,0,0,6,0,1,1 };
        break;

    // Branch on equal
    case 0x4:
        *controls = (struct_controls){ 2,0,1,0,2,1,0,2,0 };
        break;

    // Set less than immediate
    case 0xa:
        *controls = (struct_controls){ 0,0,0,0,0,2,0,1,1 };
        break;

    // Set less than immediate unsigned
    case 0xb:
        *controls = (struct_controls){ 0,0,0,0,0,3,0,1,1 };
        break;

    // Jump
    case 0x2:
        *controls = (struct_controls){ 2,1,2,0,2,0,0,2,0 };
        break;

    // If none of these apply, halt condition, return 1
    default:
        return 1;
    }

    // No halt condition, so successful encoding, so return 0
    return 0;
}


/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg, unsigned *data1, unsigned *data2)
{
    // Get the values of r1 and r2 from the register
    // Fill data1 and data2 with those values
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value)
{
    // Check if offset is negative
    if ((offset >> 15) == 1)
        // If it is negative, expand to 32 bits
        *extended_value = offset | 0xffff0000;
    else
        // If it is positive, keep at 16 bits
        *extended_value = offset & 0x0000ffff;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1, unsigned data2, unsigned extended_value, unsigned funct, char ALUOp, char ALUSrc, unsigned *ALUresult, char *Zero)
{
    unsigned ALUControl = ALUOp;

    // If a junk instruction is passed, send halt conditions
    if (ALUOp < 0 || ALUOp > 7)
        return 1;
    else {
        // Check to see if it's a valid R-type instruction
        switch (funct)
        {
        // Add
        case 0x20:
            ALUControl = 0;
            break;

        // Subtract
        case 0x22:
            ALUControl = 1;
            break;

        // And
        case 0x24:
            ALUControl = 4;
            break;

        // Or
        case 0x25:
            ALUControl = 5;
            break;
            
        // Set less than
        case 0x2a:
            ALUControl = 2;
            break;
            
        // Set less than unsigned
        case 0x2b:
            ALUControl = 3;
            break;
            
        // If it's not valid, return the halt condition
        default:
            return 1;
        }
    }

    // If the second ALU inptu comes from the sign extend unit, set the second data to extended_value
    unsigned B = (ALUSrc == 1) ? extended_value : data2;
    
    ALU(data1, B, ALUControl, ALUresult, Zero);

    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult, unsigned data2, char MemWrite, char MemRead, unsigned *memdata, unsigned *Mem)
{
    // Write to memory
    if (MemWrite == 1)
        // Check to see if ALUresult is divisible by 4
        if (ALUresult % 4 == 0)
            // If it is, write the value to an address shifted over by two
            Mem[ALUresult >> 2] = data2;
        else
            return 1;

    // Read from memory
    if (MemRead == 1)
        // Check to see if ALUresult is divisible by 4
        if (ALUresult % 4 == 0)
            // If it is, shift right by 2 to get the desired value
            *memdata = Mem[ALUresult >> 2];
        else
            return 1;

    return 0;
}
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
    if (RegWrite == 1) {
        // If it's from memory, it's I-type
        if (MemtoReg == 1)
            Reg[r2] = memdata;

        // If it comes from a register, check for which one
        if (MemtoReg == 0) {
            // If it's R-type, write to r3
            if (RegDst == 1)
                Reg[r3] = ALUresult;
            // If it's I-type , write to r2
            else
                Reg[r2] = ALUresult;
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec, unsigned extended_value, char Branch, char Jump, char Zero, unsigned *PC)
{
    // Update PC
    *PC += 4;

    // If it branched:
    if (Zero == 1 && Branch == 1)
        *PC += (extended_value << 2);

    // If we need to jump:
    if (Jump == 1)
        *PC = (*PC & 0xf000000) | (jsec << 2);
}
