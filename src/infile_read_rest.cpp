#include <iostream>
#include <fstream>
#include "simulation_domain.h"
#include "concentration_domain.h"


void read_infile(std::string filename) {
    std::string line;
    std::ifstream infile(filename);

    // Support for up to 10 different fields in infiles
    std::regex square_regex("square\\((\\d)\\) (\\d+\\.\\d+), (\\d+), (\\d+), (\\d+), (\\d+)");
    std::smatch regex_match;

    if (infile.is_open()) {
        while (std::getline(infile, line)) {
            std::cout << line << std::endl;
            if (line[0] == (char)"#") {
                continue;
            }
            if (NX == -1) {
                if (std::regex_match(line, regex_match, std::regex("NX = (\\d+)"))) {
                    NX = (long)std::stoi(regex_match[0], nullptr);
                }
            }
            if (std::regex_match(line, regex_match, square_regex)) {
                std::cout << "Field ID: " << regex_match[0] << std::endl;
                std::cout << "Value: " << regex_match[1] << std::endl;
                std::cout << "XMin: " << regex_match[2] << std::endl;
                std::cout << "XMax: " << regex_match[4] << std::endl;
                std::cout << "YMin: " << regex_match[3] << std::endl;
                std::cout << "YMax: " << regex_match[5] << std::endl;
            }
        }
        infile.close();
    }
    else {
        std::cout << "Error, Unable to open File" << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "You need to pass an inputfile to start a simulation" << std::endl;
        return;
    }
    else {
        if (read_infile())
    }


}