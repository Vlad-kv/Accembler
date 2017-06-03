#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <cmath>

typedef const char* op_t;
extern op_t operations[];

extern op_t jmp_rax;
extern op_t jmp_r9;

extern op_t call_rax;
extern op_t call_r11;

extern op_t mov_rax_in_r11;
extern op_t mov_rsp_in_rsi;
extern op_t mov_r11_in_rax;
extern op_t mov_rsp_in_rax;
extern op_t mov_rsi_in_rax;

extern op_t mov_rax_at_addres_rdi; // movq %%rax, (%%rdi);
extern op_t mov_rdi_at_addres_rsp;
extern op_t mov_r11_at_addres_rsp;

extern op_t mov_in_rdi_const_8b;
extern op_t mov_in_rax_const_8b;
extern op_t mov_in_r8_const_8b;
extern op_t mov_in_r9_const_8b;
extern op_t mov_in_r11_const_8b;

extern op_t mov_at_addres_rdi_in_rax; // movq (%%rdi), %%rax;
extern op_t mov_at_addres_r8_in_r9; // movq (%%r8), %%r9;
extern op_t mov_at_addres_rsp_in_rdi;
extern op_t mov_at_addres_rsi_in_r11;


extern op_t pop_rax;
extern op_t pop_rdi;
extern op_t pop_rsi;
extern op_t pop_r9;
extern op_t pop_r11;


extern op_t push_rax;
extern op_t push_rdi;
extern op_t push_r9;
extern op_t push_r11;

extern op_t push_0x0;

extern op_t sub_rsi_const_4b;
extern op_t sub_rsp_const_4b;
extern op_t add_rsp_const_4b;
extern op_t add_rsi_const_4b;

extern op_t sub_rax_rsi;

extern op_t cmp_rsi_rsp;

extern op_t je;
extern op_t jmp;

extern op_t ret;

extern op_t int3;
extern op_t nop;

template<typename... Args>
struct args_info;

template<>
struct args_info<> {
    const static int num_fractional_args = 0;
    const static int num_not_fract_args = 0;
    const static bool is_valid = 1;
};

template<typename First, typename... Other>
struct args_info<First, Other...> {
    const static int num_fractional_args = args_info<Other...>::num_fractional_args;
    const static int num_not_fract_args = args_info<Other...>::num_not_fract_args + 1;
    const static bool is_valid = (sizeof(First) <= 8) & args_info<Other...>::is_valid;
};

template<typename... Other>
struct args_info<float, Other...> {
    const static int num_fractional_args = args_info<Other...>::num_fractional_args + 1;
    const static int num_not_fract_args = args_info<Other...>::num_not_fract_args;
    const static bool is_valid = args_info<Other...>::is_valid;
};

template<typename... Other>
struct args_info<double, Other...> {
    const static int num_fractional_args = args_info<Other...>::num_fractional_args + 1;
    const static int num_not_fract_args = args_info<Other...>::num_not_fract_args;
    const static bool is_valid = args_info<Other...>::is_valid;
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

template <typename R, typename... Args>
struct trampoline<R (Args...)> {
private:
    static const int num_of_pages = 1;

    static void add(char* &ptr, const char* operation) {
        for (const char *i = operation; *i; ++i) {
            *(ptr++) = *i;
        }
    }

    static void add_8(char* &ptr, const char* operation, void* data) {
        add(ptr, operation);
        *(void**)ptr = data;
        ptr += 8;
    }

    static void add_4(char* &ptr, const char* operation, int32_t data) {
        add(ptr, operation);
        *(int32_t*)ptr = data;
        ptr += 4;
    }
    
public:
    template <typename F>
    trampoline(F const& func)
        : func_obj(new F(func))
        , caller(&do_call<F>)
        , deleter(delete_obj<F>)
         {

        if (!args_info<Args...>::is_valid) {
            delete (F*)func_obj;
            throw std::string("Arguments with size > 8 bytes are unsupported.");
        }
        
        code = mmap(nullptr, 4096, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        char *p = (char*)code;

        if (args_info<Args...>::num_not_fract_args >= 6) {
            add(p, pop_r11);
            add(p, push_rax);
            for (int w = 5; w >= 1; w--) {
                add(p, operations[w]);
            }
            add(p, push_rdi);
            //  rdi и rsi свободны, в r11 - адрес возврата, на вершине стека - первый аргумент.
            int size = 8 * (args_info<Args...>::num_not_fract_args - 6 +
                            std::max(args_info<Args...>::num_fractional_args - 8, 0));
            //cout << size << "   !!!!!\n";

            add(p, mov_rsp_in_rsi);
            add_4(p, add_rsi_const_4b, size + 8 * 2);
            add_4(p, add_rsp_const_4b, 8 * 2);

            char* label0 = p;

            add(p, cmp_rsi_rsp);
            add(p, je);
            char* pos_for_je = p;
            p++;

            {
                add_4(p, add_rsp_const_4b, 8);
                add(p, mov_at_addres_rsp_in_rdi);
                add_4(p, sub_rsp_const_4b, 8);
                add(p, mov_rdi_at_addres_rsp);
                add_4(p, add_rsp_const_4b, 8);
                add(p, jmp);
                *p = (label0 - (p + 1));
                p++;
            }
            (*pos_for_je) = (char)(p - (pos_for_je + 1));
            
            add(p, mov_r11_at_addres_rsp);
            add_4(p, sub_rsp_const_4b, size + 8 * 2);
            add(p, pop_rsi);

            add_8(p, mov_in_rdi_const_8b, func_obj);
            add_8(p, mov_in_r11_const_8b, (void*)&do_call<F>);

            add(p, call_r11);

            add(p, pop_r9);
            
            add(p, mov_rsp_in_rsi);
            add_4(p, add_rsi_const_4b, size);

            add(p, mov_at_addres_rsi_in_r11);
            add(p, pop_r9);
            add(p, push_r11);

            add(p, ret);

            std::cout << (int)(p - (char*)code) << "\n\n";
        } else {
            for (int w = args_info<Args...>::num_not_fract_args - 1; w >= 0; w--) {
                add(p, operations[w]);
            }
            add_8(p, mov_in_rdi_const_8b, func_obj);
            add_8(p, mov_in_r9_const_8b, (void*)&do_call<F>);

            add(p, jmp_r9);
        }
    }

    template <typename F>
    static R do_call(void* obj, Args ...args) {
        std::cout << "in do_call\n";

        return (*(F*)obj)(args...);
    }
    
    R (*get() const)(Args ...args) {
        return (R (*)(Args ...args))code;
    }
    
    ~trampoline() {
        munmap(code, 4096);
        deleter(func_obj);
    }
    
private:
    template <typename F>
    static void delete_obj(void *p) {
        delete static_cast<F*>(p);
    }

    void* func_obj;
    void* code;
    R (*caller)(void* obj, Args ...args);
    void (*deleter)(void*);

    //static vector<int> 
};

#endif //TRAMPOLINE_H