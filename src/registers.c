#include "include/registers.h"

RegisterManager_t *init_register_manager(void)
{
  RegisterManager_t *reg_manager = calloc(1, sizeof(RegisterManager_t));
  return reg_manager;
}

char *get_arg_register(int arg_n, size_t size)
{
  assert(arg_n < ArgRegs_end && "Up to 4 arguments supported");
  return argreg_enum_to_str(arg_n, size);
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

// same as get_used_register(), but return the its size
// used by binops to determine register size to use for the operation result
size_t get_used_register_size(RegisterManager_t *reg_manager, int last_used_idx)
{
  int n = 1;
  for (GenReg i = (GenRegs_end-1); i >= 0; --i) {
    if (reg_manager->inuse[i] != NOT_INUSE && n++ == last_used_idx)
      return reg_manager->inuse[i] == INUSE8 ? 8 : 4;
  }
  error_exit("get_used_register() - could not find requested register\n");
  return 0;
}

void free_register(RegisterManager_t *reg_manager, char *reg)
{
  reg_manager->inuse[genreg_str_to_enum(reg)] = NOT_INUSE;
}

char *argreg_enum_to_str(ArgReg reg, size_t size)
{
  switch(reg) {
    case rdi: return size == 8 ? "rdi" : "edi";
    case rsi: return size == 8 ? "rsi" : "esi";
    case rdx: return size == 8 ? "rdx" : "edx";
    case rcx: return size == 8 ? "rcx" : "ecx";
    default: 
      printf("reg = %d\n", reg);
      error_exit("argreg_enum_to_str() - unknown reg enum\n");
  }
  return NULL;
}

char *genreg_enum_to_str(GenReg reg, size_t size)
{
  // https://www.cs.uaf.edu/2017/fall/cs301/reference/x86_64.html nasm cheat sheet
  // not supporting anything lower than 32-bit
  switch(reg) {
    case rbx: return size == 8 ? "rbx" : "ebx";
    case r8:  return size == 8 ? "r8"  : "r8d";
    case r9:  return size == 8 ? "r9"  : "r9d";
    case r10: return size == 8 ? "r10" : "r10d";
    case r11: return size == 8 ? "r11" : "r11d";
    case r12: return size == 8 ? "r12" : "r12d";
    case r13: return size == 8 ? "r13" : "r13d";
    case r14: return size == 8 ? "r14" : "r14d";
    case r15: return size == 8 ? "r15" : "r15d";
    default: 
      printf("reg = %d\n", reg);
      error_exit("genreg_enum_to_str() - unknown reg enum\n");
  }
  return NULL;
}

GenReg genreg_str_to_enum(char *reg)
{
  for (GenReg i = 0; i < GenRegs_end; ++i) {
    if (strcmp(reg, genreg_enum_to_str(i, 8)) == 0 || \
        strcmp(reg, genreg_enum_to_str(i, 4)) == 0)
      return i;
  }
  printf("reg = %s\n", reg);
  error_exit("reg_str_to_enum() - reg str is not a register\n");
  return -1;
}