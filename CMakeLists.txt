cmake_minimum_required(VERSION 3.16)
project(MRS_CompressorDecompressor VERSION 1.0.0 LANGUAGES CXX)

# Configurar estándar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuraciones para diferentes compiladores
if(MSVC)
    # Configuración para Visual Studio
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
else()
    # Configuración para GCC/Clang
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Buscar zlib
find_package(ZLIB)

# Configurar directorios de include
set(MRS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Mrs/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/FileSystem/Include
)

# Si zlib no se encuentra del sistema, usar la versión incluida
if(NOT ZLIB_FOUND)
    message(STATUS "Using bundled zlib library")
    set(ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Source/Mrs/zlib/include)
    set(ZLIB_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Mrs/zlib/lib/zlib.lib)
    set(MRS_INCLUDE_DIRS ${MRS_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
else()
    message(STATUS "Using system zlib library")
    set(MRS_INCLUDE_DIRS ${MRS_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
endif()

# Agregar subdirectorios
add_subdirectory(Source/FileSystem)
add_subdirectory(Source/Mrs)

# Agregar herramientas de debug
add_executable(debug_signatures debug_signatures.cpp)
add_executable(debug_sizes debug_sizes.cpp)

# Configurar las herramientas de debug
target_compile_features(debug_signatures PRIVATE cxx_std_17)
target_compile_features(debug_sizes PRIVATE cxx_std_17)

if(WIN32)
    target_compile_definitions(debug_signatures PRIVATE WIN32_LEAN_AND_MEAN)
    target_compile_definitions(debug_sizes PRIVATE WIN32_LEAN_AND_MEAN)
endif()

# Target para compilar todas las herramientas de debug
add_custom_target(debug_tools
    DEPENDS debug_signatures debug_sizes
    COMMENT "Building all debug tools"
)

# Target para ejecutar todas las herramientas de debug
add_custom_target(run_debug
    COMMAND debug_signatures
    COMMAND debug_sizes
    DEPENDS debug_tools
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running all debug tools"
)

# Mostrar información de configuración
message(STATUS "")
message(STATUS "=== MRS Compressor/Decompressor Configuration ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
if(ZLIB_FOUND)
    message(STATUS "Zlib: System (${ZLIB_VERSION_STRING})")
else()
    message(STATUS "Zlib: Bundled")
endif()
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Debug tools: debug_signatures, debug_sizes")
message(STATUS "===============================================")
message(STATUS "")
