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
void push(struct cpu *cpu, unsigned char MDR) 
{
  cpu->reg[7]--;
  cpu_ram_write(cpu, cpu->reg[7], cpu->reg[MDR]);
}

void pop(struct cpu *cpu, int MAR) 
{
  cpu->reg[MAR] = cpu_ram_read(cpu, cpu->reg[7]);
  cpu->reg[7]++;
}

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
      cpu->reg[regA] += cpu->reg[regB];
      break;

    case ALU_SUB:
      cpu->reg[regA] -= cpu->reg[regB];
      break;
    
    case ALU_MUL:
      cpu->reg[regA] *= cpu->reg[regB];
      break;
    
    case ALU_DIV:
      cpu->reg[regA] /= cpu->reg[regB];
      break;
    
    case ALU_MOD:
      cpu->reg[regA] %= cpu->reg[regB];
      break;

    case ALU_INC:
      cpu->reg[regA]++;
      break;

    case ALU_DEC:
      cpu->reg[regA]--;
      break;
    
    case ALU_CMP:
      if (cpu->reg[regA] == cpu->reg[regB]) {
        cpu->FL = 0b00000001;
      } else if (cpu->reg[regA] > cpu->reg[regB]) {
        cpu->FL = 0b00000010;
      } else if (cpu->reg[regA] < cpu->reg[regB]) {
        cpu->FL = 0b00000100;
      }
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
        printf("%d\n", cpu->reg[operandA]);
        break; 

      case MUL:
        alu(cpu, ALU_MUL, operandA, operandB);
        break;

      case PUSH:
        push(cpu, operandA);
        break;

      case POP:
        pop(cpu, operandA);
        break;

      case CMP:
        alu(cpu, ALU_CMP, operandA, operandB);
        break;

      case JMP:
        cpu->PC = cpu->reg[operandA];
      // Jump to the address stored in the given register.
      // Set the `PC` to the address stored in the given register.
        continue;
      
      case JEQ:
        if (cpu->FL & 0b00000001) {
          cpu->PC = cpu->reg[operandA];
        } else {
          cpu->PC += offset;
        }
      // If `equal` flag is set (true), 
      // jump to the address stored in the given register.
        continue;

      case JNE:
        if (!(cpu->FL & 0b00000001)) {
          cpu->PC = cpu->reg[operandA];
        } else {
          cpu->PC += offset;
        }
      // If `E` flag is clear (false, 0), 
      // jump to the address stored in the given register.
        continue;
      
      case JGT:
        if (cpu->FL & 0b00000010) {
          cpu->PC = cpu->reg[operandA];
        } else {
          cpu->PC += offset;
        }
        continue;

      case JLT:
        if (cpu->FL & 0b00000100) {
          cpu->PC = cpu->reg[operandA];
        } else {
          cpu->PC += offset;
        }
        continue;

      case CALL:
        push(cpu, (cpu->PC + offset));
        cpu->PC = cpu->reg[operandA];
        break;
        // 1. The address of the ***instruction*** _directly after_ `CALL` is
        // pushed onto the stack. This allows us to return to where we left off when the subroutine finishes executing.
        // 2. The PC is set to the address stored in the given register. 
        // We jump to that location in RAM and execute the first instruction in the subroutine. 
        // The PC can move forward or backwards from its current location.

      case RET:
        pop(cpu, operandA);
        cpu->PC = cpu->reg[operandA];
        break;
        // Return from subroutine.
        // Pop the value from the top of the stack and store it in the `PC`.
      
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
  cpu->FL = 0;
  cpu->reg[7] = 0xF4;
  memset(cpu->ram, 0, sizeof(cpu->ram));
  memset(cpu->reg, 0, sizeof(cpu->reg));
}
