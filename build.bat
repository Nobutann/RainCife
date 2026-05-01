@echo off
set GCC_PATH=C:\msys64\mingw64\bin\gcc.exe
echo Compilando o jogo com %GCC_PATH%...

:: Cria a pasta libs se nao existir
if not exist libs mkdir libs

:: Compila todos os arquivos .c da pasta src
%GCC_PATH% src\main.c src\game.c src\start_menu.c src\options_menu.c src\credits_menu.c src\config_manager.c src\hitbox_menu_ini.c src\hitbox_enemy.c src\utils.c -o libs\jogo.exe -I"include" -I"C:/msys64/mingw64/include" -L"C:/msys64/mingw64/lib" -lraylib -lopengl32 -lgdi32 -lwinmm

if %errorlevel% neq 0 (
    echo.
    echo [ERRO] Falha na compilacao!
    pause
    exit /b %errorlevel%
)

echo.
echo Compilacao concluida com sucesso!
echo Iniciando o jogo de dentro da pasta libs...
cd libs
if exist jogo.exe (
    jogo.exe
) else (
    echo [ERRO] jogo.exe nao encontrado na pasta libs!
)
cd ..
pause
