#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include "debug.h"
#include <stdbool.h>
#include <assert.h>

typedef enum {
  NOT_INUSE = 0,
  INUSE4,
  INUSE8
} RegState;

// Up to 4 args support
// Argument Registers
typedef enum {
  rdi = 0,
  rsi,
  rdx,
  rcx,
  ArgRegs_end
} ArgReg;

// General Purpose Regs
typedef enum {
  rbx = 0,
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  r15,
  GenRegs_end
} GenReg;

// TODO: if this ends up only needing one field, simplify
typedef struct {
  RegState inuse[GenRegs_end];
} RegisterManager_t;

RegisterManager_t *init_register_manager(void);
char *get_arg_register(int arg_n, size_t size);
char *get_register(RegisterManager_t *reg_manager, size_t size);
char *get_used_register(RegisterManager_t *reg_manager, int last_used_idx, size_t size);
void free_register(RegisterManager_t *reg_manager, char *reg);
char *argreg_enum_to_str(GenReg reg, size_t size);
char *genreg_enum_to_str(GenReg reg, size_t size);
GenReg genreg_str_to_enum(char *reg);

#endif