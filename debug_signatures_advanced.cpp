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

void printBytes(const char* label, const void* data, size_t size) {
    std::cout << label << ": ";
    const unsigned char* bytes = (const unsigned char*)data;
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
        if ((i + 1) % 8 == 0) std::cout << " ";
    }
    std::cout << std::endl;
}

void analyzeSignature(dword sig, const char* label) {
    std::cout << "\n=== Analyzing " << label << " ===" << std::endl;
    std::cout << "Value: 0x" << std::hex << sig << " (" << std::dec << sig << ")" << std::endl;

    // Mostrar en little-endian bytes
    printBytes("Bytes", &sig, sizeof(sig));

    // Intentar desencriptar esta signature directamente
    dword testSig = sig;
    dec((char*)&testSig, sizeof(testSig));
    std::cout << "After decryption: 0x" << std::hex << testSig << " (" << std::dec << testSig << ")" << std::endl;

    // Verificar si coincide con alguna signature conocida
    if (testSig == MRS_ZIP_CODE) {
        std::cout << "✅ Matches MRS_ZIP_CODE after decryption!" << std::endl;
    } else if (testSig == MRS2_ZIP_CODE) {
        std::cout << "✅ Matches MRS2_ZIP_CODE after decryption!" << std::endl;
    }

    // Intentar interpretar como si fuera una signature ya desencriptada
    if (sig == MRS_ZIP_CODE) {
        std::cout << "✅ Already matches MRS_ZIP_CODE (no decryption needed)" << std::endl;
    } else if (sig == MRS2_ZIP_CODE) {
        std::cout << "✅ Already matches MRS2_ZIP_CODE (no decryption needed)" << std::endl;
    }

    // Intentar encriptar las signatures conocidas para ver si coinciden
    dword encMRS = MRS_ZIP_CODE;
    dword encMRS2 = MRS2_ZIP_CODE;
    enc((char*)&encMRS, sizeof(encMRS));
    enc((char*)&encMRS2, sizeof(encMRS2));

    if (sig == encMRS) {
        std::cout << "✅ Matches encrypted MRS_ZIP_CODE!" << std::endl;
    } else if (sig == encMRS2) {
        std::cout << "✅ Matches encrypted MRS2_ZIP_CODE!" << std::endl;
    }
}

int main() {
    std::cout << "=== Advanced MRS Signature Debug ===" << std::endl;
    std::cout << "MRS_ZIP_CODE: 0x" << std::hex << MRS_ZIP_CODE << std::endl;
    std::cout << "MRS2_ZIP_CODE: 0x" << std::hex << MRS2_ZIP_CODE << std::endl;
    std::cout << "Expected signature (decimal): " << std::dec << MRS2_ZIP_CODE << std::endl;

    // Test normal encryption/decryption cycle
    MZIPDIRHEADER header = {};
    header.sig = MRS2_ZIP_CODE;

    std::cout << "\n=== Normal Encryption/Decryption Test ===" << std::endl;
    std::cout << "Original: 0x" << std::hex << header.sig << " (" << std::dec << header.sig << ")" << std::endl;

    enc((char*)&header, sizeof(header));
    std::cout << "Encrypted: 0x" << std::hex << header.sig << " (" << std::dec << header.sig << ")" << std::endl;

    dec((char*)&header, sizeof(header));
    std::cout << "Decrypted: 0x" << std::hex << header.sig << " (" << std::dec << header.sig << ")" << std::endl;

    // Analyze the problematic signature from the log
    dword problematicSig = 0x140c0820; // 336332832
    analyzeSignature(problematicSig, "Problematic Signature");

    // Test various other scenarios
    std::cout << "\n=== Testing Byte Order Issues ===" << std::endl;

    // Test if it might be a byte order issue
    dword swapped = ((problematicSig & 0xFF) << 24) |
                   (((problematicSig >> 8) & 0xFF) << 16) |
                   (((problematicSig >> 16) & 0xFF) << 8) |
                   ((problematicSig >> 24) & 0xFF);
    analyzeSignature(swapped, "Byte-swapped Problematic");

    // Test individual signature field encryption vs full header encryption
    std::cout << "\n=== Signature Field Only vs Full Header ===" << std::endl;

    dword sigOnly = MRS2_ZIP_CODE;
    enc((char*)&sigOnly, sizeof(sigOnly));
    std::cout << "Signature field only encrypted: 0x" << std::hex << sigOnly << " (" << std::dec << sigOnly << ")" << std::endl;

    MZIPDIRHEADER fullHeader = {};
    fullHeader.sig = MRS2_ZIP_CODE;
    enc((char*)&fullHeader, sizeof(fullHeader));
    std::cout << "Signature in full header encrypted: 0x" << std::hex << fullHeader.sig << " (" << std::dec << fullHeader.sig << ")" << std::endl;

    // Test if offset in the encryption makes a difference
    std::cout << "\n=== Testing Offset Effects ===" << std::endl;
    char buffer[32] = {0};

    // Put signature at different offsets and encrypt
    for (int offset = 0; offset < 8; offset++) {
        memset(buffer, 0, sizeof(buffer));
        *((dword*)(buffer + offset)) = MRS2_ZIP_CODE;
        enc(buffer, sizeof(buffer));
        dword resultSig = *((dword*)(buffer + offset));
        std::cout << "Offset " << offset << ": 0x" << std::hex << resultSig << " (" << std::dec << resultSig << ")" << std::endl;

        if (resultSig == problematicSig) {
            std::cout << "  ✅ MATCH! Problem might be offset-related!" << std::endl;
        }
    }

    return 0;
}
