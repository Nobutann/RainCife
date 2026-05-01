#include <raylib.h>

int VerificarCliqueMenu(int currentWidth, int currentHeight, int idioma) {
    int menuFontSize = currentHeight / 20;
    int startY = currentHeight / 2.5;
    int spacing = menuFontSize + 20;
    
    const char* optsPT[] = { "Jogar", "Opções", "Créditos", "Sair" };
    const char* optsEN[] = { "Play", "Options", "Credits", "Exit" };
    
    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < 4; i++) {
        const char* txt = idioma ? optsEN[i] : optsPT[i];
        int textWidth = MeasureText(txt, menuFontSize);
        
        Rectangle hitbox = {
            (float)(currentWidth / 2 - textWidth / 2 - 10), 
            (float)(startY + (i * spacing) - 5), 
            (float)(textWidth + 20), 
            (float)(menuFontSize + 10)
        };

        if (CheckCollisionPointRec(mousePos, hitbox)) {
            DrawRectangleLinesEx(hitbox, 2, GREEN);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return i;
        }
    }
    return -1;
}
