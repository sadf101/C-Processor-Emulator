#include <stdio.h>

/*Datatype Definitions*/
typedef unsigned char byte;
typedef unsigned int memAddr;
typedef unsigned char regAddr;
typedef unsigned char bool;

/*Macros*/
#define true 1
#define false 0
#define registers 4
#define memorySize 2048
#define instructionLength 4

/*Emulated System Data*/
memAddr programCounter;
bool zeroFlag;
bool signFlag;
bool running;
byte registerFile[registers];
byte memoryFile[memorySize];

/*todo  -  Macro the switch cases as the instruction names and replace in decodeExecuteInstruction()
enum instruction {
  HALT,
  LDR,
  LDV,
  LDM,
  ST,
  ADD,
  SUB,
  MUL,
  DIV,
  CMP,
  JE,
  JNE,
  JG,
  JL,
  SL,
  SR,
  AND,
  OR,
  XOR
};*/

void init(void) {
  programCounter = 0;
  running = true;
}

/*todo  -  put instruction functions into separate module*/
void halt(void) {
  running = false;
}

void loadFromReg(regAddr dest, regAddr src) {
  registerFile[dest] = registerFile[src];
}

void loadValue(regAddr dest, regAddr value) {
  registerFile[dest] = value;
}

void loadFromMem(regAddr dest, memAddr address) {
  registerFile[dest] = memoryFile[address];
}

void store(memAddr dest, regAddr src) {
  memoryFile[dest] = registerFile[src];
}

void add(regAddr dest, regAddr srca, regAddr srcb) {
  registerFile[dest] = registerFile[srca] + registerFile[srcb];
}

void subtract(regAddr dest, regAddr srca, regAddr srcb) {
  registerFile[dest] = registerFile[srca] - registerFile[srcb];
}

void multiply(regAddr dest, regAddr srca, regAddr srcb) {
  registerFile[dest] = registerFile[srca] * registerFile[srcb];
}

void divide(regAddr dest, regAddr srca, regAddr srcb) {
  registerFile[dest] = registerFile[srca] / registerFile[srcb];
}

void compare(regAddr srca, regAddr srcb) {
  byte comparison = registerFile[srca] - registerFile[srcb];
  zeroFlag = false;
  signFlag = false;
  if (comparison < 0) {
    signFlag = true;
  } else if (comparison == 0) {
    zeroFlag = true;
  }
}

void jump(int address) {
  programCounter = address;
}

void jumpEqual(int address) {
  if (zeroFlag) {
    jump(address);
  }
}

void jumpNotEqual(int address) {
  if (!zeroFlag) {
    jump(address);
  }
}

void jumpGreater(int address) {
  if (!zeroFlag && !signFlag) {
    jump(address);
  }
}

void jumpLess(int address) {
  if (zeroFlag && signFlag) {
    jump(address);
  }
}

void shiftLeft(regAddr src, memAddr n) {
  registerFile[src] = registerFile[src] << n;
}

void shiftRight(regAddr src, memAddr n) {
  registerFile[src] = registerFile[src] >> n;
}

void and(regAddr srca, regAddr srcb) {
  registerFile[srca] = registerFile[srca] & registerFile[srcb];
}

void or(regAddr srca, regAddr srcb) {
  registerFile[srca] = registerFile[srca] | registerFile[srcb];
}

void xor(regAddr srca, regAddr srcb) {
  registerFile[srca] = registerFile[srca] ^ registerFile[srcb];
}

void rotateLeft(regAddr src, byte n) {
  memAddr temp = registerFile[src] << n;
  registerFile[src] = temp >> 8 | temp;
}

void rotateRight(regAddr src, byte n) {
  memAddr temp = registerFile[src] >> n;
  registerFile[src] = temp << 8 | temp;
}

byte *fetchInstruction() {
  return &memoryFile[programCounter];
}

/*todo  -  macro the magic numbers here*/
void decodeExecuteInstruction(byte *currentInstruction) {
  byte leastSigByte = *currentInstruction;
  if (leastSigByte == 0) {
    halt();
  } else {
    switch (leastSigByte & 0xF0) {
      case 0x10:
        loadFromReg((leastSigByte & 0xF3) >> 2, leastSigByte & 0xFC);
        break;
      case 0x20:
        store(*(currentInstruction + 1) << 8 | *(currentInstruction + 2), leastSigByte & 0xFC);
        break;
      case 0x30:
        add((leastSigByte & 0xF3) >> 2, leastSigByte & 0xFC, *(currentInstruction + 1) & 0x03);
        break;
      case 0x40:
        subtract((leastSigByte & 0xF3) >> 2, leastSigByte & 0xFC, *(currentInstruction + 1) & 0x03);
        break;
      case 0x50:
        multiply((leastSigByte & 0xF3) >> 2, leastSigByte & 0xFC, *(currentInstruction + 1) & 0x03);
        break;
      case 0x60:
        divide((leastSigByte & 0xF3) >> 2, leastSigByte & 0xFC, *(currentInstruction + 1) & 0x03);
        break;
      case 0x70:
        compare((leastSigByte & 0xF3) >> 2, leastSigByte & 0xFC);
        break;
      case 0x80:
        switch (leastSigByte & 0x0F) {
          case 0x00:
            jump(*(currentInstruction + 1) << 8 | *(currentInstruction + 2));
            break;
          case 0x01:
            jumpEqual(*(currentInstruction + 1) << 8 | *(currentInstruction + 2));
            break;
          case 0x02:
            jumpNotEqual(*(currentInstruction + 1) << 8 | *(currentInstruction + 2));
            break;
          case 0x03:
            jumpGreater(*(currentInstruction + 1) << 8 | *(currentInstruction + 2));
            break;
          case 0x04:
            jumpLess(*(currentInstruction + 1) << 8 | *(currentInstruction + 2));
            break;
          case 0x05:
            shiftLeft((*(currentInstruction + 1) & 0b00110000) >> 4, *(currentInstruction + 1) & 0b00000111);
            break;
          case 0x06:
            shiftRight((*(currentInstruction + 1) & 0b00110000) >> 4, *(currentInstruction + 1) & 0b00000111);
            break;
        }
        break;
      case 0x90:
        if (leastSigByte & 0x04) {
          loadFromMem(leastSigByte & 0xFC, *(currentInstruction + 1) << 8 | *(currentInstruction + 2));
        } else {
          loadValue(leastSigByte & 0xFC, *(currentInstruction + 1));
        }
        break;
      case 0xA0:
        break;
      case 0xC0:
      and((leastSigByte & 0xF3) >> 2, leastSigByte & 0xFC);
        break;
      case 0xD0:
      or((leastSigByte & 0xF3) >> 2, leastSigByte & 0xFC);
        break;
      case 0xE0:
      xor((leastSigByte & 0xF3) >> 2, leastSigByte & 0xFC);
        break;
    }
  }
}

void incProgramCounter(void) {
  programCounter += instructionLength;
}

void printState(void) {
  for (int i = 0; i < registers; i++) {
    printf("%x\n", registerFile[i]);
  }
  printf("%x\n", programCounter);
}

void emulate(void) {
  init();
  while (running) {
    decodeExecuteInstruction(fetchInstruction());
    printState();
    incProgramCounter();
  }
}

/*todo  -  fill with program loading code*/
void loadProgram(void) {
}

int main() {
  printf("***PROGRAM_LOADING***\n");
  loadProgram();
  printf("***PROGRAM_LOADED***\n");
  printf("***EMULATION_START***\n");
  emulate();
  printf("***EMULATION_COMPLETE***\n");
}
