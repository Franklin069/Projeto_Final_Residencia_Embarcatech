

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

// Definição dos pinos para comunicação I2C
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Definição dos pinos para periféricos
#define LED_GREEN 11 
#define LED_BLUE 12  
#define LED_RED 13  
#define SW_PIN 22 
#define VRX_PIN 26  
#define VRY_PIN 27  
#define BUZZER_PIN 10  
#define BTN_A 5
#define BTN_B 6  // Novo botão B

// Variáveis de controle do menu
bool in_submenu = false;
int current_menu = 0;
int menu_index = 0;
int last_vry_value = 0;
bool sw_pressed = false;
bool last_sw_state = false;
bool menu_updated = false;
int vry_value;
bool selecting_option = false;

// Títulos do menu principal
const char *menu_titles[] = {"Comunicador ", "Urgencia", "Necessito", "Sentimentos", "Agradecer"};

// Função para desenhar string com escala
void ssd1306_draw_string_scaled(uint8_t *buffer, int x, int y, const char *text, int scale) {
    while (*text) {
        for (int dx = 0; dx < scale; dx++) {
            for (int dy = 0; dy < scale; dy++) {
                ssd1306_draw_char(buffer, x + dx, y + dy, *text);
            }
        }
        x += 6 * scale;
        text++;
    }
}

// Definição das opções de menu
const char *menus[][6] = {  
    {"Retornar", "X", "Y", "W", "", ""},
    {"Retornar", "Ajuda", "Dor", "Enjoado", "Mal estar", "Mudar"},
    {"Retornar", "Sede", "Banho", "Comida", "Banheiro", "Passear",},  
    {"Retornar", "Triste", "Alegre", "Cansaco", "Medo", "Sono",},   
    {"Retornar", "Incrivel", "Obrigado", "Adoro voce", "Muito bom", "Estou feliz",}  
};
  

// Função para tom
void tone(unsigned int frequency, unsigned int duration) {  
    unsigned long period = 1000000 / frequency; 
    unsigned long end_time = time_us_64() + (duration * 1000); 

    while (time_us_64() < end_time) {  
        gpio_put(BUZZER_PIN, 1);  
        sleep_us(period / 2);  
        gpio_put(BUZZER_PIN, 0);  
        sleep_us(period / 2);  
    }  
}

// Função que gera um som de buzina por 3 segundos
void buzina() {
    const unsigned int frequency = 500; // Frequência da buzina em Hz
    const unsigned int duration = 2000; // Duração total em ms

    unsigned long period = 1000000 / frequency; // em microssegundos
    unsigned long end_time = time_us_64() + (duration * 1000);

    while (time_us_64() < end_time) {
        gpio_put(BUZZER_PIN, 1);
        sleep_us(period / 2);
        gpio_put(BUZZER_PIN, 0);
        sleep_us(period / 2);
    }
}


// Função para print menu no console
void print_menu() {  
    if (in_submenu) {  
        printf("\n%s:\n", menu_titles[current_menu]);  
        for (int i = 0; i < 6; i++) {  
            if (menus[current_menu][i][0] != '\0') {  
                if (i == menu_index) {  
                    printf("-> %s\n", menus[current_menu][i]);  
                } else {  
                    printf("   %s\n", menus[current_menu][i]);  
                }  
            }  
        }  
    } else {  
        printf("***************************\n");  
        for (int i = 0; i < 5; i++) {  
            if (i == current_menu) {  
                printf(">> %s\n", menu_titles[i]);  
            } else {  
                printf("   %s\n", menu_titles[i]);  
            }    
        }  
        printf("***************************\n");  
    }  
}

// Atualiza LEDs
void update_leds() {  
    if ((current_menu == 1 && menu_index <= 4) ||  
        (current_menu == 2 && menu_index <= 3)) { 
        gpio_put(LED_RED, true);  
        gpio_put(LED_GREEN, false); 
        gpio_put(LED_BLUE, false);
    } else if ((current_menu == 3 && menu_index <= 3) ||
               (current_menu == 4 && menu_index <= 3)) {          
        gpio_put(LED_GREEN, true);
        gpio_put(LED_RED, false);
        gpio_put(LED_BLUE, false);
    } else {  
        gpio_put(LED_RED, false);
        gpio_put(LED_GREEN, false);
        gpio_put(LED_BLUE, true);
    }  
}

int main() {
    stdio_init_all();
    print_menu();  
    
    adc_init();  
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);
    
    gpio_init(SW_PIN);  
    gpio_set_dir(SW_PIN, GPIO_IN);  
    gpio_pull_up(SW_PIN);  

    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);

    gpio_init(BTN_B);
    gpio_set_dir(BTN_B, GPIO_IN);
    gpio_pull_up(BTN_B);

    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
    ssd1306_draw_string_scaled(ssd, 25, 30, "EASY", 2);
    ssd1306_draw_string_scaled(ssd, 35, 50, "TALK", 2);
    render_on_display(ssd, &frame_area);

    gpio_init(LED_RED);  
    gpio_init(LED_GREEN);  
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_RED, GPIO_OUT);  
    gpio_set_dir(LED_GREEN, GPIO_OUT);  
    gpio_set_dir(LED_BLUE, GPIO_OUT);  
    gpio_put(LED_RED, false);  
    gpio_put(LED_GREEN, false); 
    gpio_put(LED_BLUE, false);

    gpio_init(BUZZER_PIN);  
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);  
    gpio_put(BUZZER_PIN, 0);  

    uint16_t last_vrx_value = 2048;  
    uint16_t last_vry_value = 2048;  
    bool last_sw_state = false; 

    while (true) {
        adc_select_input(0);
        uint16_t vrx_value = adc_read();
        adc_select_input(1);
        uint16_t vry_value = adc_read();
        bool sw_pressed = !gpio_get(SW_PIN);

        // Botão A (mantido igual)
                if (gpio_get(BTN_A) == 0) {
            sleep_ms(200);
            if (gpio_get(BTN_A) == 0) {
                for (int i = 0; i < 38; i++) {  // mais ciclos para manter a duração total
                    gpio_put(LED_GREEN, false);
                    gpio_put(LED_BLUE, false);
                    gpio_put(LED_RED, true);

                    tone(1000, 100); // som mais curto e mais agudo
                    sleep_ms(50);   // intervalo menor

                    gpio_put(LED_RED, false);

                    memset(ssd, 0, ssd1306_buffer_length);
                    render_on_display(ssd, &frame_area);
                    ssd1306_draw_string_scaled(ssd, 20, 30, "SOCORRO", 2);
                    render_on_display(ssd, &frame_area);
                }
            }
        }

                // Verifica se o botão B foi pressionado
        if (!gpio_get(BTN_B)) {
            sleep_ms(200); // debounce
            if (!gpio_get(BTN_B)) {
                buzina(); // Toca a buzina
            }
        }


        // Resto igual...
        if (!in_submenu) {
            if (vrx_value > 1000 && last_vrx_value <= 1000) {
                current_menu = (current_menu + 1) % 5;
                menu_updated = true;
                tone(500, 10);
                memset(ssd, 0, ssd1306_buffer_length);
                render_on_display(ssd, &frame_area);
                ssd1306_draw_string_scaled(ssd, 0, 30, menu_titles[current_menu], 2);
                render_on_display(ssd, &frame_area);
            } else if (vrx_value < 3000 && last_vrx_value >= 3000) {
                current_menu = (current_menu - 1 + 5) % 5;
                menu_updated = true;
                tone(500, 10);
                memset(ssd, 0, ssd1306_buffer_length);
                render_on_display(ssd, &frame_area);
                ssd1306_draw_string_scaled(ssd, 0, 30, menu_titles[current_menu], 2);
                render_on_display(ssd, &frame_area);
            }

            if (sw_pressed && !last_sw_state) {
                in_submenu = true;
                menu_index = 0;
                menu_updated = true;
                sleep_ms(200);
                memset(ssd, 0, ssd1306_buffer_length);
                render_on_display(ssd, &frame_area);
                ssd1306_draw_string_scaled(ssd, 0, 30, menus[current_menu][menu_index], 2);
                render_on_display(ssd, &frame_area);
            }
        } else {
            if (!selecting_option) {
                if (vrx_value > 1000 && last_vrx_value <= 1000) {
                    menu_index = (menu_index + 1) % 5;
                    menu_updated = true;
                    tone(500, 10);
                    memset(ssd, 0, ssd1306_buffer_length);
                    render_on_display(ssd, &frame_area);
                    ssd1306_draw_string_scaled(ssd, 0, 30, menus[current_menu][menu_index], 2);
                    render_on_display(ssd, &frame_area);
                } else if (vrx_value < 3000 && last_vrx_value >= 3000) {
                    menu_index = (menu_index - 1 + 5) % 5;
                    menu_updated = true;
                    tone(500, 10);
                    memset(ssd, 0, ssd1306_buffer_length);
                    render_on_display(ssd, &frame_area);
                    ssd1306_draw_string_scaled(ssd, 0, 30, menus[current_menu][menu_index], 2);
                    render_on_display(ssd, &frame_area);
                }
            }

            if (sw_pressed && !last_sw_state) {
            if (selecting_option) {
                // Já estamos exibindo ou executando a opção escolhida
                printf("\nSelecionado: %s\n", menus[current_menu][menu_index]);

                // Toca som, acende LEDs, etc.
                if (current_menu == 1) {
                    tone(1000, 10000);
                } else {
                // Som alegre de confirmação — 3 tons ascendentes
                tone(600, 100);
                sleep_ms(50);
                tone(800, 100);
                sleep_ms(50);
                tone(1000, 100);
            }

            sleep_ms(200);

                selecting_option = false;
                in_submenu = false;
                menu_index = 0;
            } else {
                
                if (menu_index == 0) {
                    // Se for "Retornar"
                    printf("\nRetornando ao menu principal!\n");
                    in_submenu = false;
                    selecting_option = false;
                    menu_index = 0;

                    memset(ssd, 0, ssd1306_buffer_length);
                    render_on_display(ssd, &frame_area);
                    ssd1306_draw_string_scaled(ssd, 0, 30, "COMUNICADOR", 2);
                    ssd1306_draw_string_scaled(ssd, 20, 50, "TESTER", 2);
                    render_on_display(ssd, &frame_area);

                    sleep_ms(200);
                } else {
                    // Qualquer outra opção, entra em seleção
                    selecting_option = true;
                    printf("\n%s\n", menus[current_menu][menu_index]);

                    // Aqui entra seu som, LEDs, display...
                    memset(ssd, 0, ssd1306_buffer_length);
                    render_on_display(ssd, &frame_area);
                    ssd1306_draw_string_scaled(ssd, 0, 30, menus[current_menu][menu_index], 2);
                    render_on_display(ssd, &frame_area);

                    for (int i = 0; i < 2; i++) {
                        tone(200, 100);
                        sleep_ms(300);
                        tone(400, 100);
                        sleep_ms(300);
                        tone(100, 100);
                        sleep_ms(300);
                    }
                }
            }
        }


            if (selecting_option && sw_pressed && !last_sw_state) {
                printf("\nVoltando ao menu principal!\n");
                in_submenu = false;
                menu_index = 0;
                current_menu = 0;
                selecting_option = false;
                menu_updated = true;
                sleep_ms(500);
                memset(ssd, 0, ssd1306_buffer_length);
                render_on_display(ssd, &frame_area);
                ssd1306_draw_string_scaled(ssd, 0, 30, "COMUNICADOR", 2);
                ssd1306_draw_string_scaled(ssd, 20, 50, "TESTER", 2);
                render_on_display(ssd, &frame_area);
            }
        }

        update_leds();

        if (menu_updated) {
            print_menu();
            menu_updated = false;
        }

        last_vrx_value = vrx_value;
        last_vry_value = vry_value;
        last_sw_state = sw_pressed;

        sleep_ms(100);
    }

    return 0;
}
