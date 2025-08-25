#!/bin/bash

# MRS Compressor/Decompressor - Build Script para Linux/macOS
# Uso: ./build.sh [debug|release] [clean]

set -e  # Salir en caso de error

# Configuración por defecto
BUILD_TYPE="Release"
CLEAN_BUILD=false
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

# Función para mostrar ayuda
show_help() {
    echo "MRS Compressor/Decompressor - Build Script"
    echo ""
    echo "Uso: $0 [opciones]"
    echo ""
    echo "Opciones:"
    echo "  debug      Compilar en modo Debug"
    echo "  release    Compilar en modo Release (por defecto)"
    echo "  clean      Limpiar build anterior"
    echo "  help       Mostrar esta ayuda"
    echo ""
    echo "Ejemplos:"
    echo "  $0                  # Compilar en Release"
    echo "  $0 debug           # Compilar en Debug"
    echo "  $0 release clean   # Limpiar y compilar en Release"
    echo ""
}

# Procesar argumentos
for arg in "$@"; do
    case $arg in
        debug)
            BUILD_TYPE="Debug"
            ;;
        release)
            BUILD_TYPE="Release"
            ;;
        clean)
            CLEAN_BUILD=true
            ;;
        help|--help|-h)
            show_help
            exit 0
            ;;
        *)
            echo "Argumento desconocido: $arg"
            show_help
            exit 1
            ;;
    esac
done

echo "========================================"
echo "MRS Compressor/Decompressor - Build"
echo "========================================"
echo "Build Type: $BUILD_TYPE"
echo "Clean Build: $CLEAN_BUILD"
echo "Source Dir: $SCRIPT_DIR"
echo "Build Dir: $BUILD_DIR"
echo ""

# Verificar dependencias
echo "Verificando dependencias..."

if ! command -v cmake &> /dev/null; then
    echo "❌ Error: CMake no está instalado"
    echo "Instalar con: sudo apt-get install cmake"
    exit 1
fi

if ! command -v g++ &> /dev/null; then
    echo "❌ Error: g++ no está instalado"
    echo "Instalar con: sudo apt-get install build-essential"
    exit 1
fi

# Verificar zlib
if ! pkg-config --exists zlib; then
    echo "⚠️  Advertencia: zlib no encontrada en el sistema"
    echo "Se recomienda instalar: sudo apt-get install zlib1g-dev"
    echo "Continuando con la librería incluida..."
else
    echo "✅ zlib encontrada: $(pkg-config --modversion zlib)"
fi

echo "✅ CMake: $(cmake --version | head -n1)"
echo "✅ g++: $(g++ --version | head -n1)"
echo ""

# Limpiar si se solicita
if [ "$CLEAN_BUILD" = true ]; then
    echo "🧹 Limpiando build anterior..."
    rm -rf "$BUILD_DIR"
fi

# Crear directorio de build
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configurar con CMake
echo "⚙️  Configurando con CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX="$SCRIPT_DIR/install" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Compilar
echo ""
echo "🔨 Compilando..."
make -j$(nproc)

# Verificar que se creó el ejecutable
if [ -f "Source/Mrs/Mrs" ]; then
    echo ""
    echo "✅ Compilación exitosa!"
    echo "📁 Ejecutable: $BUILD_DIR/Source/Mrs/Mrs"
    echo ""
    echo "Para ejecutar:"
    echo "  cd $BUILD_DIR/Source/Mrs"
    echo "  ./Mrs"
    echo ""
    echo "Para instalar:"
    echo "  make install"
    echo ""
else
    echo ""
    echo "❌ Error: El ejecutable no se creó correctamente"
    exit 1
fi
