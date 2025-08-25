# FileSystem Library
set(FILESYSTEM_SOURCES
    Source/FileSystem.cpp
)

set(FILESYSTEM_HEADERS
    Include/FileSystem.h
)

# Crear librería estática
add_library(FileSystem STATIC
    ${FILESYSTEM_SOURCES}
    ${FILESYSTEM_HEADERS}
)

# Configurar propiedades de la librería
target_include_directories(FileSystem
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<INSTALL_INTERFACE:include>
)

# Configurar C++17 para esta librería
target_compile_features(FileSystem PUBLIC cxx_std_17)

# Configuraciones específicas por plataforma
if(WIN32)
    target_compile_definitions(FileSystem PRIVATE WIN32_LEAN_AND_MEAN)
endif()

# Agregar a la lista de targets para instalación
set_target_properties(FileSystem PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${FILESYSTEM_HEADERS}"
)

# Configurar instalación
install(TARGETS FileSystem
    EXPORT FileSystemTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)
