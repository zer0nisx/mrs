#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#ifdef _WIN32
#include <Windows.h>
typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned char BYTE;
#else
typedef uint32_t dword;
typedef uint16_t word;
typedef uint8_t BYTE;
#include <cstdint>
#endif

#define MRS_ZIP_CODE    0x05030207
#define MRS2_ZIP_CODE   0x05030208

#pragma pack(2)
struct MZIPDIRHEADER {
    dword   sig;
    word    nDisk;
    word    nStartDisk;
    word    nDirEntries;
    word    totalDirEntries;
    dword   dirSize;
    dword   dirOffset;
    word    cmntLen;
};
#pragma pack()

void dec(char* szBuffer, int nSize) {
    if (!szBuffer) return;

    BYTE b;
    char key[18] = { 15, -81, 42, 3, -123, 66, -109, 103, -46, -36, -94, 64, -115, 113, -103, -9, -65, -103 };
    for (int i = 0; i < nSize; ++i) {
        char xor_key = key[i % 18];
        b = *szBuffer;
        b ^= xor_key;
        *szBuffer = b;
        szBuffer++;
    }
}

void printHex(const char* label, const void* data, size_t size) {
    std::cout << label << ": ";
    const unsigned char* bytes = (const unsigned char*)data;
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "=== MRS File Reading Debug ===" << std::endl;

    std::string filename;
    std::cout << "Enter MRS filename (without .mrs extension): ";
    std::getline(std::cin, filename);
    filename += ".mrs";

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cout << "Cannot open file: " << filename << std::endl;
        return 1;
    }

    // Obtener tamaño del archivo
    size_t fileSize = file.tellg();
    std::cout << "File size: " << fileSize << " bytes" << std::endl;

    // Ir al final - tamaño del header
    size_t headerSize = sizeof(MZIPDIRHEADER);
    std::cout << "MZIPDIRHEADER size: " << headerSize << " bytes" << std::endl;

    if (fileSize < headerSize) {
        std::cout << "File too small to contain header!" << std::endl;
        return 1;
    }

    // Leer desde diferentes posiciones para debug
    std::cout << "\n=== Reading from different positions ===" << std::endl;

    // Leer los últimos 32 bytes del archivo para análisis
    size_t readSize = std::min((size_t)32, fileSize);
    file.seekg(-readSize, std::ios::end);

    char lastBytes[32];
    file.read(lastBytes, readSize);

    std::cout << "Last " << readSize << " bytes of file:" << std::endl;
    printHex("Raw", lastBytes, readSize);

    // Intentar leer el header desde la posición esperada
    file.seekg(-headerSize, std::ios::end);
    size_t headerOffset = file.tellg();
    std::cout << "\nReading header from offset: " << headerOffset << std::endl;

    MZIPDIRHEADER header;
    file.read((char*)&header, sizeof(header));

    std::cout << "\n=== Header before decryption ===" << std::endl;
    printHex("Raw header", &header, sizeof(header));
    std::cout << "Signature (raw): 0x" << std::hex << header.sig << " (" << std::dec << header.sig << ")" << std::endl;

    // Hacer una copia para desencriptar
    MZIPDIRHEADER decryptedHeader = header;
    dec((char*)&decryptedHeader, sizeof(decryptedHeader));

    std::cout << "\n=== Header after decryption ===" << std::endl;
    printHex("Decrypted header", &decryptedHeader, sizeof(decryptedHeader));
    std::cout << "Signature (decrypted): 0x" << std::hex << decryptedHeader.sig << " (" << std::dec << decryptedHeader.sig << ")" << std::endl;

    // Verificar si coincide con las signatures esperadas
    std::cout << "\n=== Signature verification ===" << std::endl;
    std::cout << "Expected MRS_ZIP_CODE: 0x" << std::hex << MRS_ZIP_CODE << " (" << std::dec << MRS_ZIP_CODE << ")" << std::endl;
    std::cout << "Expected MRS2_ZIP_CODE: 0x" << std::hex << MRS2_ZIP_CODE << " (" << std::dec << MRS2_ZIP_CODE << ")" << std::endl;

    if (decryptedHeader.sig == MRS2_ZIP_CODE) {
        std::cout << "✅ Valid MRS2 signature!" << std::endl;
    } else if (decryptedHeader.sig == MRS_ZIP_CODE) {
        std::cout << "✅ Valid MRS signature!" << std::endl;
    } else {
        std::cout << "❌ Invalid signature!" << std::endl;
        std::cout << "Got: 0x" << std::hex << decryptedHeader.sig << " (" << std::dec << decryptedHeader.sig << ")" << std::endl;
    }

    // Intentar también desde -24 bytes por si el padding es diferente
    std::cout << "\n=== Trying with 24-byte header size ===" << std::endl;
    file.seekg(-24, std::ios::end);

    MZIPDIRHEADER header24;
    file.read((char*)&header24, sizeof(header24));

    std::cout << "Header from -24 bytes:" << std::endl;
    printHex("Raw", &header24, sizeof(header24));

    MZIPDIRHEADER decrypted24 = header24;
    dec((char*)&decrypted24, sizeof(decrypted24));
    std::cout << "Signature (24-byte): 0x" << std::hex << decrypted24.sig << " (" << std::dec << decrypted24.sig << ")" << std::endl;

    file.close();
    return 0;
}
