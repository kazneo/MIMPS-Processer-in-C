/* 
Class Project: The logical conclusion (v1.1)
CSCI-2500 Spring 2021
Prof. Slota 
*/

/******************************************************************************/
/* Usual suspects to include  */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define BIT type as a char (i.e., one byte)
typedef char BIT;
#define TRUE 1
#define FALSE 0
#define UNDEF -1

// useful constants
BIT ONE[32] = {TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};
BIT ZERO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};
BIT REG_THIRTY_ONE[5] = {TRUE, TRUE, TRUE, TRUE, TRUE};
BIT THIRTY_TWO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};


/******************************************************************************/
/* Function prototypes */
/******************************************************************************/
BIT not_gate(BIT A);
BIT or_gate(BIT A, BIT B);
BIT or_gate3(BIT A, BIT B, BIT C);
BIT and_gate(BIT A, BIT B);
BIT and_gate3(BIT A, BIT B, BIT C);
BIT xor_gate(BIT A, BIT B);
BIT nor_gate(BIT A, BIT B);
BIT nand_gate(BIT A, BIT B);

void decoder2(BIT I0, BIT I1, BIT* O0, BIT* O1, BIT* O2, BIT* O3);
BIT multiplexor2(BIT S, BIT I0, BIT I1);
void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output);
BIT multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3);

void copy_bits(BIT* A, BIT* B);
void print_binary(BIT* A);
void convert_to_binary(int a, BIT* A, int length);
void convert_to_binary_char(int a, char* A, int length);
int binary_to_integer(BIT* A);

int get_instructions(BIT Instructions[][32]);

void Instruction_Memory(BIT* ReadAddress, BIT* Instruction);
void Control(BIT* OpCode,
  BIT* RegDst, BIT* Jump, BIT* Branch, BIT* MemRead, BIT* MemToReg,
  BIT* ALUOp, BIT* MemWrite, BIT* ALUSrc, BIT* RegWrite, BIT* SpecialJump, BIT* LinkJump);
void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
  BIT* ReadData1, BIT* ReadData2);
void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData);
void ALU_Control(BIT* ALUOp, BIT* funct, BIT* ALUControl);
void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result);
void Data_Memory(BIT MemWrite, BIT MemRead, 
  BIT* Address, BIT* WriteData, BIT* ReadData);
void Extend_Sign16(BIT* Input, BIT* Output);
void updateState();
  
/******************************************************************************/
/* Functions provided for your convenience */
/******************************************************************************/
BIT not_gate(BIT A)
{
  if (A)
    return FALSE;
  else
    return TRUE;
}

BIT or_gate(BIT A, BIT B)
{
  if (A || B)
    return TRUE;
  else
    return FALSE;
}

BIT or_gate3(BIT A, BIT B, BIT C)
{
  return or_gate(A, or_gate(B, C));
}

BIT and_gate(BIT A, BIT B)
{
  if (A && B)
    return TRUE;
  else
    return FALSE;
}

BIT and_gate3(BIT A, BIT B, BIT C)
{
  return and_gate(A, and_gate(B, C));
}

BIT xor_gate(BIT A, BIT B)
{
  if (A ^ B)
    return TRUE;
  else
    return FALSE;
}

BIT nor_gate(BIT A, BIT B)
{
  return not_gate(or_gate(A, B));
}

BIT nand_gate(BIT A, BIT B)
{
  return not_gate(and_gate(A, B));
}

void decoder2(BIT I0, BIT I1, BIT* O0, BIT* O1, BIT* O2, BIT* O3)
{
  // Note: The input -> output mapping is slightly modified from before
  *O0 = and_gate(not_gate(I1), not_gate(I0));
  *O1 = and_gate(not_gate(I1), I0);
  *O2 = and_gate(I1, not_gate(I0));
  *O3 = and_gate(I1, I0);
  
  return;
}

BIT multiplexor2(BIT S, BIT I0, BIT I1)
{
  BIT x0 = and_gate(not_gate(S), I0);
  BIT x1 = and_gate(S, I1);
  return or_gate(x0, x1);  
}

void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output)
{
  for (int i = 0; i < 32; ++i) {
    BIT x0 = and_gate(not_gate(S), I0[i]);
    BIT x1 = and_gate(S, I1[i]);
    Output[i] = or_gate(x0, x1);
  }
}

BIT multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3)
{
  BIT x0, x1, x2, x3 = FALSE;
  decoder2(S0, S1, &x0, &x1, &x2, &x3);
  
  BIT y0 = and_gate(x0, I0);
  BIT y1 = and_gate(x1, I1);
  BIT y2 = and_gate(x2, I2);
  BIT y3 = and_gate(x3, I3);
  
  BIT z0 = or_gate(y0, y1);
  BIT z1 = or_gate(y2, y3);
  
  return or_gate(z0, z1);  
}


/******************************************************************************/
/* Helper functions */
/******************************************************************************/
void copy_bits(BIT* A, BIT* B)
{
  for (int i = 0; i < 32; ++i)
    B[i] = A[i]; 
}

void print_binary(BIT* A)
{
  for (int i = 31; i >= 0; --i)
    printf("%d", A[i]); 
}

void convert_to_binary(int a, BIT* A, int length)
{
  if (a >= 0) {
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 1 ? TRUE : FALSE);
      a /= 2;
    }
  } else {
    a += 1;
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 0 ? TRUE : FALSE);
      a /= 2;
    }
  }
}

void convert_to_binary_char(int a, char* A, int length)
{
  if (a >= 0) {
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 1 ? '1' : '0');
      a /= 2;
    }
  } else {
    a += 1;
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 0 ? '1' : '0');
      a /= 2;
    }
  }
}
  
int binary_to_integer(BIT* A)
{
  unsigned a = 0;
  unsigned mult = 1;
  
  for (int i = 0; i < 32; ++i) {
    a += A[i]*mult;
    mult *= 2;
  }
  
  return (int)a;
}

//---------------------------------------------------
// Some extra helper functions
//---------------------------------------------------

int convert_register(char* reg){
  if(strcmp(reg,"zero") == 0){
    return 0;
  }else if(strcmp(reg,"sp") == 0){
    return 29;
  }else if(strcmp(reg,"ra") == 0){
    return 31;
  }else if(reg[0] == 'v'){
    return 2+atoi(&reg[1]);
  }else if(reg[0] == 'a'){
    return 4+atoi(&reg[1]);
  }else if(reg[0] == 't'){
    return 8+atoi(&reg[1]);
  }else if(reg[0] == 's'){
    return 16+atoi(&reg[1]);
  }
  return -1;
}

BIT or_gate4(BIT A, BIT B, BIT C, BIT D)
{
  return or_gate(A, or_gate3(B, C, D));
}

BIT and_gate4(BIT A, BIT B, BIT C, BIT D)
{
  return and_gate(A, and_gate3(B, C, D));
}

BIT and_gate5(BIT A, BIT B, BIT C, BIT D, BIT E)
{
  return and_gate(A, and_gate4(B, C, D, E));
}

BIT and_gate6(BIT A, BIT B, BIT C, BIT D, BIT E, BIT F)
{
  return and_gate(A, and_gate5(B, C, D, E, F));
}

void decoder3(BIT* I, BIT EN, BIT* O)
{
  // 3-to-8 decoder using gates
  // See lecture slides, book, and/or online resources for logic designs
  O[0] = and_gate(EN,and_gate3(not_gate(I[2]),not_gate(I[1]),not_gate(I[0])));
  O[1] = and_gate(EN,and_gate3(not_gate(I[2]),not_gate(I[1]),I[0]));
  O[2] = and_gate(EN,and_gate3(not_gate(I[2]),I[1],not_gate(I[0])));
  O[3] = and_gate(EN,and_gate3(not_gate(I[2]),I[1],I[0]));
  O[4] = and_gate(EN,and_gate3(I[2],not_gate(I[1]),not_gate(I[0])));
  O[5] = and_gate(EN,and_gate3(I[2],not_gate(I[1]),I[0]));
  O[6] = and_gate(EN,and_gate3(I[2],I[1],not_gate(I[0])));
  O[7] = and_gate(EN,and_gate3(I[2],I[1],I[0]));
  return;
}

void decoder5(BIT* I, BIT* O)
{
  // 5-to-32 decoder using 2-to-4 and 3-to-8 decoders
  BIT T[8] = {0};
  BIT T0,T1,T2,T3;
  int index = 0;
  int i;
  decoder2(I[3],I[4],&T0,&T1,&T2,&T3);
  decoder3(I,T0,T);
  for(i = 0; i < 8; ++i,++index){
    O[index] = T[i];
  }
  decoder3(I,T1,T);
  for(i = 0; i < 8; ++i,++index){
    O[index] = T[i];
  }
  decoder3(I,T2,T);
  for(i = 0; i < 8; ++i,++index){
    O[index] = T[i];
  }
  decoder3(I,T3,T);
  for(i = 0; i < 8; ++i,++index){
    O[index] = T[i];
  }
  return;
}

/******************************************************************************/
/* Parsing functions */
/******************************************************************************/

int get_instructions(BIT Instructions[][32])
{
  char line[256] = {0};
  int instruction_count = 0;
  
  int index,i;
  char i1[32], i2[32], i3[32], i4[32];
  BIT opcode[6], rs[5], rt[5], rd[5], shamt[5], funct[6], immediate[16], address[26];
  while (fgets(line, 256, stdin) != NULL) {        
    // Implemented by Kevin Xia: perform conversion of instructions to bianry
    // Input: coming from stdin via: ./a.out < input.txt
    // Output: Convert instructions to binary in the instruction memory
    // Return: Total number of instructions
    // Note: you are free to use if-else and external libraries here
    // Note: you don't need to implement circuits for saving to inst. mem.
    // My approach:
    // - Use sscanf on line to get strings for instruction and registers
    // - Use instructions to determine op code, funct, and shamt fields
    // - Convert immediate field and jump address field to binary
    // - Use registers to get rt, rd, rs fields
    // Note: I parse everything as strings, then convert to BIT array at end
    sscanf(line,"%s %s %s %s",i1,i2,i3,i4);
    if(strcmp(i1,"lw") == 0 || strcmp(i1,"sw") == 0 || strcmp(i1,"beq") == 0 || strcmp(i1,"addi") == 0){
      if(strcmp(i1,"lw") == 0){
        convert_to_binary(35,opcode,6);
      }else if(strcmp(i1,"sw") == 0){
        convert_to_binary(43,opcode,6);
      }else if(strcmp(i1,"beq") == 0){
        convert_to_binary(4,opcode,6);
      }else if(strcmp(i1,"addi") == 0){
        convert_to_binary(8,opcode,6);
      }
      convert_to_binary(convert_register(i2),rt,5);
      convert_to_binary(convert_register(i3),rs,5);
      convert_to_binary(atoi(i4),immediate,16);
      index = 31;
      for(i = 5; i >= 0; --i){
        Instructions[instruction_count][index] = opcode[i];
        --index;
      }
      for(i = 4; i >= 0; --i){
        Instructions[instruction_count][index] = rs[i];
        --index;
      }
      for(i = 4; i >= 0; --i){
        Instructions[instruction_count][index] = rt[i];
        --index;
      }
      for(i = 15; i >= 0; --i){
        Instructions[instruction_count][index] = immediate[i];
        --index;
      }
    }else if(strcmp(i1,"add") == 0 || strcmp(i1,"or") == 0 || strcmp(i1,"and") == 0 || strcmp(i1,"sub") == 0 || strcmp(i1,"slt") == 0 || strcmp(i1,"jr") == 0){
      convert_to_binary(0,opcode,6);
      convert_to_binary(convert_register(i2),rd,5);
      convert_to_binary(convert_register(i3),rs,5);
      convert_to_binary(convert_register(i4),rt,5);
      convert_to_binary(0,shamt,5);
      if(strcmp(i1,"add") == 0){
        convert_to_binary(32,funct,6);
      }else if(strcmp(i1,"or") == 0){
        convert_to_binary(37,funct,6);
      }else if(strcmp(i1,"and") == 0){
        convert_to_binary(36,funct,6);
      }else if(strcmp(i1,"sub") == 0){
        convert_to_binary(34,funct,6);
      }else if(strcmp(i1,"slt") == 0){
        convert_to_binary(42,funct,6);
      }else if(strcmp(i1,"jr") == 0){
        convert_to_binary(8,funct,6);
        convert_to_binary(convert_register(i2),rs,5);
        convert_to_binary(0,rd,5);
        convert_to_binary(0,rt,5);
      }
      index = 31;
      for(i = 5; i >= 0; --i){
        Instructions[instruction_count][index] = opcode[i];
        --index;
      }
      for(i = 4; i >= 0; --i){
        Instructions[instruction_count][index] = rs[i];
        --index;
      }
      for(i = 4; i >= 0; --i){
        Instructions[instruction_count][index] = rt[i];
        --index;
      }
      for(i = 4; i >= 0; --i){
        Instructions[instruction_count][index] = rd[i];
        --index;
      }
      for(i = 4; i >= 0; --i){
        Instructions[instruction_count][index] = shamt[i];
        --index;
      }
      for(i = 5; i >= 0; --i){
        Instructions[instruction_count][index] = funct[i];
        --index;
      }
    }else if(strcmp(i1,"j") == 0 || strcmp(i1,"jal") == 0){
      if(strcmp(i1,"j") == 0){
        convert_to_binary(2,opcode,6);
      }else if(strcmp(i1,"jal") == 0){
        convert_to_binary(3,opcode,6);
      }
      convert_to_binary(atoi(i2),address,26);
      index = 31;
      for(i = 5; i >= 0; --i){
        Instructions[instruction_count][index] = opcode[i];
        --index;
      }
      for(i = 25; i >= 0; --i){
        Instructions[instruction_count][index] = address[i];
        --index;
      }
    }else{
      printf("Wrong Instruction: %s\n",i1);
      exit(-1);
    }
    ++instruction_count;
  }
  
  return instruction_count;
}


/******************************************************************************/
/* Program state - memory spaces, PC, and control */
/******************************************************************************/
BIT PC[32]                  = {FALSE};
BIT MEM_Instruction[32][32] = {FALSE};
BIT MEM_Data[32][32]        = {FALSE};
BIT MEM_Register[32][32]    = {FALSE};

BIT RegDst    = FALSE;
BIT Jump      = FALSE;
BIT Branch    = FALSE;
BIT MemRead   = FALSE;
BIT MemToReg  = FALSE;
BIT ALUOp[2]  = {FALSE};
BIT MemWrite  = FALSE;
BIT ALUSrc    = FALSE;
BIT RegWrite  = FALSE;
BIT Zero      = FALSE;
BIT ALUControl[4] = {FALSE};
BIT LinkJump = FALSE;
BIT SpecialJump = FALSE;

void print_instruction()
{
  unsigned pc = binary_to_integer(PC);
  printf("PC: %d\n", pc);
  printf("Instruction: ");
  print_binary(MEM_Instruction[pc]);
  printf("\n");
}

void print_state()
{
  printf("Data: ");
  for (int i = 0; i < 32; ++i) {
    printf("%d ", binary_to_integer(MEM_Data[i]));
  } 
  printf("\n");  
  
  printf("Register: ");
  for (int i = 0; i < 32; ++i) {
    printf("%d ", binary_to_integer(MEM_Register[i]));
  } 
  printf("\n");
}


/******************************************************************************/
/* Functions that you will implement */
/******************************************************************************/
void Instruction_Memory(BIT* ReadAddress, BIT* Instruction)
{
  // Implemented by Kevin Xia: instruction memory
  // Input: 32-bit instruction address
  // Output: 32-bit binary instruction
  // Note: Useful to use a 5-to-32 decoder here
  int i;
  BIT temp[32] = {0};
  decoder5(ReadAddress,temp);
  for(i = 0; i < 32; ++i){
    multiplexor2_32(temp[i], Instruction, MEM_Instruction[i], Instruction);
  }
}

void Control(BIT* OpCode,
  BIT* RegDst, BIT* Jump, BIT* Branch, BIT* MemRead, BIT* MemToReg,
  BIT* ALUOp, BIT* MemWrite, BIT* ALUSrc, BIT* RegWrite, BIT* SpecialJump, BIT* LinkJump)
{
  // Implemented by Anthony DiGiovanna
  // Input: opcode field from the instruction
  // Output: all control lines get set
  // Note: Can use SOP or similar approaches to determine bits

  //OpCode is expected to be 6 bits long and OpCode[0] should be the msb of the instruction
  //Setting RegDst
  //Should be TRUE when r-type instruction: And, Or, Add, Sub, Slt, Jr
  //Using a SOP of the opcodes for these instructions (all have opcode 000000)
  *RegDst = and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), not_gate(OpCode[4]) ,not_gate(OpCode[5]));

  //Setting Jump
  //Should be TRUE if jump instruction: J, Jal
  //Using a SOP of the opcodes for these instructions
  //Should be TRUE for Jr, which result in TRUE for all r-type instruction.

  // *Jump = and_gate5(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4]);
  *Jump = or_gate(and_gate5(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4]),
                  and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), not_gate(OpCode[4]), not_gate(OpCode[5]))
  );

  //Setting Branch
  //Should be TRUE if branch instruction: Beq
  //Using a SOP of the opcode for this instruction (000100)
  *Branch = and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), OpCode[3], not_gate(OpCode[4]), not_gate(OpCode[5]));

  //Setting MemRead
  //Should be TRUE if memory-loading instruction: Lw
  //Using a SOP of the opcode for this instruction (100011)
  *MemRead = and_gate6(OpCode[0], not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4], OpCode[5]);

  //Setting MemToReg
  //Should be true for only LW
  //Using a SOP of the opcodes for these instructions (000000, 100011)
  *MemToReg = and_gate6(OpCode[0], not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4], OpCode[5]);

  //Setting first ALUOp bit
  //Should be TRUE if the instruction is R-Type (Copied from RegDst code)
  ALUOp[0] = and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), not_gate(OpCode[4]) ,not_gate(OpCode[5]));
  //Setting second ALUOp bit
  //Should be TRUE if we are doing a branch instruction (Copied from Branch code)
  ALUOp[1] = and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), OpCode[3], not_gate(OpCode[4]), not_gate(OpCode[5]));

  //Setting MemWrite
  //Should be TRUE if memory-storing instruction: Sw
  //Using a SOP of the opcodes for this instruction (101011)
  *MemWrite = and_gate6(OpCode[0], not_gate(OpCode[1]), OpCode[2], not_gate(OpCode[3]), OpCode[4], OpCode[5]);

  //Setting ALUSrc
  //Should be TRUE for an I-type: Lw, Sw, Beq, Addi
  //Using a SOP of the opcodes for these instructions (101011, 100011)
  *ALUSrc = or_gate4(
    and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), OpCode[3], not_gate(OpCode[4]), not_gate(OpCode[5])),
    and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), OpCode[2], not_gate(OpCode[3]), not_gate(OpCode[4]), not_gate(OpCode[5])),
    and_gate6(OpCode[0], not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4], OpCode[5]),
    and_gate6(OpCode[0], not_gate(OpCode[1]), OpCode[2], not_gate(OpCode[3]), OpCode[4], OpCode[5])
  );

  //Setting RegWrite
  //Should be TRUE for arithmetic and load instructions: Lw, And, Or, Add, Sub, Slt, Addi
  //Should be TRUE for jal; Setting TRUE for all R-type instructions, which jr is also TRUE;

  // *RegWrite = or_gate3(
  //   and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), not_gate(OpCode[4]), not_gate(OpCode[5])),
  //   and_gate6(OpCode[0], not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4], OpCode[5]),
  //   and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), OpCode[2], not_gate(OpCode[3]), not_gate(OpCode[4]), not_gate(OpCode[5]))
  // );

  *RegWrite = or_gate4(
    and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), not_gate(OpCode[4]), not_gate(OpCode[5])),
    and_gate6(OpCode[0], not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4], OpCode[5]),
    and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), OpCode[2], not_gate(OpCode[3]), not_gate(OpCode[4]), not_gate(OpCode[5])),
    and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4], OpCode[5])
  );

  //Setting SpecialJump
  //Should be TRUE for jr and jal; Setting TRUE for all R-type instructions. Need to check for both Jump and SpecialJump.
  *SpecialJump = or_gate(and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4], OpCode[5]),
                            and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), not_gate(OpCode[4]), not_gate(OpCode[5]))
  );

  //Setting LinkJump
  //Should be TRUE for jal
  *LinkJump = and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4], OpCode[5]);

}

void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
  BIT* ReadData1, BIT* ReadData2)
{
  // Implemented by Kevin Xia: Implement register read functionality ←
  // Input: two 5-bit register addresses
  // Output: the values of the specified registers in ReadData1 and ReadData2
  // Note: Implementation will be very similar to instruction memory circuit
  int i;
  BIT temp[32] = {0};
  BIT temp2[32] = {0};
  decoder5(ReadRegister1,temp);
  decoder5(ReadRegister2,temp2);
  for(i = 0; i < 32; ++i){
    multiplexor2_32(temp[i], ReadData1, MEM_Register[i], ReadData1);
    multiplexor2_32(temp2[i], ReadData2, MEM_Register[i], ReadData2);    
  }
}

void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData)
{
  // Implemented by Kevin Xia: register write functionality
  // Input: one 5-bit register address, data to write, and control bit
  // Output: None, but will modify register file
  // Note: Implementation will again be similar to those above
  int i;
  BIT temp[32] = {0};
  decoder5(WriteRegister,temp);
  for(i = 0; i < 32; ++i){
    multiplexor2_32(and_gate(temp[i],RegWrite), MEM_Register[i], WriteData, MEM_Register[i]);
  }
}

void ALU_Control(BIT* ALUOp, BIT* funct, BIT* ALUControl)
{
  // Implemented by Anthony DiGiovanna
  // Input: 2-bit ALUOp from main control circuit, 6-bit funct field from the
  //        binary instruction
  // NOTE: funct is expected to be 6 bits, with funct[0] being the MSB
  // Output:4-bit ALUControl for input into the ALU

  //The first bit of ALUControl will always be 0, so we can leave it be
  //The second bit will be one when the ALUOp is 01, or with certain funct's
  //SOP:
  ALUControl[1] = or_gate(
    and_gate(not_gate(ALUOp[0]), ALUOp[1]),
    and_gate5(ALUOp[0], not_gate(ALUOp[1]), not_gate(funct[3]), funct[4], not_gate(funct[5]))
  );

  //The third bit will only not be one when funct[3] is one
  ALUControl[2] = or_gate(
    not_gate(ALUOp[0]),
    not_gate(funct[3])
  );

  //The fourth bit is only one when we are doing or or slt (funct's 100101, 101010)
  //SOP:
  ALUControl[3] = or_gate(
    and_gate6(funct[0], not_gate(funct[1]), not_gate(funct[2]), funct[3], not_gate(funct[4]), funct[5]),
    and_gate6(funct[0], not_gate(funct[1]), funct[2], not_gate(funct[3]), funct[4], not_gate(funct[5]))
  );
}

void adder1(BIT A, BIT B, BIT CarryIn, BIT* CarryOut, BIT* Sum){  
  *CarryOut = or_gate(and_gate(A, B), or_gate(and_gate(A, CarryIn), and_gate(B, CarryIn)));
  *Sum = or_gate(and_gate(CarryIn, and_gate(A, B)), and_gate(not_gate(*CarryOut), or_gate(A, or_gate(B, CarryIn))));
}

void ALU1(BIT A, BIT B, BIT Binvert, BIT CarryIn, BIT Less, 
  BIT Op0, BIT Op1, BIT* Result, BIT* CarryOut, BIT* Set)
{
  // TODO: implement a 1-bit ALU 
  // Note: this will need modifications from Lab 5 to account for 'slt'
  // See slide 30 in Chapter-3d
  BIT bInv = xor_gate(B, Binvert);
  adder1(A, bInv, CarryIn, CarryOut, Set);
  *Result = multiplexor4(Op0, Op1, and_gate(A, bInv), or_gate(A, bInv), *Set, Less);
}


void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result)
{
  // TODO: implement a 32-bit ALU
  // You'll need to essentially implement a 32-bit ripple addder here
  // See slide 31 in Chapter-3d
  BIT set = 0;
  BIT tempCarry = ALUControl[1];
  BIT CarryOut = 0;
  *Zero = 1;
  for(int i = 0; i < 32; i++)
  {
    ALU1(Input1[i], Input2[i], ALUControl[1], tempCarry, 0, ALUControl[3], ALUControl[2], &Result[i], &CarryOut, &set);
    tempCarry = CarryOut;
    *Zero = and_gate(not_gate(Result[i]), *Zero);
  }
  ALU1(Input1[0], Input2[0], ALUControl[1], ALUControl[1], set, ALUControl[3], ALUControl[2], &Result[0], &tempCarry, &set);
  *Zero = and_gate(not_gate(Result[0]), *Zero);
}

void Data_Memory(BIT MemWrite, BIT MemRead, 
  BIT* Address, BIT* WriteData, BIT* ReadData)
{
  // TODO: Implement data memory
  // Input: 32-bit address, control flags for read/write, and data to write
  // Output: data read if processing a lw instruction
  // Note: Implementation similar as above
  
  BIT toWrite[32] = {FALSE};
  BIT toRead[32] = {FALSE};
  decoder5(Address, toWrite);
  decoder5(Address, toRead);
  for(int i = 0; i < 32; ++i){
   multiplexor2_32(and_gate(toWrite[i], MemWrite), MEM_Data[i], WriteData, MEM_Data[i]);
   multiplexor2_32(and_gate(toRead[i], MemRead), ReadData, MEM_Data[i], ReadData);
  }
}

void Extend_Sign16(BIT* Input, BIT* Output)
{
  // Implemented by Anthony DiGiovanna
  // Copy Input to Output, then extend 16th Input bit to 17-32 bits in Output

  // NOTE: This is done assuming input[0] is the lsb and input[31] is the msb
  copy_bits(Input,Output);
  for(int i = 15; i < 32; i++) {
    Output[i] = Input[15];
  }
}

void updateState()
{
  //Generate Instruction from PC
  BIT Instruction[32] = {0};
  Instruction_Memory(PC, Instruction);
  BIT OpCode[6];
  for(int i = 31; i >= 26; --i) {
	  OpCode[31-i] = Instruction[i];
  }
  Control(OpCode, &RegDst, &Jump, &Branch, &MemRead, &MemToReg, ALUOp, &MemWrite, &ALUSrc, &RegWrite, &SpecialJump, &LinkJump);

  /********************************************/
  /*Register manipulation                     */
  /********************************************/

  //Read Register
  BIT ReadData1[32], ReadData2[32];
  copy_bits(ZERO, ReadData1);
  copy_bits(ZERO, ReadData2);
  BIT readReg1[5] = {Instruction[21], Instruction[22], Instruction[23], Instruction[24], Instruction[25]};
  BIT readReg2[5] = {Instruction[16], Instruction[17], Instruction[18], Instruction[19], Instruction[20]};  
  Read_Register(readReg1, readReg2, ReadData1, ReadData2);

  //Sign Extender
  BIT extend[32];
  copy_bits(ZERO, extend);
  BIT toExtend[16] = {Instruction[0], Instruction[1], Instruction[2], Instruction[3], Instruction[4], Instruction[5], Instruction[6], Instruction[7], Instruction[8], Instruction[9], Instruction[10], Instruction[11], Instruction[12], Instruction[13], Instruction[14], Instruction[15]};
  Extend_Sign16(toExtend, extend);

  //ALU
  BIT ALUResult[32];
  copy_bits(ZERO, ALUResult);
  BIT aluCont[6] = {Instruction[5], Instruction[4], Instruction[3], Instruction[2], Instruction[1], Instruction[0]};

  //Control line for jr and jal instruction
  //Jump to be TRUE only for jal, jr, j
  Jump = or_gate3(and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4], OpCode[5]),
                and_gate6(not_gate(aluCont[0]),not_gate(aluCont[1]),aluCont[2],not_gate(aluCont[3]),not_gate(aluCont[4]),not_gate(aluCont[5])),
                and_gate5(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4])
                );
  //Special Jump to be TRUE only for jal, jr
  SpecialJump = or_gate(and_gate6(not_gate(OpCode[0]), not_gate(OpCode[1]), not_gate(OpCode[2]), not_gate(OpCode[3]), OpCode[4], OpCode[5]),
                and_gate6(not_gate(aluCont[0]),not_gate(aluCont[1]),aluCont[2],not_gate(aluCont[3]),not_gate(aluCont[4]),not_gate(aluCont[5]))
                );
  // print_binary(OpCode);
  // printf("\nJump,SpecialJump,LinkJump:%d,%d,%d\n",Jump,SpecialJump,LinkJump);
  ALU_Control(ALUOp, aluCont, ALUControl);
  BIT ALUMux[32];
  multiplexor2_32(ALUSrc, ReadData2, extend, ALUMux);
  ALU(ALUControl, ReadData1, ALUMux, &Zero, ALUResult);

  //Data Memory
  BIT memReadData[32];
  copy_bits(ZERO, memReadData);
  Data_Memory(MemWrite, MemRead, ALUResult, ReadData2, memReadData);
  BIT dataToWrite[32];
  multiplexor2_32(MemToReg, ALUResult, memReadData, dataToWrite);

  //Write Register
  BIT writeReg[5];
  for(int i = 0; i < 5; i++)
  {
    writeReg[i] = multiplexor2(RegDst, Instruction[i+16], Instruction[i+11]);
  }
  BIT add[4] = {FALSE, FALSE, TRUE, FALSE};
  BIT throaway;
  BIT PC2[32];
  BIT Two[32];
  Two[1] = 1;
  ALU(add, PC, Two, &throaway, PC2);
  multiplexor2_32(SpecialJump, dataToWrite, PC2, dataToWrite);
  Write_Register(RegWrite, writeReg, dataToWrite);
  

  /********************************************/
  /*Determine PC value                        */
  /********************************************/
  

  //Calculate Jump Address
  /*BIT Four[32];
  copy_bits(ZERO, Four);
  Four[2] = TRUE;*/  
  BIT PC1[32];
  copy_bits(ZERO, PC1);
  ALU(add, PC, ONE, &throaway, PC1);
  BIT JumpAddress[32];
  copy_bits(PC1, JumpAddress);
  for(int i = 0; i < 26; i++)
  {
    JumpAddress[i] = Instruction[i];
  }

  //Determine what address to set PC to
  BIT JmpMux[32];
  copy_bits(extend, JmpMux);
  BIT JmpALU[32];
  ALU(add, PC1, JmpMux, &throaway, JmpALU);
  multiplexor2_32(and_gate(Branch, Zero), PC1, JmpALU, JmpALU);
  BIT sJump[32];
  multiplexor2_32(LinkJump, extend, ReadData1, sJump);
  multiplexor2_32(SpecialJump, JmpALU, sJump, JmpALU);
  multiplexor2_32(Jump, JmpALU, JumpAddress, PC);
}


/******************************************************************************/
/* Main */
/******************************************************************************/

int main()
{
  setbuf(stdout, NULL);
    
  // parse instructions into binary format
  int counter = get_instructions(MEM_Instruction);
  
  // load program and run
  copy_bits(ZERO, PC);
  copy_bits(THIRTY_TWO, MEM_Register[29]);
  
  while (binary_to_integer(PC) < counter) {
    print_instruction();
    updateState();
    print_state();
  }

  return 0;
}
