#include <iostream>
#include "../include/storage/page.h"

int main() {
    cmse::Page page;
    std::cout << "Page size = " << sizeof(page) << "\n";
    return 0;
}

