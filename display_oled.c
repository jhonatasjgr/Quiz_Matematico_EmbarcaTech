#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

const uint BTN_TRUE = 5;  // Botão para "Verdadeiro"
const uint BTN_FALSE = 6; // Botão para "Falso"

typedef struct {
    char question[32];
    char resposte[32];
    bool resposta; // true para "Verdadeiro", false para "Falso"
} Questao;

// Lista de perguntas
Questao quiz[10] = {
    {"34 menos 7", "igual a 220", false},
    {"51 vezes 15", "igual a 765", true},  
    {"42 mais 6", "igual a 48", true},       
    {"57 menos 8", "igual a 480", false},
    {"17 menos 5", "igual a 12", true},  
    {"25 vezes 13", "igual a 325", true},   
    {"11 mais 19", "igual a 210", false},   
    {"29 mais 14", "igual a 420", false},  
    {"32 vezes 8", "igual a 256", true},     
    {"18 vezes 4", "igual a 70", false},    
};

void jogar(){
    stdio_init_all();

    // Inicializa I2C
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display SSD1306
    ssd1306_init();

    // Configuração de área de renderização
    struct render_area frame_area = {0, ssd1306_width - 1, 0, ssd1306_n_pages - 1};
    calculate_render_area_buffer_length(&frame_area);

    // Inicializa botões
    gpio_init(BTN_TRUE);
    gpio_set_dir(BTN_TRUE, GPIO_IN);
    gpio_pull_up(BTN_TRUE);

    gpio_init(BTN_FALSE);
    gpio_set_dir(BTN_FALSE, GPIO_IN);
    gpio_pull_up(BTN_FALSE);

    // Variáveis de pontuação
    int correct = 0, wrong = 0;

    uint8_t ssd[ssd1306_buffer_length];
    int iniciado =0;
    while(!iniciado){
        memset(ssd, 0, ssd1306_buffer_length);
        ssd1306_draw_string(ssd, 5, 10, "Pressione");
        ssd1306_draw_string(ssd, 5, 20, "o botao");
        ssd1306_draw_string(ssd, 5, 30, "para iniciar");
        render_on_display(ssd, &frame_area);
        if (gpio_get(BTN_TRUE) == 0 || gpio_get(BTN_FALSE) == 0) {
            iniciado = 1;
        }
        sleep_ms(200);
    }
    for (int i = 0; i < 10; i++) {
        memset(ssd, 0, ssd1306_buffer_length);
        ssd1306_draw_string(ssd, 5, 10, quiz[i].question);
        ssd1306_draw_string(ssd, 5, 30, quiz[i].resposte);
        ssd1306_draw_string(ssd, 5, 50," A para verdadeiro");
        ssd1306_draw_string(ssd, 5, 60," B para falso");

        render_on_display(ssd, &frame_area);
        
        // Aguarda resposta
        bool respondido = false;
        while (!respondido) {
            if (gpio_get(BTN_TRUE) == 0) {
                respondido = true;
                if (quiz[i].resposta) correct++; else wrong++;
            }else
            if (gpio_get(BTN_FALSE) == 0) {
                respondido = true;
                if (!quiz[i].resposta) correct++; else wrong++;
            }
            sleep_ms(200);
        }
    }

    // Exibe o resultado final
    memset(ssd, 0, ssd1306_buffer_length);
    char result[32];
    snprintf(result, sizeof(result), "Acertos: %d", correct);
    ssd1306_draw_string(ssd, 5, 10, result);
    snprintf(result, sizeof(result), "Erros: %d", wrong);
    ssd1306_draw_string(ssd, 5, 20, result);
    render_on_display(ssd, &frame_area);
    
    sleep_ms(7000);
    return;
}
int main() {
    
    while (1) {
        jogar();
    }
    return 0;
}
