@echo off
echo ========================================
echo MRS Compressor/Decompressor - Ejemplos
echo ========================================
echo.

echo Creando carpetas de ejemplo...
mkdir test_folder 2>nul
mkdir test_folder\subfolder 2>nul
echo Archivo de prueba 1 > test_folder\file1.txt
echo Archivo de prueba 2 > test_folder\file2.txt
echo Archivo en subcarpeta > test_folder\subfolder\subfile.txt

echo.
echo Carpetas creadas:
echo - test_folder/
echo   - file1.txt
echo   - file2.txt
echo   - subfolder/
echo     - subfile.txt
echo.

echo ========================================
echo Ejecutando Mrs.exe...
echo ========================================
echo.
echo "Selecciona opcion 2 para comprimir test_folder"
echo "Ingresa: test_folder"
echo "Nombre de salida: ejemplo"
echo.

pause
Mrs.exe

echo.
echo ========================================
echo Verificando archivos creados...
echo ========================================

if exist ejemplo.mrs (
    echo [OK] ejemplo.mrs creado exitosamente
    dir ejemplo.mrs
) else (
    echo [ERROR] No se creo ejemplo.mrs
)

echo.
echo Para probar la descompresion:
echo 1. Ejecuta Mrs.exe nuevamente
echo 2. Selecciona opcion 1
echo 3. Ingresa: ejemplo
echo 4. Los archivos se extraeran en MRS\ejemplo\

pause
