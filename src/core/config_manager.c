#include <stdio.h>
#include <stddef.h>
#include <raylib.h>
#include "core/config_manager.h"

static Config *configAtiva = NULL;

void SalvarConfig(Config config) {
    FILE *arquivo = fopen("config.bin", "wb");
    if (arquivo) {
        fwrite(&config, sizeof(Config), 1, arquivo);
        fclose(arquivo);
    }
}

Config CarregarConfig() {
    Config config = { 0, 1, 0.5f, 0.5f, KEY_D, KEY_A, KEY_SPACE, KEY_S, 0 };
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
        if (bytesLidos >= offsetof(Config, espacoAtaca)) {
            config.teclaFrente = lida.teclaFrente;
            config.teclaTras = lida.teclaTras;
            config.teclaPular = lida.teclaPular;
            config.teclaAgachar = lida.teclaAgachar;
        }
        if (bytesLidos >= sizeof(Config)) {
            config.espacoAtaca = lida.espacoAtaca != 0;
        }
        fclose(arquivo);
    }
    return config;
}

void DefinirConfigAtiva(Config *config) {
    configAtiva = config;
}

void AtualizarPreferenciaEspaco(void) {
    if (configAtiva != NULL && IsKeyPressed(KEY_P)) {
        configAtiva->espacoAtaca = !configAtiva->espacoAtaca;
        SalvarConfig(*configAtiva);
    }
}
