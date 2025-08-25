#include "stdafx.h"
#include "FileSystem.h"
#include "zconf.h"
#include "zlib.h"
#include <filesystem>

using namespace std;
using namespace std::filesystem;

// Agregar soporte para la signature problemática detectada
#define MRS3_ZIP_CODE   0x140c0820  // Nueva signature detectada en el debug

void dec(char* szBuffer, int nSize) {
	if (!szBuffer) return;

	BYTE b;

	char key[18] = { 15, -81, 42, 3, -123, 66, -109, 103, -46, -36, -94, 64, -115, 113, -103, -9, -65, -103 };
	for (int i = 0; i < nSize; ++i)
	{
		char xor = key[i % 18];
		b = *szBuffer;
		b ^= xor;
		*szBuffer = b;
		szBuffer++;
	}
}

void enc(char* szBuffer, int nSize) {
	dec(szBuffer, nSize);
}

void printHex(const char* label, const void* data, size_t size) {
	printf("%s: ", label);
	const unsigned char* bytes = (const unsigned char*)data;
	for (size_t i = 0; i < size; i++) {
		printf("%02X ", bytes[i]);
	}
	printf("\n");
}

bool isValidSignature(dword sig) {
	return (sig == MRS2_ZIP_CODE || sig == MRS_ZIP_CODE || sig == MRS3_ZIP_CODE);
}

const char* getSignatureName(dword sig) {
	if (sig == MRS2_ZIP_CODE) return "MRS2_ZIP_CODE";
	if (sig == MRS_ZIP_CODE) return "MRS_ZIP_CODE";
	if (sig == MRS3_ZIP_CODE) return "MRS3_ZIP_CODE";
	return "UNKNOWN";
}

// Función mejorada para leer header con mejor debug
bool readMRSHeader(File* pFile, MZIPDIRHEADER& header, size_t& headerPosition) {
	size_t fileSize = pFile->getSize();

	printf("=== MRS Header Debug ===\n");
	printf("File size: %zu bytes\n", fileSize);
	printf("MZIPDIRHEADER size: %zu bytes\n", sizeof(MZIPDIRHEADER));

	// Intentar leer desde diferentes posiciones
	vector<int> tryOffsets = {
		-(int)sizeof(MZIPDIRHEADER),  // -22 bytes (pack 2)
		-24,                          // -24 bytes (normal alignment)
		-20,                          // -20 bytes (pack 1)
		-26,                          // Por si hay padding extra
		-28
	};

	for (int offset : tryOffsets) {
		if (fileSize < abs(offset)) continue;

		printf("\nTrying offset %d from end...\n", offset);

		pFile->setOffset(offset, SEEK_END);
		headerPosition = pFile->getOffset();
		printf("Reading from position: %zu\n", headerPosition);

		// Leer header
		MZIPDIRHEADER tempHeader;
		pFile->read((char*)&tempHeader, sizeof(MZIPDIRHEADER));

		printf("Raw header data:\n");
		printHex("  Before decryption", &tempHeader, sizeof(tempHeader));
		printf("  Signature (raw): 0x%08X (%u)\n", tempHeader.sig, tempHeader.sig);

		// Desencriptar
		MZIPDIRHEADER decryptedHeader = tempHeader;
		dec((char*)&decryptedHeader, sizeof(decryptedHeader));

		printf("  After decryption:\n");
		printHex("  Decrypted header", &decryptedHeader, sizeof(decryptedHeader));
		printf("  Signature (decrypted): 0x%08X (%u) - %s\n",
			   decryptedHeader.sig, decryptedHeader.sig, getSignatureName(decryptedHeader.sig));

		// Verificar si es válida
		if (isValidSignature(decryptedHeader.sig)) {
			printf("  ✅ Valid signature found!\n");
			header = decryptedHeader;
			return true;
		} else {
			printf("  ❌ Invalid signature\n");
		}
	}

	printf("\n❌ No valid signature found in any position!\n");
	return false;
}

void createDir(const char * szPath)
{
	char *work = _strdup(szPath), *ptr;

	for (ptr = work; *ptr; ++ptr)
	{
		if (*ptr == '\\')
		{
			*ptr = 0;
			CreateDirectoryA(work, NULL);
			*ptr = '\\';
		}
	}
	free(work);
}

int main() {

	printf("MasangSoft MRS Compressor/Decompressor (Improved Debug Version)\n");

mainMenu:
	cout << "\nSelect operation:\n";
	cout << "1. Extract MRS file (with enhanced debug)\n";
	cout << "2. Create MRS file from folder\n";
	cout << "3. Exit\n";
	cout << "Choice: ";

	int choice;
	cin >> choice;
	cin.ignore();

	switch (choice) {
		case 1: {
		extractBegin:
			cout << "\nPlease enter the file name: ";
			string strFile;
			getline(cin, strFile);

			File* pFile = new File();
			if(!pFile->open(strFile + ".mrs")) {
				cout << "File not found\n";
				goto extractBegin;
			}

			// Usar la función mejorada de lectura de header
			MZIPDIRHEADER dh;
			size_t headerPosition;

			if (!readMRSHeader(pFile, dh, headerPosition)) {
				cout << "Failed to read valid MRS header. File might be corrupted or use unsupported format.\n";
				pFile->close();
				delete pFile;
				goto extractBegin;
			}

			printf("\n=== Successfully read header ===\n");
			printf("Signature: 0x%08X (%s)\n", dh.sig, getSignatureName(dh.sig));
			printf("Directory entries: %u\n", dh.nDirEntries);
			printf("Directory size: %u bytes\n", dh.dirSize);
			printf("Directory offset: %u\n", dh.dirOffset);

			// Continuar con el proceso normal de extracción...
			pFile->setOffset(headerPosition - dh.dirSize, SEEK_SET);

			char* szBuffer = new char[dh.dirSize + dh.nDirEntries * sizeof(MZIPDIRFILEHEADER)];
			pFile->read(szBuffer, dh.dirSize);
			pFile->close();
			delete pFile;

			dec(szBuffer, dh.dirSize);

			char* ptr = szBuffer;
			for(word i = 0; i < dh.nDirEntries; ++i) {
				MZIPDIRFILEHEADER* pfh = (MZIPDIRFILEHEADER*)ptr;
				ptr += sizeof(MZIPDIRFILEHEADER);

				char* szFileName = new char[pfh->fileNameLen + 1];
				memcpy(szFileName, ptr, pfh->fileNameLen);
				szFileName[pfh->fileNameLen] = 0;
				ptr += pfh->fileNameLen;

				printf("Extracting: %s\n", szFileName);

				createDir(szFileName);

				File* pExtractFile = new File();
				if(pExtractFile->create(szFileName)) {
					File* pDataFile = new File();
					if(pDataFile->open(strFile + ".mrs")) {
						pDataFile->setOffset(pfh->offset, SEEK_SET);

						char* szCompressedData = new char[pfh->compressedSize];
						pDataFile->read(szCompressedData, pfh->compressedSize);

						dec(szCompressedData, pfh->compressedSize);

						if(pfh->method == 8) {
							char* szUncompressedData = new char[pfh->uncompressedSize];
							uLongf uncompressedSize = pfh->uncompressedSize;

							if(uncompress((Bytef*)szUncompressedData, &uncompressedSize,
										(const Bytef*)szCompressedData, pfh->compressedSize) == Z_OK) {
								pExtractFile->write(uncompressedSize, szUncompressedData);
							} else {
								printf("Decompression failed for: %s\n", szFileName);
							}
							delete[] szUncompressedData;
						} else {
							pExtractFile->write(pfh->compressedSize, szCompressedData);
						}

						delete[] szCompressedData;
						pDataFile->close();
						delete pDataFile;
					}
					pExtractFile->close();
					delete pExtractFile;
				}

				delete[] szFileName;
			}

			delete[] szBuffer;
			printf("Extraction completed!\n");
			break;
		}

		case 2: {
			// Código de compresión existente...
			cout << "Compression feature - code would go here...\n";
			break;
		}

		case 3:
			return 0;

		default:
			cout << "Invalid choice\n";
			break;
	}

	goto mainMenu;
	return 0;
}
