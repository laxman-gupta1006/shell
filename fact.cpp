#include <iostream>
#include <cstdlib> // For std::atoi
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>

// Simulate computation time
void simulate_computation() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

// Function to calculate factorial for large numbers
std::string large_factorial(int n) {
    simulate_computation(); // Simulate computation time
    std::vector<int> result;
    result.push_back(1);

    for (int i = 2; i <= n; ++i) {
        int carry = 0;
        for (size_t j = 0; j < result.size(); ++j) {
            int product = result[j] * i + carry;
            result[j] = product % 10;
            carry = product / 10;
        }

        while (carry) {
            result.push_back(carry % 10);
            carry /= 10;
        }
    }

    std::string factorial;
    for (auto it = result.rbegin(); it != result.rend(); ++it) {
        factorial += std::to_string(*it);
    }

    return factorial;
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
    std::cout << "The factorial of " << num << " is " << large_factorial(num) << std::endl;
    return 0;
}
