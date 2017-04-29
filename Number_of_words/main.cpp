#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <time.h>

#include <immintrin.h>

#include <emmintrin.h>
using namespace std;

int calc_number_of_words_simple(const char *str) {
	int ans = 0;
	bool prev = 1;
	for (; *str; ++str) {
		if (*str == ' ') {
			ans += prev ^ 1;
			prev = 1;
		} else {
			prev = 0;
		}
	}
	if (prev == 0) {
		ans++;
	}
	return ans;
}

int calc_number_of_words(const char *str) {
	__m128i spaces, zero, xmm0, xmm1;
	char c_spaces[16];
	for (int w = 0; w < 16; w++) {
		c_spaces[w] = ' ';
	}
	int address_sp = (int)c_spaces;
	int ans = 0;
	
	asm volatile (
		"xorps %[zero], %[zero];"            "\n\t"
		"movdqa (%[address_sp]), %[spaces];" "\n\t"   // прочитать
				
		"movl $1, %%edx;"                    "\n\t" // edx - самый последний бит.
		
		"1:"                                 "\n\t"
			"movl $15, %%eax;"               "\n\t"
			"andl %[str], %%eax;"            "\n\t"
			"cmpl $0, %%eax;"                "\n\t"
			"je 3f;"                         "\n\t"
			
			"movb (%[str]), %%al;"           "\n\t"
			"cmpb $0, %%al;"                 "\n\t"
			"je 6f;"                         "\n\t"
			
			"movb (%[str]), %%ah;"           "\n\t"
			
			"movl %%edx, %%ecx;"             "\n\t"
			"xorl %%edx, %%edx;"             "\n\t"
			"cmpb $32, %%ah;"                "\n\t"
			
			"jg 98f;"   "\n\t"               "\n\t"
				"incl %[ans];"               "\n\t"
				"subl %%ecx, %[ans];"        "\n\t"
				
				"incl %%edx;"                "\n\t"
			"98:"                            "\n\t"
			
			"incl %[str];"                   "\n\t"
			"jmp 1b;"                        "\n\t"
		"3:"                                 "\n\t"
			
			"movdqa (%[str]), %[xmm0];"      "\n\t"
			"movdqa %[xmm0], %[xmm1];"       "\n\t"
			
			"pcmpeqb %[zero], %[xmm1];"      "\n\t"
			"pmovmskb %[xmm1], %%eax;"       "\n\t"
			
			"cmpl $0, %%eax;"                "\n\t"
			"jg 5f;"                         "\n\t"
			
			"pcmpeqb %[spaces], %[xmm0];"    "\n\t"
			"palignr $1, %[xmm0], %[xmm1];"  "\n\t"  // сдвиг
			
			"pmovmskb %[xmm0], %%eax;"       "\n\t" // маска заковых битов
			"pmovmskb %[xmm1], %%ecx;"       "\n\t"
			
			"xorl $1, %%edx;"                "\n\t"
			"andl %%eax, %%edx;"             "\n\t"
			"addl %%edx, %[ans];"            "\n\t"
			
			"andl %%eax, %%ecx;"             "\n\t"
			"andl $65534, %%eax;"            "\n\t"
			
			"popcnt %%ecx, %%ecx;"           "\n\t"
			"subl %%ecx, %[ans];"            "\n\t"
			
			"popcnt %%eax, %%ecx;"           "\n\t"
			"addl %%ecx, %[ans];"            "\n\t"
			
			"shrl $15, %%eax;"               "\n\t"
			"movl %%eax, %%edx;"             "\n\t"
			
			"addl $16, %[str];"              "\n\t"
			"jmp 3b;"                        "\n\t"
		"5:"                                 "\n\t"
			"movb (%[str]), %%al;"           "\n\t"
			"cmpb $0, %%al;"                 "\n\t"
			"je 6f;"                         "\n\t"
			
			"movb (%[str]), %%ah;"           "\n\t"
			
			"movl %%edx, %%ecx;"             "\n\t"
			"xorl %%edx, %%edx;"             "\n\t"
			"cmpb $32, %%ah;"                "\n\t"
			
			"jg 98f;"                        "\n\t"
				"incl %[ans];"               "\n\t"
				"subl %%ecx, %[ans];"        "\n\t"
				
				"incl %%edx;"                "\n\t"
			"98:"                            "\n\t"
			
			"incl %[str];"                   "\n\t"
			
			"jmp 5b;"                        "\n\t"
		"6:"                                 "\n\t"
		
		"xorl $1, %%edx;"                    "\n\t"
		"addl %%edx, %[ans];"                "\n\t"
		
		: [str]"=r"(str), [spaces]"=x"(spaces), [xmm1]"=x"(xmm1), [address_sp]"=r"(address_sp),
			[ans]"=r"(ans), [xmm0]"=x"(xmm0), [zero]"=x"(zero)
		: "0"(str), "3"(address_sp), "4"(ans)
		: "%eax", "%ecx", "%edx", "memory"
	);
	return ans;
}

char s1[100001], s2[100001];
char buf[100001];

int main() {
//	freopen("input.txt", "r", stdin);
//	freopen("output.txt", "w", stdout);
    int q, w, e, r, t;
    
    srand(time(0));
    
//    cout << calc_number_of_words(" ");
    
    
    for (int test = 0; test < 1000; test++) {
		int len = 500;
		for (w = 0; w < len; w++) {
			if (rand() % 2) {
				s1[w] = ' ';
			} else {
				s1[w] = 'a';
			}
		}
		s1[len] = 0;
		
		for (int e = 0; e < len; e++) {
			int res_1 = calc_number_of_words(s1 + e);
			int res_2 = calc_number_of_words_simple(s1 + e);
			
			if (res_1 != res_2) {
				cout << "WA " << res_1 << " " << res_2 << "\n";
				cout << s1 + e << "|\n";
				return 0;
			}
		}
    }
	
	cout << "OK\n";
	
	
//	for (w = 1000; w < 100000; w++) {
//		s1[w] = s1[w - 35];
//	}
//	s1[100000] = 0;
//    
//    for (w = 0; w < 100000; w++) {
//		calc_number_of_words(s1);
//    }
    return 0;
}
