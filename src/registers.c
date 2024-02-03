#include "include/registers.h"

RegisterManager_t *init_register_manager(void)
{
  RegisterManager_t *reg_manager = calloc(1, sizeof(RegisterManager_t));
  return reg_manager;
}

char *get_param_register(int param_n, size_t size)
{
  assert(param_n < ParamRegs_end && "Up to 4 parameters supported");
  return paramreg_enum_to_str(param_n, size);
}

char *get_register(RegisterManager_t *reg_manager, size_t size)
{
  for (GenReg i = 0; i < GenRegs_end; ++i) {
    if (reg_manager->inuse[i] == NOT_INUSE) {
      reg_manager->inuse[i] = size == 8 ? INUSE8 : INUSE4;
      return genreg_enum_to_str(i, size);
    }
  }
  error_exit("get_register() - out of registers\n");
  return NULL;
}

// last_used_idx - choose which used reg to get
// TODO: make less confusing
char *get_used_register(RegisterManager_t *reg_manager, int last_used_idx, size_t size)
{
  int n = 1;
  for (GenReg i = (GenRegs_end-1); i >= 0; --i) {
    if (reg_manager->inuse[i] != NOT_INUSE && n++ == last_used_idx)
      return genreg_enum_to_str(i, size);
  }
  error_exit("get_used_register() - could not find requested register\n");
  return NULL;
}

void free_register(RegisterManager_t *reg_manager, char *reg)
{
  reg_manager->inuse[genreg_str_to_enum(reg)] = NOT_INUSE;
}

char *paramreg_enum_to_str(GenReg reg, size_t size)
{
  switch(reg) {
    case rdi: return size == 8 ? "rdi" : "edi";
    case rsi: return size == 8 ? "rsi" : "esi";
    case rdx: return size == 8 ? "rdx" : "edx";
    case rcx: return size == 8 ? "rcx" : "ecx";
    default: 
      printf("reg = %d\n", reg);
      error_exit("paramreg_enum_to_str() - unknown reg enum\n");
  }
  return NULL;
}

char *genreg_enum_to_str(GenReg reg, size_t size)
{
  // nasm does not support r8l - r15l
  // so only rbx gets 4-byte variant
  switch(reg) {
    case rbx: return size == 8 ? "rbx" : "ebx";
    case r8:  return "r8";
    case r9:  return "r9";
    case r10: return "r10";
    case r11: return "r11";
    case r12: return "r12";
    case r13: return "r13";
    case r14: return "r14";
    case r15: return "r15";
    default: 
      printf("reg = %d\n", reg);
      error_exit("genreg_enum_to_str() - unknown reg enum\n");
  }
  return NULL;
}

GenReg genreg_str_to_enum(char *reg)
{
  for (GenReg i = 0; i < GenRegs_end; ++i) {
    // almost all 4-byte variants are not supported by nasm
    // but this is fine since genreg_str_to_enum()
    // is only used by free_register() to set RegState to NOT_INUSE
    if (strcmp(reg, genreg_enum_to_str(i, 8)) == 0 || \
        strcmp(reg, genreg_enum_to_str(i, 4)) == 0)
      return i;
  }
  printf("reg = %s\n", reg);
  error_exit("reg_str_to_enum() - reg str is not a register\n");
  return -1;
}