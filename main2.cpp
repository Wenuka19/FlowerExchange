#include <iostream>

int main() {
    int x = 42;
    int& refToX = x;

    std::cout << "Original value of x: " << x << std::endl;
    std::cout << "Value through reference: " << refToX << std::endl;

    refToX = 99;  // This modifies the original variable x

    std::cout << "Modified value of x: " << x <<"\n"<< std::endl;


    x = 43;
    int* ptrToX = &x;

    std::cout << "Original value of x: " << x << std::endl;
    std::cout << "Value through pointer: " << *ptrToX << std::endl;

    *ptrToX = 99;  // This modifies the original variable x

    std::cout << "Modified value of x: " << x << std::endl;

    return 0;
}