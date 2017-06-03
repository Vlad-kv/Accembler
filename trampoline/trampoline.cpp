
#include "trampoline.h"

char** trampoline_work_with_memory::root = nullptr;

/**/
typedef const char* op_t;
	op_t operations[] = {"\x48\x89\xfe" /*movq %%rdi, %%rsi;*/,
	                            "\x48\x89\xf2" /*movq %%rsi, %%rdx;*/,
	                            "\x48\x89\xd1" /*movq %%rdx, %%rcx;*/,
	                            "\x49\x89\xc8" /*movq %%rcx, %%r8;*/,
	                            "\x4d\x89\xc1" /*movq %%r8, %%r9;*/,
	                            "\x41\x51" /*push %%r9;*/};

	op_t jmp_rax = "\xff\xe0";
	op_t jmp_r9 = "\x41\xff\xe1";

	op_t call_rax = "\xff\xd0";
	op_t call_r11 = "\x41\xff\xd3";

	op_t mov_rax_in_r11 = "\x49\x89\xc3";
	op_t mov_rsp_in_rsi = "\x48\x89\xe6";
	op_t mov_r11_in_rax = "\x4c\x89\xd8";
	op_t mov_rsp_in_rax = "\x48\x89\xe0";
	op_t mov_rsi_in_rax = "\x48\x89\xf0";

	op_t mov_rax_at_addres_rdi = "\x48\x89\x07"; // movq %%rax, (%%rdi);
	op_t mov_rdi_at_addres_rsp = "\x48\x89\x3c\x24";
	op_t mov_r11_at_addres_rsp = "\x4c\x89\x1c\x24";
	op_t mov_rdi_at_addres_rsp_m8 = "\x48\x89\x7c\x24\xf8";

	op_t mov_in_rdi_const_8b = "\x48\xbf";
	op_t mov_in_rax_const_8b = "\x48\xb8";
	op_t mov_in_r8_const_8b = "\x49\xb8";
	op_t mov_in_r9_const_8b = "\x49\xb9";
	op_t mov_in_r11_const_8b = "\x49\xbb";

	op_t mov_at_addres_rdi_in_rax = "\x48\x8b\x07"; // movq (%%rdi), %%rax;
	op_t mov_at_addres_r8_in_r9 = "\x4d\x8b\x08"; // movq (%%r8), %%r9;
	op_t mov_at_addres_rsp_in_rdi = "\x48\x8b\x3c\x24";
	op_t mov_at_addres_rsi_in_r11 = "\x4c\x8b\x1e";


	op_t pop_rax = "\x58";
	op_t pop_rdi = "\x5f";
	op_t pop_rsi = "\x5e";
	op_t pop_r9 = "\x41\x59";
	op_t pop_r11 = "\x41\x5b";


	op_t push_rax = "\x50";
	op_t push_rdi = "\x57";
	op_t push_r9 = "\x41\x51";
	op_t push_r11 = "\x41\x53";

	op_t push_0x0 = "\x6a\x00";


	op_t sub_rsi_const_4b = "\x48\x81\xee";
	op_t sub_rsp_const_4b = "\x48\x81\xec";
	op_t add_rsp_const_4b = "\x48\x81\xc4";
	op_t add_rsi_const_4b = "\x48\x81\xc6";

	op_t sub_rax_rsi = "\x48\x29\xf0";


	op_t cmp_rsi_rsp = "\x48\x39\xe6";

	op_t je = "\x74";
	op_t jmp = "\xeb";

	op_t ret = "\xc3";

	op_t int3 = "\xCC";
	op_t nop = "\x90";
