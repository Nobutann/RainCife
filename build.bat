@echo off
set GCC_PATH=C:\msys64\mingw64\bin\gcc.exe
echo Compilando o jogo com %GCC_PATH%...

:: Cria a pasta libs se nao existir
if not exist libs mkdir libs

:: Compila todos os arquivos .c
%GCC_PATH% src\core\main.c src\core\start_menu.c src\core\options_menu.c src\core\credits_menu.c src\core\config_manager.c src\entities\player.c src\entities\hairy_leg.c src\entities\enemy.c src\gameplay\weapon.c src\graphics\background.c src\graphics\sprites.c src\game.c src\utils.c src\enemy_caller.c -o libs\jogo.exe -I"include" -I"C:/msys64/mingw64/include" -L"C:/msys64/mingw64/lib" -lraylib -lopengl32 -lgdi32 -lwinmm

if %errorlevel% neq 0 (
    echo.
    echo [ERRO] Falha na compilacao!
    pause
    exit /b %errorlevel%
)

echo.
echo Compilacao concluida com sucesso!
echo Iniciando o jogo...
if exist libs\jogo.exe (
    libs\jogo.exe
) else (
    echo [ERRO] jogo.exe nao encontrado na pasta libs!
)
pause
