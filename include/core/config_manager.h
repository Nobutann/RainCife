#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

typedef struct {
    int idioma;
    int telaCheia;
    float volume;
    float musica;
    int teclaFrente;
    int teclaTras;
    int teclaPular;
    int teclaAgachar;
} Config;

void SalvarConfig(Config config);
Config CarregarConfig();

#endif
