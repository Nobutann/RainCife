#include <raylib.h>

void DesenharMenuCreditos(int currentWidth, int currentHeight) {
    ClearBackground(RAYWHITE);

    const char* titulo = "CRÉDITOS";
    int titleFontSize = currentHeight / 10;
    int titleWidth = MeasureText(titulo, titleFontSize);
    DrawText(titulo, (currentWidth / 2) - (titleWidth / 2), currentHeight / 8, titleFontSize, DARKBLUE);

    int nameFontSize = currentHeight / 25;
    int startY = currentHeight / 3;
    int spacing = nameFontSize + 15;

    const char* nomes[] = { 
        "Bruno Augusto", 
        "Gustavo Torres", 
        "Igor Gabriel", 
        "Luiz Gonzaga", 
        "Marco Aurélio" 
    };
    
    for (int i = 0; i < 5; i++) {
        int textWidth = MeasureText(nomes[i], nameFontSize);
        DrawText(nomes[i], (currentWidth / 2) - (textWidth / 2), startY + (i * spacing), nameFontSize, DARKGRAY);
    }
}
