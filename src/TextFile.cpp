#include "TextFile.h"

TextFile::TextFile(const char* filename) {
    std::ifstream file(filename);

    file.seekg(0, std::ios_base::end);
    const uint32_t fileLength = file.tellg();
    file.seekg(0);

    data = new char[fileLength + 1];
    file.read(&data[0], fileLength);
    data[fileLength] = '\0';
}

TextFile::~TextFile() {
    delete[] data;
    data = nullptr;
}

const char* TextFile::cStr() const {
    return data;
}

TextFile::operator const char* () {
    return data;
}