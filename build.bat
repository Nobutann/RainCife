@echo off
set GCC_PATH=C:\msys64\mingw64\bin\gcc.exe
echo Compilando o jogo com %GCC_PATH%...

%GCC_PATH% menu_inicial.c hitbox_menu_ini.c menu_opcoes.c menu_creditos.c config_manager.c -o libs/jogo.exe -I"C:/msys64/mingw64/include" -L"C:/msys64/mingw64/lib" -lraylib -lopengl32 -lgdi32 -lwinmm

if %errorlevel% neq 0 (
    echo.
    echo [ERRO] Falha na compilacao!
    pause
    exit /b %errorlevel%
)

echo Compilacao concluida com sucesso!
echo Iniciando o jogo de dentro da pasta libs...
cd libs
jogo.exe
cd ..
pause
