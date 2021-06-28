#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void PrintUsage(std::string const & filename) {
    std::cout << "Usage: " << filename << " [inputfile]" << std::endl;
}

int main(int argc, char *argv[]) {
    std::string bfFilename;
    
    if (argc == 1) {
        std::cout << "Not enough arguments." << std::endl;
        PrintUsage(argv[0]);
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        bfFilename = argv[1];
    } else {
        std::cout << "Too much arguments." << std::endl;
        PrintUsage(argv[0]);
        return EXIT_FAILURE;
    }

    std::string code = 
        "#include <stdio.h>" "\n" 
        "#include <stdlib.h>" "\n"
        "\n"
        "int main() {" "\n"
        "\t" "char* p = calloc(30000, sizeof(char));" "\n";
    
    std::ifstream bfFile { bfFilename, std::ios::binary|std::ios::ate };
    
    if (!bfFile.is_open()) {
        std::cout << "Failed to open '" << bfFilename << "'." << std::endl;
        return EXIT_FAILURE;
    }

    auto end = bfFile.tellg();
    bfFile.seekg(0, std::ios::beg);
    auto size = std::size_t(end - bfFile.tellg());

    if (size == 0) {
        std::cout << "The file '" << bfFilename << "' was empty." << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<char> bfContent(size);
    if (!bfFile.read(bfContent.data(), bfContent.size())) {
        throw std::runtime_error(bfFilename + ": " + std::strerror(errno));
    }
    bfFile.close();

    std::size_t tabs = 1;
    for (const char c : bfContent) {
        const std::size_t indent = (c != ']') ? tabs : tabs - 1;
        code += std::string(indent, '\t');
        switch (c) {
            case '>': 
                code += "++p;" "\n";
                break;
            case '<':
                code += "--p;" "\n";
                break;
            case '+':
                code += "++*p;" "\n";
                break;
            case '-':
                code += "--*p;" "\n";
                break;
            case '.':
                code += "putchar(*p);" "\n";
                break;
            case ',':
                code += "*p = getchar();" "\n";
                break;
            case '[':
                code += "while (*p) {" "\n";
                ++tabs;
                break;
            case ']':
                code += "}" "\n";
                --tabs;
                break;
            default: break;
        }
    }

    code += "\t" "free(p);" "\n" "}";

    std::ofstream cFile { "output.c" };
    cFile << code << std::endl;

    return EXIT_SUCCESS;
}
