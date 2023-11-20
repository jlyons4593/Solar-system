// Joseph Lyons 12/11/2023
#include <iostream>
#include <fstream>
#include <string>
namespace fs = std::filesystem;

namespace FileReader{
// function for reading in text files and outputting them to the console
void readAndOutputFile(const std::string& filename) {
    // Check if the file exists
    if (!fs::exists(filename)) {
        std::cerr << "Error: File " << filename << " not found." << std::endl;
        return;
    }

    // Open the file
    std::ifstream file(filename);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open the file " << filename << std::endl;
        return;
    }

    // Read and output the file content line by line
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    // Check if any error occurred while reading
    if (file.bad()) {
        std::cerr << "Error reading from file " << filename << std::endl;
    }

    // Close the file
    file.close();
}

}

