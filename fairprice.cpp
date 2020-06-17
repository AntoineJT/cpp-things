#include <cstdlib>
#include <iostream>
#include <random>

constexpr int MIN = 0;
constexpr int MAX = 1000;

int random_int() {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(MIN, MAX);
    return dist(rd);
}

int main() {
    std::cout 
        << "-----------------" << std::endl
        << " Fair price game" << std::endl
        << "-----------------" << std::endl
        << std::endl
        << "The number to guess is a random one under " << MIN << " and " << MAX << std::endl
        << std::endl;

    int number = random_int();
    int round = 1;
    int guess = 0;

    while (true) {
        std::cout << "Your guess: ";
        std::cin >> guess;

        if (std::cin.eof()) {
            throw std::runtime_error("Input stream has been closed!");
        } else
        if (std::cin.fail()) {
            std::cout << "Invalid input. Try again!" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            if (guess < MIN || guess > MAX) {
                std::cout << "Number out of range (range: " << MIN << "-" << MAX << "). Try again." << std::endl;
            } else {
                if (guess < number) {
                    std::cout << "More." << std::endl;
                } else
                if (guess > number) {
                    std::cout << "Less." << std::endl;
                } else {
                    std::cout << "That's it! The random number was " << number << "." << std::endl
                        << "You win in " << round << " round" << ((round > 1) ? "s." : ".") << std::endl;
                    break;
                }
                ++round;
            }
        }
    }
    return EXIT_SUCCESS;
}
