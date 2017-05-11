#include <iostream>
#include <unistd.h>
//#include <sys/mman.h>
#include <cstdio>

using namespace std;

template<typename... Args>
struct num_inter_args;

template<>
struct num_inter_args<> {
	const static int val = 0;
};

template<typename First, typename... Other>
struct num_inter_args<First, Other...> {
	const static int val = num_inter_args<Other...>::val + 1;
};

template<typename... Other>
struct num_inter_args<float, Other...> {
	const static int val = num_inter_args<Other...>::val;
};

template<typename... Other>
struct num_inter_args<double, Other...> {
	const static int val = num_inter_args<Other...>::val;
};

template <typename T>
struct trampoline
{
    template <typename F>
    trampoline(F func)
    {}
    
    ~trampoline();
    
    T* get() const;
};

const char* operations[] = {"\x48\x89\xfe" /*movq %%rdi, %%rsi;*/,
							"\x48\x89\xf2" /*movq %%rsi, %%rdx;*/,
							"\x48\x89\xd1" /*movq %%rdx, %%rcx;*/,
							"\x49\x89\xc8" /*movq %%rcx, %%r8;*/,
							"\x4d\x89\xc1" /*movq %%r8, %%r9;*/,
							"\x41\x51" /*push %%r9;*/};
const char* pop_r9 = "\x41\x59";
const char* jmp_rax = "\xff\xe0";

template <typename R, typename... Args>
struct trampoline<R (Args...)> {
private:
	
	static void add_op(char* &ptr, const char* operation) {
		for (char *i = operation; *i; ++i) {
			*(ptr++) = *i;
		}
	}
	
public:
    template <typename F>
    trampoline(F const& func)
        : func_obj(new F(func))
        , caller(&do_call<F>) {
//        code = mmap(nullptr, 4096, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        char* pcode = (char*)code;
        
        for (int i = max(num_inter_args<Args...>::val, 6) - 1; i >= 0; i--) {
			add_op(pcode, operations[i]);
        }
        
        // 48BF         mov rdi, imm
        *pcode++ = 0x48;
        *pcode++ = 0xbf;
        *(void**)pcode = func_obj;
        pcode += 8;
        // 48B8         mov rax, imm
        *pcode++ = 0x48;
        *pcode++ = 0xb8;
        *(void**)pcode = (void*)&do_call<F>;
        pcode += 8;
        
        add_op(pcode, jmp_rax);
        
        if (num_inter_args<Args...>::val >= 6) {
			add_op(pcode, pop_r9);
        }
    }
    
    template <typename F>
    static R do_call(void* obj, Args ...args) {
        return (*(F*)obj)(args...);
    }
    
    R (*get() const)(Args ...args) {
        return (R (*)(Args ...args))code;
    }
    
    ~trampoline() {
    	
    }
    
private:
    void* func_obj;
    void* code;
    R (*caller)(void* obj, Args ...args);
};


int main() {
//    int b = 123;
//    
//    trampoline<int (int)> tr([&](int a) { return printf("%d %d\n", a, b); });
//    auto p = tr.get();
//    
//    p(5);
//    b = 124;
//    
//    p(6);
}
