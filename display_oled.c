#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

//pinos do display oled
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
//pinos dos botoes
const uint BTN_TRUE = 5;  
const uint BTN_FALSE = 6; 
//pinos dos leds
const uint LED_VERDE = 11;
const uint LED_VERMELHO = 13;

typedef struct {
    char questao[32];
    char afirmacao[32];
    bool resposta; 
} Questao;

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

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    // Inicializa I2C
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display oled
    ssd1306_init();

    // Configuração de area de renderização
    struct render_area frame_area = {0, ssd1306_width - 1, 0, ssd1306_n_pages - 1};
    calculate_render_area_buffer_length(&frame_area);

    // Inicializa botoes
    gpio_init(BTN_TRUE);
    gpio_set_dir(BTN_TRUE, GPIO_IN);
    gpio_pull_up(BTN_TRUE);

    gpio_init(BTN_FALSE);
    gpio_set_dir(BTN_FALSE, GPIO_IN);
    gpio_pull_up(BTN_FALSE);

    // Variaveis de pontuacao
    int corretas = 0, erradas = 0;

    uint8_t ssd[ssd1306_buffer_length];

    int iniciado =0;
    while(!iniciado){
        memset(ssd, 0, ssd1306_buffer_length);
        ssd1306_draw_string(ssd, 5, 10, "Pressione");
        ssd1306_draw_string(ssd, 5, 25, "o botao");
        ssd1306_draw_string(ssd, 5, 40, "para iniciar");
        render_on_display(ssd, &frame_area);
        if (gpio_get(BTN_TRUE) == 0 || gpio_get(BTN_FALSE) == 0) {
            iniciado = 1;
        }
        sleep_ms(100); // evitar duplo clique
    }

    for (int i = 0; i < 10; i++) {
        memset(ssd, 0, ssd1306_buffer_length);
        ssd1306_draw_string(ssd, 5, 15, quiz[i].questao);
        ssd1306_draw_string(ssd, 5, 25, quiz[i].afirmacao);
        ssd1306_draw_string(ssd, 0, 40," A para verdade");
        ssd1306_draw_string(ssd, 0, 50," B para falso");

        render_on_display(ssd, &frame_area);
        
        
        bool respondido = false;
        while (!respondido) {
            if (gpio_get(BTN_TRUE) == 0) {
                respondido = true;
                if (quiz[i].resposta) corretas++; else erradas++;
            } 
            else if (gpio_get(BTN_FALSE) == 0) {
                respondido = true;
                if (!quiz[i].resposta) corretas++; else erradas++;
            }
            sleep_ms(200);
        }
    }

    // resultado final
    memset(ssd, 0, ssd1306_buffer_length);
    char result[32];
    snprintf(result, sizeof(result), "Acertos: %d", corretas);
    ssd1306_draw_string(ssd, 5, 10, result);
    snprintf(result, sizeof(result), "Erros: %d", erradas);
    ssd1306_draw_string(ssd, 5, 20, result);
    if(corretas>6){
        snprintf(result, sizeof(result), "Parabens!");
        ssd1306_draw_string(ssd, 5, 40, result);
        gpio_put(LED_VERDE, 1);
    }else{
        snprintf(result, sizeof(result), "Estude Mais!");
        ssd1306_draw_string(ssd, 5, 40, result);
        gpio_put(LED_VERMELHO, 1);
    }
   
    render_on_display(ssd, &frame_area);
    
    sleep_ms(7000);

    gpio_put(LED_VERDE, 0);
    gpio_put(LED_VERMELHO, 0);
    return;
}
int main() {
    
    while (1) {
        jogar();
    }
    return 0;
}
