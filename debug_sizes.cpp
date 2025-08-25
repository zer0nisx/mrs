#include <iostream>
#include <cstring>

typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned char BYTE;

#define MRS2_ZIP_CODE   0x05030208

// Test without pragma pack
struct MZIPDIRHEADER_NORMAL {
    dword   sig;
    word    nDisk;
    word    nStartDisk;
    word    nDirEntries;
    word    totalDirEntries;
    dword   dirSize;
    dword   dirOffset;
    word    cmntLen;
};

// Test with pragma pack(2)
#pragma pack(2)
struct MZIPDIRHEADER_PACKED {
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

// Test with pragma pack(1)
#pragma pack(1)
struct MZIPDIRHEADER_PACKED1 {
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

int main() {
    std::cout << "=== Data Type Sizes ===" << std::endl;
    std::cout << "sizeof(dword): " << sizeof(dword) << " bytes" << std::endl;
    std::cout << "sizeof(word): " << sizeof(word) << " bytes" << std::endl;
    std::cout << "sizeof(BYTE): " << sizeof(BYTE) << " bytes" << std::endl;
    std::cout << "sizeof(unsigned long): " << sizeof(unsigned long) << " bytes" << std::endl;
    std::cout << "sizeof(unsigned short): " << sizeof(unsigned short) << " bytes" << std::endl;

    std::cout << "\n=== Structure Sizes ===" << std::endl;
    std::cout << "MZIPDIRHEADER_NORMAL: " << sizeof(MZIPDIRHEADER_NORMAL) << " bytes" << std::endl;
    std::cout << "MZIPDIRHEADER_PACKED(2): " << sizeof(MZIPDIRHEADER_PACKED) << " bytes" << std::endl;
    std::cout << "MZIPDIRHEADER_PACKED(1): " << sizeof(MZIPDIRHEADER_PACKED1) << " bytes" << std::endl;

    std::cout << "\n=== Field Offsets (PACKED2) ===" << std::endl;
    MZIPDIRHEADER_PACKED header = {};
    char* base = (char*)&header;
    std::cout << "sig offset: " << ((char*)&header.sig - base) << std::endl;
    std::cout << "nDisk offset: " << ((char*)&header.nDisk - base) << std::endl;
    std::cout << "nStartDisk offset: " << ((char*)&header.nStartDisk - base) << std::endl;
    std::cout << "dirSize offset: " << ((char*)&header.dirSize - base) << std::endl;
    std::cout << "dirOffset offset: " << ((char*)&header.dirOffset - base) << std::endl;

    std::cout << "\n=== Expected vs Actual Values ===" << std::endl;
    std::cout << "MRS2_ZIP_CODE: 0x" << std::hex << MRS2_ZIP_CODE << " (" << std::dec << MRS2_ZIP_CODE << ")" << std::endl;
    std::cout << "Problem signature: 336332832 (0x" << std::hex << 336332832 << ")" << std::endl;

    return 0;
}
