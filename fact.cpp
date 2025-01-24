#include <iostream>
#include <cstdlib> // For std::atoi

// Function to calculate factorial
unsigned long long factorial(int n) {
    if (n == 0 || n == 1)
        return 1;
    return n * factorial(n - 1);
}

int main(int argc, char* argv[]) {
    // Check if the input is provided
    if (argc != 2) {
        std::cerr << "Usage: ./factorial <number>" << std::endl;
        return 1;
    }

    // Convert the input to an integer
    int num = std::atoi(argv[1]);

    // Check if the number is non-negative
    if (num < 0) {
        std::cerr << "Error: Factorial is not defined for negative numbers." << std::endl;
        return 1;
    }

    // Calculate and display the factorial
    std::cout << "The factorial of " << num << " is " << factorial(num) << std::endl;
    return 0;
}
