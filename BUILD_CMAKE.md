# MRS - Guía de Construcción con CMake

Este proyecto ahora soporta **CMake** como sistema de construcción moderno y multiplataforma, además del sistema Visual Studio original.

## 🚀 Inicio Rápido

### Linux/macOS (Recomendado)
```bash
# 1. Clonar el repositorio
git clone https://github.com/zer0nisx/mrs.git
cd mrs

# 2. Instalar dependencias (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install cmake build-essential zlib1g-dev

# 3. Compilar usando el script
./build.sh release

# 4. Ejecutar
cd build/Source/Mrs
./Mrs
```

### Windows (Visual Studio)
```cmd
# 1. Instalar Visual Studio 2019+ con CMake support
# 2. Abrir como carpeta CMake en Visual Studio
# O usar línea de comandos:

mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## 📋 Requisitos

### Dependencias Obligatorias
- **CMake 3.16+**
- **C++17 compatible compiler**:
  - GCC 7+ (Linux)
  - Clang 5+ (macOS)
  - Visual Studio 2019+ (Windows)

### Dependencias Opcionales
- **zlib** (recomendado instalar desde sistema)
  - Ubuntu/Debian: `sudo apt-get install zlib1g-dev`
  - macOS: `brew install zlib`
  - Windows: Se usa la versión incluida automáticamente

## 🛠️ Métodos de Construcción

### 1. Script de Construcción (Linux/macOS)
```bash
# Compilar en Release (recomendado)
./build.sh release

# Compilar en Debug
./build.sh debug

# Limpiar y recompilar
./build.sh release clean

# Ver ayuda
./build.sh help
```

### 2. CMake Manual
```bash
# Configurar
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compilar
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS

# Instalar (opcional)
sudo make install
```

### 3. CMake con Presets
```bash
# Ver presets disponibles
cmake --list-presets

# Configurar con preset
cmake --preset linux-release

# Compilar con preset
cmake --build --preset linux-release
```

### 4. Visual Studio Code (con CMake extension)
1. Instalar extensión "CMake Tools"
2. Abrir carpeta del proyecto
3. Ctrl+Shift+P → "CMake: Configure"
4. Ctrl+Shift+P → "CMake: Build"

### 5. CLion (JetBrains)
1. Abrir proyecto (detecta CMake automáticamente)
2. Configurar toolchain si es necesario
3. Build → Build Project

## ⚙️ Configuraciones Disponibles

### Build Types
- **Debug**: Símbolos de debug, sin optimizaciones
- **Release**: Optimizaciones máximas, sin debug
- **RelWithDebInfo**: Optimizaciones + símbolos debug
- **MinSizeRel**: Optimización para tamaño mínimo

### Opciones CMake
```bash
# Especificar directorio de instalación
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local

# Forzar compilador específico
cmake .. -DCMAKE_CXX_COMPILER=g++-9

# Usar zlib del sistema (auto-detectado)
cmake .. -DZLIB_ROOT=/usr/local

# Verbose build (ver comandos completos)
make VERBOSE=1
```

## 📁 Estructura del Proyecto

```
mrs/
├── CMakeLists.txt              # Configuración principal
├── CMakePresets.json           # Presets para IDEs
├── build.sh                    # Script de construcción
├── Source/
│   ├── FileSystem/
│   │   ├── CMakeLists.txt      # Librería FileSystem
│   │   ├── Include/
│   │   └── Source/
│   └── Mrs/
│       ├── CMakeLists.txt      # Ejecutable principal
│       ├── Include/
│       ├── Source/
│       └── zlib/               # zlib incluida (Windows)
└── build/                      # Archivos generados
    ├── Source/Mrs/Mrs          # Ejecutable final
    └── ...
```

## 🎯 Targets Disponibles

```bash
# Targets principales
make Mrs           # Compilar ejecutable
make FileSystem    # Compilar librería

# Targets de utilidad
make install       # Instalar en sistema
make clean         # Limpiar archivos objeto
make run           # Ejecutar Mrs
make clean-test    # Limpiar archivos de prueba
```

## 🐛 Solución de Problemas

### Error: "zlib not found"
```bash
# Ubuntu/Debian
sudo apt-get install zlib1g-dev

# CentOS/RHEL
sudo yum install zlib-devel

# macOS
brew install zlib
```

### Error: "CMake version too old"
```bash
# Ubuntu 18.04+
sudo apt-get install cmake

# Ubuntu 16.04 (instalar desde snap)
sudo snap install cmake --classic

# CentOS 7+ (habilitar EPEL)
sudo yum install epel-release
sudo yum install cmake3
```

### Error: "C++17 not supported"
```bash
# Actualizar GCC
sudo apt-get install gcc-7 g++-7

# Especificar compilador
cmake .. -DCMAKE_CXX_COMPILER=g++-7
```

### Windows: Error de MSVC
- Asegurar Visual Studio 2019+ instalado
- Usar "Developer Command Prompt"
- Verificar componentes C++ instalados

## 🔄 Migración desde Makefile

El proyecto mantiene ambos sistemas:
- **CMake**: Sistema principal recomendado
- **Makefile**: Disponible para compatibilidad
- **Visual Studio**: Proyecto original mantenido

```bash
# Limpiar builds previos si hay conflictos
make clean        # Makefile
rm -rf build/     # CMake
```

## 📚 Documentación Adicional

- **Uso del programa**: Ver `README_COMPRESSION.md`
- **Desarrollo**: Ver comentarios en `CMakeLists.txt`
- **Ejemplos**: Ejecutar `example_usage.bat` (Windows)

## 🎉 Ventajas de CMake

✅ **Multiplataforma**: Windows, Linux, macOS
✅ **IDE Integration**: Visual Studio, VSCode, CLion, Qt Creator
✅ **Gestión automática de dependencias**
✅ **Configuraciones de build flexibles**
✅ **Generación de proyectos nativos**
✅ **Soporte para tests y packaging**

El sistema CMake está listo para producción y es la forma recomendada de compilar el proyecto MRS.
