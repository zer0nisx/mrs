# Makefile para MRS Compressor/Decompressor
# Requiere zlib development headers: sudo apt-get install zlib1g-dev

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -ISource/Mrs/Include -ISource/FileSystem/Include -ISource/Mrs/zlib/include
LIBS = -lz

# Directorios
SRC_DIR = Source
MRS_SRC = $(SRC_DIR)/Mrs/Source
FS_SRC = $(SRC_DIR)/FileSystem/Source
BUILD_DIR = build

# Archivos fuente
SOURCES = $(MRS_SRC)/main.cpp $(MRS_SRC)/stdafx.cpp $(FS_SRC)/FileSystem.cpp
OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
TARGET = Mrs

# Regla principal
all: $(BUILD_DIR) $(TARGET)

# Crear directorio de compilación
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/$(MRS_SRC)
	mkdir -p $(BUILD_DIR)/$(FS_SRC)

# Compilar ejecutable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $@

# Compilar archivos objeto
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Limpiar archivos compilados
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Instalar (copiar a /usr/local/bin)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Desinstalar
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Compilación para debug
debug: CXXFLAGS += -g -DDEBUG
debug: all

# Mostrar ayuda
help:
	@echo "Makefile para MRS Compressor/Decompressor"
	@echo ""
	@echo "Objetivos disponibles:"
	@echo "  all     - Compilar el proyecto (default)"
	@echo "  debug   - Compilar con información de debug"
	@echo "  clean   - Limpiar archivos compilados"
	@echo "  install - Instalar en /usr/local/bin"
	@echo "  uninstall - Desinstalar de /usr/local/bin"
	@echo "  help    - Mostrar esta ayuda"
	@echo ""
	@echo "Requisitos:"
	@echo "  sudo apt-get install zlib1g-dev"

.PHONY: all clean install uninstall debug help
