#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// #define DATA_LEN 6

// RAM Functions
unsigned char cpu_ram_read(struct cpu *cpu, int MAR) 
{
  return cpu->ram[MAR];
}

void cpu_ram_write(struct cpu *cpu, int MAR, unsigned char MDR) 
{
  cpu->ram[MAR] = MDR;
}

// Helper Functions
// void push(struct cpu *cpu, unsigned char MDR) 
// {
//   cpu->reg[7]--;
//   cpu_ram_write(cpu, cpu->reg[7], MDR);
// }

// void pop(struct cpu *cpu, int MAR) 
// {
//   unsigned char MDR = cpu_ram_read(cpu, cpu->reg[7]);
//   cpu->reg[MAR] = MDR;
//   cpu->reg[7]++;
// }

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *path)
{
  // read lines from a file, convert to numbers, store in ram
  FILE *file;
  char line[1024];

  file = fopen(path, "r");

  if (file == NULL) {
    printf("File failed to open");
  }
  
  int MAR = 0;
  while (fgets(line, 1024, file) != NULL) {

    char *end;
    unsigned char MDR = strtoul(line, &end, 2);
    
    if (line == end) {
      continue;
    }

    cpu_ram_write(cpu, MAR++, MDR);
  }

  fclose(file);

  // char data[DATA_LEN] = {
  //   // From print8.ls8
  //   0b10000010, // LDI R0,8
  //   0b00000000,
  //   0b00001000,
  //   0b01000111, // PRN R0
  //   0b00000000,
  //   0b00000001  // HLT
  // };
  
  // int address = 0;

  // for (int i = 0; i < DATA_LEN; i++) {
  //   cpu->ram[address++] = data[i];
  // }

}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_ADD:
      break;

    case ALU_SUB:
      break;
    
    case ALU_MUL:
      cpu->reg[regA] *= cpu->reg[regB];
      break;
    
    case ALU_DIV:
      break;
    
    case ALU_MOD:
      break;

    case ALU_INC:
      break;

    case ALU_DEC:
      break;
    
    case ALU_CMP:
      break;

    case ALU_AND:
      break;
    
    case ALU_NOT:
      break;
    
    case ALU_OR:
      break;
    
    case ALU_XOR:
      break;
    
    case ALU_SHL:
      break;

    case ALU_SHR:
      break;
  
    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  while (running) {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    unsigned int IR = cpu->ram[cpu->PC];

    // 2. Figure out how many operands this next instruction requires
    int num_ops = IR >> 6;
    int offset = num_ops + 1;

    // 3. Get the appropriate value(s) of the operands following this instruction
    unsigned char operandA = cpu_ram_read(cpu, cpu->PC + 1);
    unsigned char operandB = cpu_ram_read(cpu, cpu->PC + 2);

    // 4. switch() over it to decide on a course of action.
    // 5. Do whatever the instruction should do according to the spec.
    switch (IR) {
      case HLT:
        running = 0;
        exit(0);
        break;
      
      case LDI:
        cpu->reg[operandA] = operandB;
        break;
      
      case PRN:
        printf("%d", cpu->reg[operandA]);
        break; 

      case MUL:
        alu(cpu, ALU_MUL, operandA, operandB);
        break;

      case PUSH:
        cpu->reg[7]--;
        cpu_ram_write(cpu, cpu->reg[7], operandA);
        break;

      case POP:
        cpu->reg[operandA] = cpu_ram_read(cpu, cpu->reg[7]);
        cpu->reg[7]++;
        break;

      default:
        printf("instruction not recognized");
        exit(1);
        break;
    }

    // 6. Move the PC to the next instruction.
    cpu->PC += offset;
  }
  
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  cpu->PC = 0;
  memset(cpu->ram, 0, sizeof(cpu->ram));
  memset(cpu->reg, 0, sizeof(cpu->reg));
  cpu->reg[7] = 0xF4;
  // TODO: Initialize the PC and other special registers
}
