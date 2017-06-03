#include <iostream>
#include <cstdio>
#include <vector>
#include "trampoline.h"
using namespace std;

struct func_obj {
    int operator()(int c1, int c2, int c3, int c4, int c5) {
        cout << "in func_obj()\n";
        return 2345619;
    }
    ~func_obj() {
        cout << "func_obj deleted\n";
    }
};


void test_1() {
    int b = 123;
    func_obj fo;

    trampoline<int (int, int, int, int, int)> tr(fo);
    auto p = tr.get();
    p(1, 2, 3, 4, 5);
    b = 124;

    int new_i = 999;
    int res = p(2, 3, 4, 5, 6);
    cout << res << "\n";
    cout << b << " " << new_i << "\n";
}

void test_2() {
    int b = 123;
    trampoline<long long (int, int, int, int, int, int, int, int, int)>
            tr([&](int c1, int c2, int c3, int c4, int c5, int c6, int c7, int c8, int c9) {
                cout << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " ";
                cout << c7 << " " << c8 << " " << c9 << "|\n";
                return 0;
            }
        );
    auto p = tr.get();
    {
        int res = p(100, 200, 300, 400, 500, 600, 700, 800, 900);
        cout << "result : " << res << "\n";
    }
    //cout << "after\n";

    b = 124;

    int new_i = 999;

    //cout << "???_2\n";
    p(9, 8, 7, 6, 5, 4, 3, 2, 1);
    //cout << "after_2\n";
    //cout << b << " " << new_i << "\n";
}

void test_3() {
    trampoline<long long (int, int, int, int, int, int, int, int, int)>
            tr([&](int c1, int c2, int c3, int c4, int c5, int c6, int c7, int c8, int c9) {
                cout << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " ";
                cout << c7 << " " << c8 << " " << c9 << "|\n";
                return 0;
            }
        );
    trampoline<long long (int, int, int, int, int, int, int, int, int)>
            tr1([&](int c1, int c2, int c3, int c4, int c5, int c6, int c7, int c8, int c9) {
                cout << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " ";
                cout << c7 << " " << c8 << " " << c9 << "|\n";
                return 0;
            }
        );
    trampoline<long long (int, int, int, int, int, int, int, int, int)>
            tr2([&](int c1, int c2, int c3, int c4, int c5, int c6, int c7, int c8, int c9) {
                cout << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " ";
                cout << c7 << " " << c8 << " " << c9 << "|\n";
                return 0;
            }
        );
    trampoline<long long (int, int, int, int, int, int, int, int, int)>
            tr3([&](int c1, int c2, int c3, int c4, int c5, int c6, int c7, int c8, int c9) {
                cout << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " ";
                cout << c7 << " " << c8 << " " << c9 << "|\n";
                return 0;
            }
        );
    cout << "---------\n";
}

struct big_obj {
    int q, w, e, r, t;
};

/*
void test_4() {
    trampoline<long long (int, int, int, int, int, int, int, int, int, big_obj)>
            tr3([&](int c1, int c2, int c3, int c4, int c5, int c6, int c7, int c8, int c9, big_obj c10) {
                cout << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " ";
                cout << c7 << " " << c8 << " " << c9 << "|\n";
                return 0;
            }
        );
}*/

int main() {
    test_1();
    cout << "\n\n";
    test_2();
    cout << "\n\n";

    test_3();
    test_3();

    //test_4();
}

// g++ -o main.exe main.cpp trampoline.h trampoline.cpp -std=c++11
