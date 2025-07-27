#include <vector>
#include <iostream>
#include "Signal.h"

int main() {
    std::vector<signal::Signal>  foo(500000000);
    char wait{};   // use to look at storage
    std::cout << "Should see storage used; press key to continue\n";
    std::cin >> wait;
    std::cout << "Now we swap out the vector; should see storage reduced\n";
    std::vector<signal::Signal>().swap(foo);
    std::cout << "Look at memory now; press any key to finish\n";
    return 0;
}