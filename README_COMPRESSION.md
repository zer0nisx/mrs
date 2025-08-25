# MRS Compressor/Decompressor Extendido

Este proyecto ha sido extendido para incluir tanto la funcionalidad de **descompresión** original como la nueva funcionalidad de **compresión** para archivos MRS.

## Características

### Funcionalidades Originales (Descompresión)
- Extrae archivos de archivos `.mrs` existentes
- Descomprime usando zlib
- Desencripta usando clave XOR personalizada
- Recrea la estructura de directorios original

### Nuevas Funcionalidades (Compresión)
- Comprime carpetas completas en archivos `.mrs`
- Recorre recursivamente subdirectorios
- Comprime archivos usando zlib con máxima compresión
- Encripta usando la misma clave XOR del formato original
- Compatible con el formato MRS existente

## Cómo Usar

### Modo Interactivo
```bash
./Mrs.exe
```

El programa mostrará un menú:
1. **Extraer archivo MRS** - Funcionalidad original
2. **Crear archivo MRS desde carpeta** - Nueva funcionalidad
3. **Salir**

### Compresión (Opción 2)
1. Selecciona la opción 2
2. Ingresa la ruta de la carpeta que quieres comprimir (ej: `system`)
3. Ingresa el nombre del archivo de salida (sin extensión .mrs)
4. El programa creará `nombreArchivo.mrs`

### Descompresión (Opción 1)
1. Selecciona la opción 1
2. Ingresa el nombre del archivo MRS (sin extensión)
3. Los archivos se extraerán a `MRS\nombreArchivo\`

## Compilación

### Requisitos
- Visual Studio 2019 o superior (para Windows)
- C++17 support
- zlib library (incluida en el proyecto)

### Herramientas de Debug (Nuevas)
El proyecto incluye herramientas de diagnóstico para debugging:
- `debug_signatures.exe` - Prueba encriptación/desencriptación de signatures
- `debug_sizes.exe` - Verifica tamaños de estructuras y alineación

### Compilación con Visual Studio
1. Abre `Masangsoft.sln`
2. Asegúrate de que la configuración sea **Release** o **Debug**
3. Compila el proyecto

### Compilación con CMake (Recomendado)
```bash
# Windows
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release

# Linux/macOS
mkdir build && cd build
cmake ..
make -j4

# Compilar solo herramientas de debug
cmake --build . --target debug_tools

# Ejecutar herramientas de debug
cmake --build . --target run_debug
```

### Configuración Necesaria para Visual Studio
El archivo de proyecto necesita ser actualizado para C++17. Agrega estas líneas en cada configuración:

```xml
<LanguageStandard>stdcpp17</LanguageStandard>
<AdditionalIncludeDirectories>Include;../FileSystem/Include;zlib/include;</AdditionalIncludeDirectories>
```

Y en las configuraciones de enlazado:
```xml
<AdditionalDependencies>zlib/lib/zlib.lib;%(AdditionalDependencies)</AdditionalDependencies>
```

### Compilación Alternativa (Linux/GCC)
```bash
# Instalar zlib development headers
sudo apt-get install zlib1g-dev

# Compilar
cd Source/Mrs
g++ -std=c++17 -I Include -I ../FileSystem/Include -I zlib/include \
    Source/main.cpp Source/stdafx.cpp ../FileSystem/Source/FileSystem.cpp \
    -lz -o Mrs
```

## Estructura del Formato MRS

El formato MRS es un archivo ZIP modificado con:
- **Encriptación XOR**: Usa clave de 18 bytes fija
- **Compresión DEFLATE**: Compatible con zlib
- **Headers customizados**: Signature modificada (0x05030208)
- **Estructura ZIP**: Headers locales + directorio central + header final

## Ejemplos de Uso

### Comprimir una carpeta del sistema
```
Folder path: system/
Output filename: game_assets
-> Crea: game_assets.mrs
```

### Extraer un archivo MRS
```
Filename: game_assets
-> Extrae a: MRS/game_assets/
```

## Notas Técnicas

- Los archivos son comprimidos con máxima compresión (`Z_BEST_COMPRESSION`)
- Se preserva la estructura de directorios original
- Los CRC32 son calculados automáticamente
- La encriptación XOR es aplicada a headers y directorio central
- Compatible 100% con archivos MRS existentes

## Dependencias

- **zlib**: Para compresión/descompresión DEFLATE
- **std::filesystem**: Para recorrer directorios (C++17)
- **Windows API**: Para funciones de creación de directorios (Windows)

## Limitaciones

- Específico para Windows (usa CreateDirectoryA)
- Requiere C++17 para std::filesystem
- La clave de encriptación está hardcodeada (por compatibilidad)

## 🔧 Troubleshooting con Herramientas de Debug

Si experimentas problemas con signatures incorrectas:

### Usar debug_signatures
```bash
# Windows
debug_signatures.exe

# Linux
./debug_signatures
```
Esta herramienta verifica:
- Valores correctos de las macros MRS_ZIP_CODE y MRS2_ZIP_CODE
- Funcionamiento de encriptación/desencriptación
- Ciclo completo de escritura/lectura de signatures

### Usar debug_sizes
```bash
# Windows
debug_sizes.exe

# Linux
./debug_sizes
```
Esta herramienta verifica:
- Tamaños de tipos de datos (dword, word)
- Tamaños de estructuras con diferentes pragma pack
- Offsets de campos en las estructuras
- Detecta problemas de alineación de memoria
