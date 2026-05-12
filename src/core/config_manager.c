#include <stdio.h>
#include <raylib.h>
#include "core/config_manager.h"

void SalvarConfig(Config config) {
    FILE *arquivo = fopen("config.bin", "wb");
    if (arquivo) {
        fwrite(&config, sizeof(Config), 1, arquivo);
        fclose(arquivo);
    }
}

Config CarregarConfig() {
    Config config = { 0, 1, 0.5f, 0.5f, KEY_D, KEY_A, KEY_W, KEY_S };
    FILE *arquivo = fopen("config.bin", "rb");
    if (arquivo) {
        Config lida = config;
        size_t bytesLidos = fread(&lida, 1, sizeof(Config), arquivo);
        if (bytesLidos >= sizeof(int) * 2 + sizeof(float) * 2) {
            config.idioma = lida.idioma;
            config.telaCheia = lida.telaCheia;
            config.volume = lida.volume;
            config.musica = lida.musica;
        }
        if (bytesLidos == sizeof(Config)) {
            config.teclaFrente = lida.teclaFrente;
            config.teclaTras = lida.teclaTras;
            config.teclaPular = lida.teclaPular;
            config.teclaAgachar = lida.teclaAgachar;
        }
        fclose(arquivo);
    }
    return config;
}
