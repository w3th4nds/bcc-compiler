#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include "debug.h"
#include <stdbool.h>

// Up to 4 args support
// Parameter Registers
typedef enum {
  rdi = 0,
  rsi,
  rdx,
  rcx,
  ParamRegs_end
} ParamReg;

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

typedef struct {
  bool inuse[GenRegs_end];
} RegisterManager_t;

RegisterManager_t *init_register_manager(void);
char *get_param_register(int param_n);
char *get_register(RegisterManager_t *reg_manager);
char *get_used_register(RegisterManager_t *reg_manager, int last_used_idx);
void free_register(RegisterManager_t *reg_manager, char *reg);
char *paramreg_enum_to_str(GenReg reg);
char *genreg_enum_to_str(GenReg reg);
int genreg_str_to_enum(char *reg);

#endif