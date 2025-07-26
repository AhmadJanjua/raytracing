#pragma once

#include <iostream>
#include <fstream>
#include <cstdint>

class TextFile {
private:
    char* data = nullptr;
public:
    TextFile(const char* filename);

    ~TextFile();

    const char* cStr() const;

    operator const char*();
};