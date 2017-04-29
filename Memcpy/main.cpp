#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <emmintrin.h>
using namespace std;

void simple_my_memcpy(char *dst, const char *src, size_t size) {
	for (int w = 0; w < size; w++) {
		dst[w] = src[w];
	}
}

void my_memcpy_0(char *dst, const char *src, size_t size) {
	
	asm volatile (
		"1:"                   "\n\t"
			"cmpl $0, %[size];"      "\n\t"
			"je 2f;"                 "\n\t"
			
			"movb (%[src]), %%ah;"   "\n\t"
			"movb %%ah, (%[dst]);"   "\n\t"
			"decl %[size]; "         "\n\t"
			
			"incl %[src];"           "\n\t"
			"incl %[dst];"           "\n\t"
			
			"jmp 1b;"                "\n\t"
		"2:"                   "\n\t"
		
		: [src]"=r"(src), [dst]"=r"(dst), [size]"=r"(size)
		: "0"(src)   , "1"(dst)   , "2"(size)
		: "%ah", "memory"
	);
}

void my_memcpy_1(char *dst, const char *src, size_t size) {
	
	__m128i tmp;
	
	asm volatile (
		"1:"                   "\n\t"
			"movl $15, %%eax;"       "\n\t"
			"andl %[dst], %%eax;"    "\n\t"
			"cmpl $0, %%eax;"        "\n\t"
			"je 3f;"                 "\n\t"
			
			"cmpl $0, %[size];"      "\n\t"
			"je 6f;"                 "\n\t"
			
			"movb (%[src]), %%ah;"   "\n\t"
			"movb %%ah, (%[dst]);"   "\n\t"
			"decl %[size];"          "\n\t"
			
			"incl %[src];"           "\n\t"
			"incl %[dst];"           "\n\t"
			
			"jmp 1b;"                "\n\t"
		
		"3:"                   "\n\t"
			"cmpl $16, %[size];"          "\n\t"
			"jl 4f;"                      "\n\t"
			
			"movdqu (%[src]), %[tmp];"    "\n\t"
			"movntdq %[tmp], (%[dst]);"   "\n\t"
			
			"subl $16, %[size];"          "\n\t"
			
			"addl $16, %[src];"           "\n\t"
			"addl $16, %[dst];"           "\n\t"
			
			"jmp 3b;"                     "\n\t"
		
		"4:"                   "\n\t"
		
		"sfence;"              "\n\t"
		
		"5:"                   "\n\t"
			"cmpl $0, %[size];"      "\n\t"
			"je 6f;"                 "\n\t"
			
			"movb (%[src]), %%ah;"   "\n\t"
			"movb %%ah, (%[dst]);"   "\n\t"
			"decl %[size]; "         "\n\t"
			
			"incl %[src];"           "\n\t"
			"incl %[dst];"           "\n\t"
			
			"jmp 5b;"                "\n\t"
		"6:"                   "\n\t"
		
		: [src]"=r"(src), [dst]"=r"(dst), [size]"=r"(size), [tmp]"=x"(tmp)
		: "0"(src)   , "1"(dst)   , "2"(size)
		: "%eax", "%ah", "memory"
	);
}

char s1[100001], s2[100001];
char buf[100001];

//bool test(char *a, char *s, int size) {
//	for (int w = 0; w < 700; w++) {
//		a[w] = 'A';
//		s[w] = 'B';
//	}
//	
//	my_memcpy_1(s, a, size);
//	
//	for (int w = 0; w < size; w++) {
//		if (s[w] != 'A') {
//			return 1;
//		}
//	}
//	for (int w = size; w < 700; w++) {
//		if (s[w] != 'B') {
//			return 1;
//		}
//	}
//	
//	return 0;
// }


int main() {
//	freopen("input.txt", "r", stdin);
//	freopen("output.txt", "w", stdout);
	
    int q, w, e, r, t;
//    cin >> q;
    
    
    for (int w = 0; w < 200000; w++) {
		my_memcpy_1(s2, s1, 100000);
    }
    
//    for (int w = 0; w < 200000; w++) {
//		my_memcpy_0(s2, s1, 100000);
//    }
    
//    for (int w = 0; w < 200000; w++) {
//		std::memcpy(s2, s1, 100000);
//    }
    return 0;
}
