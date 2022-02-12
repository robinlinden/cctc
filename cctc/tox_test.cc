#include "cctc/tox.h"

#include <iostream>

int main() {
    std::cout << cctc::toxcore_version() << '\n';
    cctc::Tox tox;
}
