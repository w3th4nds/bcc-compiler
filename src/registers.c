#include "include/registers.h"

RegisterManager_t *init_register_manager(void)
{
  RegisterManager_t *reg_manager = calloc(1, sizeof(RegisterManager_t));
  return reg_manager;
}

char *get_param_register(int param_n)
{
  return paramreg_enum_to_str(param_n);
}

char *get_register(RegisterManager_t *reg_manager)
{
  for (GenReg i = 0; i < GenRegs_end; ++i) {
    if (reg_manager->inuse[i] == false) {
      reg_manager->inuse[i] = true;
      return genreg_enum_to_str(i);
    }
  }
  error_exit("get_register() - out of registers\n");
  return NULL;
}

void free_register(RegisterManager_t *reg_manager, char *reg)
{
  reg_manager->inuse[genreg_str_to_enum(reg)] = false;
}

char *paramreg_enum_to_str(GenReg reg)
{
  switch(reg) {
    case rdi: return "rdi";
    case rsi: return "rsi";
    case rdx: return "rdx";
    case rcx: return "rcx";
    default: error_exit("paramreg_enum_to_str() - unknown reg enum\n");
  }
  return NULL;
}

char *genreg_enum_to_str(GenReg reg)
{
  switch(reg) {
    case rbx: return "rbx";
    case r8:  return "r8";
    case r9:  return "r9";
    case r10: return "r10";
    case r11: return "r11";
    case r12: return "r12";
    case r13: return "r13";
    case r14: return "r14";
    case r15: return "r15";
    default: error_exit("genreg_enum_to_str() - unknown reg enum\n");
  }
  return NULL;
}


int genreg_str_to_enum(char *reg)
{
  for (GenReg i = 0; i < GenRegs_end; ++i) {
    if (strcmp(reg, genreg_enum_to_str(i)) == 0)
      return i;
  }
  error_exit("reg_str_to_enum() - reg str is not a register\n");
  return -1;
}