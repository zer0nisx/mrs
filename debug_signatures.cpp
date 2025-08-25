#include <iostream>
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

void enc(char* szBuffer, int nSize) {
    dec(szBuffer, nSize);
}

int main() {
    std::cout << "=== MRS Signature Debug ===" << std::endl;
    std::cout << "MRS_ZIP_CODE: 0x" << std::hex << MRS_ZIP_CODE << std::endl;
    std::cout << "MRS2_ZIP_CODE: 0x" << std::hex << MRS2_ZIP_CODE << std::endl;
    std::cout << "Expected signature (decimal): " << std::dec << MRS2_ZIP_CODE << std::endl;

    // Test encryption/decryption cycle
    MZIPDIRHEADER header = {};
    header.sig = MRS2_ZIP_CODE;

    std::cout << "\nBefore encryption:" << std::endl;
    std::cout << "header.sig: 0x" << std::hex << header.sig << " (" << std::dec << header.sig << ")" << std::endl;
    std::cout << "Size of MZIPDIRHEADER: " << sizeof(MZIPDIRHEADER) << " bytes" << std::endl;

    // Encrypt
    enc((char*)&header, sizeof(header));
    std::cout << "\nAfter encryption:" << std::endl;
    std::cout << "header.sig: 0x" << std::hex << header.sig << " (" << std::dec << header.sig << ")" << std::endl;

    // Decrypt
    dec((char*)&header, sizeof(header));
    std::cout << "\nAfter decryption:" << std::endl;
    std::cout << "header.sig: 0x" << std::hex << header.sig << " (" << std::dec << header.sig << ")" << std::endl;

    // Test just the signature field
    dword sig_test = MRS2_ZIP_CODE;
    std::cout << "\n=== Testing just signature field ===" << std::endl;
    std::cout << "Original: 0x" << std::hex << sig_test << " (" << std::dec << sig_test << ")" << std::endl;

    enc((char*)&sig_test, sizeof(sig_test));
    std::cout << "Encrypted: 0x" << std::hex << sig_test << " (" << std::dec << sig_test << ")" << std::endl;

    dec((char*)&sig_test, sizeof(sig_test));
    std::cout << "Decrypted: 0x" << std::hex << sig_test << " (" << std::dec << sig_test << ")" << std::endl;

    return 0;
}
