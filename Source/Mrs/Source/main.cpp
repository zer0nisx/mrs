#include "stdafx.h"
#include "FileSystem.h"
#include "zconf.h"
#include "zlib.h"
#include <filesystem>

using namespace std;
using namespace std::filesystem;

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

// Función de encriptación (inversa de dec - es la misma función ya que XOR es reversible)
void enc(char* szBuffer, int nSize) {
	dec(szBuffer, nSize); // XOR es reversible, usar la misma función
}

// Función para calcular CRC32
dword calculateCRC32(const char* data, size_t size) {
	dword crc = crc32(0L, Z_NULL, 0);
	return crc32(crc, (const Bytef*)data, size);
}

// Estructura para almacenar información de archivos a comprimir
struct FileEntry {
	string relativePath;
	string fullPath;
	size_t fileSize;
	dword crc32;
	vector<char> compressedData;
	size_t compressedSize;
};

// Función para recorrer recursivamente un directorio
vector<FileEntry> collectFiles(const string& rootPath) {
	vector<FileEntry> files;

	try {
		for (const auto& entry : recursive_directory_iterator(rootPath)) {
			if (entry.is_regular_file()) {
				FileEntry fileEntry;
				fileEntry.fullPath = entry.path().string();

				// Obtener ruta relativa desde rootPath
				string relativePath = relative(entry.path(), rootPath).string();
				// Convertir backslashes a forward slashes para consistencia
				replace(relativePath.begin(), relativePath.end(), '\\', '/');
				fileEntry.relativePath = relativePath;

				fileEntry.fileSize = entry.file_size();
				files.push_back(fileEntry);
			}
		}
	}
	catch (const filesystem_error& ex) {
		cout << "Error accessing directory: " << ex.what() << endl;
	}

	return files;
}

// Función para comprimir un archivo usando zlib
bool compressFile(FileEntry& fileEntry) {
	File* pFile = new File();
	if (!pFile->open(fileEntry.fullPath)) {
		cout << "Failed to open file: " << fileEntry.fullPath << endl;
		delete pFile;
		return false;
	}

	// Leer contenido del archivo
	vector<char> fileData(fileEntry.fileSize);
	pFile->read(fileData.data(), fileEntry.fileSize);
	pFile->close();
	delete pFile;

	// Calcular CRC32
	fileEntry.crc32 = calculateCRC32(fileData.data(), fileEntry.fileSize);

	// Comprimir usando zlib
	uLongf compressedSize = compressBound(fileEntry.fileSize);
	fileEntry.compressedData.resize(compressedSize);

	int result = compress2((Bytef*)fileEntry.compressedData.data(), &compressedSize,
						  (const Bytef*)fileData.data(), fileEntry.fileSize, Z_BEST_COMPRESSION);

	if (result != Z_OK) {
		cout << "Failed to compress file: " << fileEntry.relativePath << endl;
		return false;
	}

	fileEntry.compressedSize = compressedSize;
	fileEntry.compressedData.resize(compressedSize);

	return true;
}

// Función principal para crear un archivo MRS
bool createMRS(const string& folderPath, const string& outputFile) {
	cout << "Creating MRS from folder: " << folderPath << endl;

	// Recopilar todos los archivos
	vector<FileEntry> files = collectFiles(folderPath);
	if (files.empty()) {
		cout << "No files found in directory: " << folderPath << endl;
		return false;
	}

	cout << "Found " << files.size() << " files to compress" << endl;

	// Comprimir cada archivo
	for (auto& file : files) {
		cout << "Compressing: " << file.relativePath << endl;
		if (!compressFile(file)) {
			return false;
		}
	}

	// Crear archivo MRS
	File* pMRSFile = new File();
	if (!pMRSFile->open(outputFile + ".mrs", FF_WRITE)) {
		cout << "Failed to create output file: " << outputFile << ".mrs" << endl;
		delete pMRSFile;
		return false;
	}

	vector<dword> fileOffsets;

	// Escribir datos locales de archivos
	for (const auto& file : files) {
		fileOffsets.push_back(pMRSFile->getOffset());

		// Crear header local
		MZIPLOCALHEADER localHeader = {};
		localHeader.sig = MZIPLOCALHEADER::SIGNATURE2;
		localHeader.version = 20;
		localHeader.flag = 0;
		localHeader.compression = MZIPLOCALHEADER::COMP_DEFLAT;
		localHeader.modTime = 0;
		localHeader.modDate = 0;
		localHeader.crc32 = file.crc32;
		localHeader.cSize = file.compressedSize;
		localHeader.ucSize = file.fileSize;
		localHeader.fnameLen = file.relativePath.length();
		localHeader.xtraLen = 0;

		// Encriptar header
		enc((char*)&localHeader, sizeof(localHeader));

		// Escribir header local
		pMRSFile->write(sizeof(localHeader), &localHeader);

		// Escribir nombre del archivo
		pMRSFile->write(file.relativePath.length(), (void*)file.relativePath.c_str());

		// Escribir datos comprimidos
		pMRSFile->write(file.compressedSize, (void*)file.compressedData.data());
	}

	// Guardar offset del directorio central
	dword centralDirOffset = pMRSFile->getOffset();

	// Escribir directorio central
	vector<char> centralDirData;
	for (size_t i = 0; i < files.size(); i++) {
		const auto& file = files[i];

		MZIPDIRFILEHEADER dirHeader = {};
		dirHeader.sig = MZIPDIRFILEHEADER::SIGNATURE2;
		dirHeader.verMade = 20;
		dirHeader.verNeeded = 20;
		dirHeader.flag = 0;
		dirHeader.compression = MZIPDIRFILEHEADER::COMP_DEFLAT;
		dirHeader.modTime = 0;
		dirHeader.modDate = 0;
		dirHeader.crc32 = file.crc32;
		dirHeader.cSize = file.compressedSize;
		dirHeader.ucSize = file.fileSize;
		dirHeader.fnameLen = file.relativePath.length();
		dirHeader.xtraLen = 0;
		dirHeader.cmntLen = 0;
		dirHeader.diskStart = 0;
		dirHeader.intAttr = 0;
		dirHeader.extAttr = 0;
		dirHeader.hdrOffset = fileOffsets[i];

		// Agregar header a datos del directorio central
		size_t currentPos = centralDirData.size();
		centralDirData.resize(currentPos + sizeof(dirHeader));
		memcpy(centralDirData.data() + currentPos, &dirHeader, sizeof(dirHeader));

		// Agregar nombre del archivo
		currentPos = centralDirData.size();
		centralDirData.resize(currentPos + file.relativePath.length());
		memcpy(centralDirData.data() + currentPos, file.relativePath.c_str(), file.relativePath.length());
	}

	// Encriptar datos del directorio central
	enc(centralDirData.data(), centralDirData.size());

	// Escribir directorio central
	pMRSFile->write(centralDirData.size(), centralDirData.data());

	// Crear y escribir header del directorio final
	MZIPDIRHEADER finalHeader = {};
	finalHeader.sig = MRS2_ZIP_CODE;
	finalHeader.nDisk = 0;
	finalHeader.nStartDisk = 0;
	finalHeader.nDirEntries = files.size();
	finalHeader.totalDirEntries = files.size();
	finalHeader.dirSize = centralDirData.size();
	finalHeader.dirOffset = centralDirOffset;
	finalHeader.cmntLen = 0;

	// Encriptar header final
	enc((char*)&finalHeader, sizeof(finalHeader));

	// Escribir header final
	pMRSFile->write(sizeof(finalHeader), &finalHeader);

	pMRSFile->close();
	delete pMRSFile;

	cout << "Successfully created: " << outputFile << ".mrs" << endl;
	return true;
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

	printf("MasangSoft MRS Compressor/Decompressor\n");

mainMenu:
	cout << "\nSelect operation:\n";
	cout << "1. Extract MRS file\n";
	cout << "2. Create MRS file from folder\n";
	cout << "3. Exit\n";
	cout << "Choice: ";

	int choice;
	cin >> choice;
	cin.ignore(); // Clear input buffer

	switch (choice) {
		case 1: {
			// Existing extraction code
		extractBegin:
			cout << "\nPlease enter the file name: ";
			string strFile;
			getline(cin, strFile);

			File* pFile = new File();
			if(!pFile->open(strFile + ".mrs")) {
				cout << "File not found\n";
				goto extractBegin;
			}

			pFile->setOffset(-(int)sizeof(MZIPDIRHEADER), SEEK_END);
			size_t offSet = pFile->getOffset();

			MZIPDIRHEADER dh;
			pFile->read((char*)&dh, sizeof(MZIPDIRHEADER));
			dec((char*)&dh, sizeof(MZIPDIRHEADER));

			if (dh.sig != MRS2_ZIP_CODE && dh.sig != MRS_ZIP_CODE) {
				cout << "File is not valid wrong signature " << dh.sig << "\n";
				goto extractBegin;
			}

			pFile->setOffset(offSet - dh.dirSize, SEEK_SET);

			char* szBuffer = new char[dh.dirSize + dh.nDirEntries * sizeof(MZIPDIRFILEHEADER)];
			pFile->read(szBuffer, dh.dirSize);
			dec(szBuffer, dh.dirSize);

			MZIPDIRFILEHEADER ** pDir = (MZIPDIRFILEHEADER **)(szBuffer + dh.dirSize);

			char* szTemp = move(szBuffer);
			for (int i = 0; i < dh.nDirEntries; i++) {
				MZIPDIRFILEHEADER& p = *(MZIPDIRFILEHEADER*)szTemp;

				if (p.sig != MZIPDIRFILEHEADER::SIGNATURE && p.sig != MZIPDIRFILEHEADER::SIGNATURE2)
				{
					cout << "Dir File header wrong signature " << p.sig << "\n";
					break;
				}

				szTemp += sizeof(p);

				for (int j = 0; j < p.fnameLen; j++) {
					if (szTemp[j] == '/') {
						szTemp[j] = '\\';
					}
				}

				pDir[i] = &p;

				szTemp += p.fnameLen + p.xtraLen + p.cmntLen;
			}

		#define GetFileName(i, szDest) \
		{ \
			if (szDest != NULL) \
			{ \
				memcpy(szDest, pDir[i]->GetName(), pDir[i]->fnameLen); \
				szDest[pDir[i]->fnameLen] = '\0'; \
			} \
		} \

			for (int i = 0; i < dh.nDirEntries; i++)
			{
				char szFileName[_MAX_PATH];
				GetFileName(i, szFileName);

				MZIPDIRFILEHEADER* dh = pDir[i];

				pFile->setOffset(dh->hdrOffset, SEEK_SET);

				MZIPLOCALHEADER lh;
				pFile->read(&lh, sizeof(MZIPLOCALHEADER));
				dec((char*)&lh, sizeof(MZIPLOCALHEADER));

				if (lh.sig != MZIPLOCALHEADER::SIGNATURE && lh.sig != MZIPLOCALHEADER::SIGNATURE2) {
					cout << "Local header wrong signature " << lh.sig << "\n";
					break;
				}

				pFile->skip(lh.fnameLen + lh.xtraLen);

				if (lh.compression != MZIPLOCALHEADER::COMP_DEFLAT) {
					cout << "Local header wrong compression (" << lh.compression << ")\n";
					break;
				}

				char* szBufferC = new char[lh.cSize];
				pFile->read(szBufferC, lh.cSize);

				char* szBufferU = new char[lh.ucSize];
				z_stream stream;

				stream.zalloc = (alloc_func)0;
				stream.zfree = (free_func)0;
				stream.opaque = (voidpf)0;

				stream.next_in = (Bytef*)szBufferC;
				stream.avail_in = (uInt)lh.cSize;
				stream.next_out = (Bytef*)szBufferU;
				stream.avail_out = min(dh->ucSize, lh.ucSize);

				int err = inflateInit2(&stream, -MAX_WBITS);
				if (err == Z_OK)
				{
					err = inflate(&stream, Z_FINISH);
					inflateEnd(&stream);
					if (err == Z_STREAM_END)
					{
						err = Z_OK;
					}
					inflateEnd(&stream);
				}

				if (err == Z_OK)
				{
					char szFinalPath[256];
					sprintf_s(szFinalPath, "MRS\\%s\\%s", strFile.c_str(), szFileName);

					for (int j = 0; j < sizeof(szFinalPath); j++)
					{
						if (szFinalPath[j] == '/')
							szFinalPath[j] = '\\';
					}

					createDir(szFinalPath);
					printf("%s\n", szFinalPath);

					File* pTemp = new File();
					if (!pTemp->open(szFinalPath, FF_WRITE)) {
						cout << "Failed to write " << szFinalPath << "\n";
						continue;
					}

					pTemp->write(dh->ucSize, szBufferU);
					pTemp->close();
				}

				delete[] szBufferC;
				delete[] szBufferU;
			}

			pFile->close();
			break;
		}

		case 2: {
			// New compression functionality
			cout << "\nEnter folder path to compress: ";
			string folderPath;
			getline(cin, folderPath);

			cout << "Enter output filename (without .mrs extension): ";
			string outputFile;
			getline(cin, outputFile);

			if (!createMRS(folderPath, outputFile)) {
				cout << "Failed to create MRS file\n";
			}
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
