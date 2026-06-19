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
    int espacoAtaca;
} Config;

void SalvarConfig(Config config);
Config CarregarConfig();
void DefinirConfigAtiva(Config *config);
void AtualizarPreferenciaEspaco(void);

#endif
