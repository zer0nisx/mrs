# Mrs Executable
set(MRS_SOURCES
    Source/main.cpp
    Source/stdafx.cpp
)

set(MRS_HEADERS
    Include/stdafx.h
)

# Verificar que tenemos los archivos necesarios
foreach(source_file ${MRS_SOURCES})
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source_file})
        message(FATAL_ERROR "Source file not found: ${source_file}")
    endif()
endforeach()

# Crear el ejecutable
add_executable(Mrs ${MRS_SOURCES} ${MRS_HEADERS})

# Configurar directorios de include
target_include_directories(Mrs PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../FileSystem/Include
)

# Configurar precompiled headers si es compatible
if(MSVC)
    target_precompile_headers(Mrs PRIVATE Include/stdafx.h)
endif()

# Enlazar con librerías
target_link_libraries(Mrs PRIVATE FileSystem)

# Configurar zlib según disponibilidad
if(ZLIB_FOUND)
    target_link_libraries(Mrs PRIVATE ZLIB::ZLIB)
    target_include_directories(Mrs PRIVATE ${ZLIB_INCLUDE_DIRS})
else()
    # Usar zlib incluida en el proyecto
    target_include_directories(Mrs PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/zlib/include
    )

    if(WIN32)
        # En Windows, usar la librería estática incluida
        target_link_libraries(Mrs PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/zlib/lib/zlib.lib
        )
    else()
        # En Linux, intentar encontrar zlib del sistema o dar error
        find_library(ZLIB_LIBRARY z)
        if(ZLIB_LIBRARY)
            target_link_libraries(Mrs PRIVATE ${ZLIB_LIBRARY})
        else()
            message(FATAL_ERROR "zlib not found. Please install zlib development packages (e.g., zlib1g-dev on Ubuntu)")
        endif()
    endif()
endif()

# Configuraciones específicas por plataforma
if(WIN32)
    target_compile_definitions(Mrs PRIVATE
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
    )

    # Configurar subsistema de consola
    set_target_properties(Mrs PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

# Configurar C++17
target_compile_features(Mrs PRIVATE cxx_std_17)

# Configurar nombre del ejecutable
set_target_properties(Mrs PROPERTIES
    OUTPUT_NAME "Mrs"
    VERSION ${PROJECT_VERSION}
)

# Configurar instalación
install(TARGETS Mrs
    RUNTIME DESTINATION bin
)

# Agregar target para ejecutar el programa
add_custom_target(run
    COMMAND Mrs
    DEPENDS Mrs
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running Mrs Compressor/Decompressor"
)

# Target para limpiar archivos de prueba
add_custom_target(clean-test
    COMMAND ${CMAKE_COMMAND} -E remove_directory test_folder
    COMMAND ${CMAKE_COMMAND} -E remove -f ejemplo.mrs
    COMMAND ${CMAKE_COMMAND} -E remove_directory MRS
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Cleaning test files"
)
