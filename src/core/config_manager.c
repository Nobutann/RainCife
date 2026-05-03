#include <stdio.h>
#include "core/config_manager.h"

void SalvarConfig(Config config) {
    FILE *arquivo = fopen("config.bin", "wb");
    if (arquivo) {
        fwrite(&config, sizeof(Config), 1, arquivo);
        fclose(arquivo);
    }
}

Config CarregarConfig() {
    Config config = { 0, 1, 0.5f };
    FILE *arquivo = fopen("config.bin", "rb");
    if (arquivo) {
        fread(&config, sizeof(Config), 1, arquivo);
        fclose(arquivo);
    }
    return config;
}
