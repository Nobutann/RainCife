@echo off

:: Detecta qual GCC esta disponivel
if exist C:\msys64\mingw64\bin\gcc.exe (
    set GCC_PATH=C:\msys64\mingw64\bin\gcc.exe
    set INC=-I"include" -I"C:/msys64/mingw64/include"
    set LIB=-L"C:/msys64/mingw64/lib"
) else if exist C:\raylib\w64devkit\bin\gcc.exe (
    set GCC_PATH=C:\raylib\w64devkit\bin\gcc.exe
    set INC=-I"include" -I"C:/raylib-5.5_win64_mingw-w64/include"
    set LIB=-L"C:/raylib-5.5_win64_mingw-w64/lib"
) else (
    echo [ERRO] Nenhum GCC encontrado! Instale MSYS2 ou w64devkit.
    pause
    exit /b 1
)

echo Compilando com %GCC_PATH%...
if not exist libs mkdir libs

%GCC_PATH% src\core\main.c src\core\start_menu.c src\core\options_menu.c src\core\credits_menu.c src\core\config_manager.c src\core\cursor.c src\entities\player.c src\entities\hairy_leg.c src\entities\shark.c src\entities\enemy.c src\gameplay\weapon.c src\gameplay\levels.c src\graphics\background.c src\graphics\sprites.c src\utils.c src\enemy_caller.c -o libs\jogo.exe %INC% %LIB% -lraylib -lopengl32 -lgdi32 -lwinmm

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
