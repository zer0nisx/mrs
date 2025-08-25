# MRS - Compressor/Decompressor Extendido
MasangSoft MRS con funcionalidades de compresión y descompresión

## Funcionalidades
- ✅ **Descompresión**: Extrae archivos .mrs existentes (funcionalidad original)
- ✅ **Compresión**: Crea archivos .mrs desde carpetas (nueva funcionalidad)
- ✅ **Soporte recursivo**: Maneja subcarpetas automáticamente
- ✅ **Compatibilidad**: 100% compatible con archivos MRS existentes

## Instrucciones Detalladas
Ver `README_COMPRESSION.md` para instrucciones completas de uso y compilación.

## 🔧 Herramientas de Debug
- `debug_signatures` - Diagnóstico de signatures y encriptación
- `debug_sizes` - Verificación de tamaños de estructuras

## Uso Rápido

### 🚀 CMake (Recomendado)
```bash
# Linux/macOS
./build.sh release
cd build/Source/Mrs && ./Mrs

# Windows (Visual Studio)
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

### 📋 Alternativas
```bash
# Makefile tradicional (Linux)
make

# Visual Studio (Windows)
Abrir Masangsoft.sln
```

📖 **Guía completa de CMake**: Ver `BUILD_CMAKE.md`
