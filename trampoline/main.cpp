#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
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
const char* push_rax = "\x50";
const char* pop_rax = "\x58";
const char* call_rax = "\xff\xd0";
const char* mov_in_rdi_smth8 = "\x48\xbf";
const char* mov_in_rax_smth8 = "\x48\xb8";
const char* mov_rax_at_addres_rdi = "\x48\x89\x07"; // movq %%rax, (%%rdi);
const char* mov_at_addres_rdi_in_rax = "\x48\x8b\x07"; // movq (%%rdi), %%rax;

const char* mov_in_r8_smth8 = "\x49\xb8";
const char* mov_at_r8_rdi_in_r9 = "\x4d\x8b\x08"; // movq (%%r8), %%r9;
const char* jmp_r9 = "\x41\xff\xe1";

struct func_obj {
    int operator()(int c1, int c2, int c3, int c4, int c5) {
        cout << "in func_obj()\n";
        return 2345619;
    }
};

void call_3(int c1, int c2, int c3) {

}

void call_2(int c1, int c2) {
    void *ptr = (void*) &call_3;

    ((void (*)(int, int, int))ptr)(1, c1, c2);
}

const int shift = 8;

template <typename R, typename... Args>
struct trampoline<R (Args...)> {
private:
    
    static void add_op(char* &ptr, const char* operation) {
        for (const char *i = operation; *i; ++i) {
            *(ptr++) = *i;
        }
    }
    
public:
    template <typename F>
    trampoline(F const& func)
        : func_obj(new F(func))
        , caller(&do_call<F>) {

        code = mmap(nullptr, 4096, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        char* pcode = ((char*)code) + shift;
        


        *pcode++ = 0x58; // pop rax;

        for (int i = min((int)num_inter_args<Args...>::val, 6) - 1; i >= 0; i--) {
            add_op(pcode, operations[i]);
        }
        
        add_op(pcode, mov_in_rdi_smth8);
        *(void**)pcode = code;
        pcode += 8;

        add_op(pcode, mov_rax_at_addres_rdi); // сохранить значение вовзврата.

        add_op(pcode, mov_in_rax_smth8);
        char *to_ret = pcode;
        pcode += 8;

        add_op(pcode, push_rax);

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

        *(void**)to_ret = (void*)pcode;


        if (num_inter_args<Args...>::val >= 6) {
            add_op(pcode, pop_r9);
        }

        add_op(pcode, mov_in_r8_smth8);  // адрес откуда доставать
        *(void**)pcode = code;
        pcode += 8;

        add_op(pcode, mov_at_r8_rdi_in_r9); // достать
        add_op(pcode, jmp_r9);
    }
    
    template <typename F>
    static void* get_ptr(void* obj) {
        
        void *ptr = (void*)&(*(F*)obj);
        return ptr;
    }

    template <typename F>
    static R do_call(void* obj, Args ...args) {
        cout << "in do_call\n";

        R temp = (*(F*)obj)(args...);
        return temp;
    }
    
    R (*get() const)(Args ...args) {
        return (R (*)(Args ...args))(  ((char*)code) + shift  );
    }
    
    ~trampoline() {
        munmap(code, 4096);
    }
    
private:
    void* func_obj;
    void* code;
    R (*caller)(void* obj, Args ...args);
};

int test(int c1, int c2, int c3, int c4, int c5, int c6) {
    return 0;
}

int main() {
    /*
    {
        int b = 123;
        func_obj fo;

        trampoline<int (int, int, int, int, int)> tr(fo);
        auto p = tr.get();
        try {
            p(1, 2, 3, 4, 5);
        } catch (...) {
            //cout << "catched : " << str << "\n";
        }
        b = 124;

        //int new_i = 999;
        //int res = p(2, 3, 4, 5, 6, 7);
        //cout << res << "\n";
        //cout << b << " " << new_i << "\n";
    }
    */

    {
        int b = 123;

        trampoline<int (int, int, int, int, int, int)>
                tr([&](int c1, int c2, int c3, int c4, int c5, int c6) {
                    cout << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " |\n";
                    return 0;
                }
            );
        auto p = tr.get();
        
        cout << "???\n";

        p(1, 2, 3, 4, 5, 6);
        cout << "after\n";

        b = 124;

        int new_i = 999;

        cout << "???_2\n";
        p(6, 5, 4, 3, 2, 1);
        cout << "after_2\n";
        cout << b << " " << new_i << "\n";
    }

}

// g++ -o main.exe main.cpp -std=c++11
