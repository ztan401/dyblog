#include <iostream>
#include "nanotimestamp.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::cout << "now in nano: "<< kungfu::yijinjing::time::now_in_nano()<< std::endl;
    std::cout << "now: "<< kungfu::yijinjing::time::strftime(kungfu::yijinjing::time::now_in_nano())<< std::endl;
    return 0;
}
