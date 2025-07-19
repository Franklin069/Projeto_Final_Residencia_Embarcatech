# Comunicador Interativo com Raspberry Pi Pico

## 📌 Descrição do Projeto

Este projeto tem como objetivo criar um **dispositivo de comunicação acessível** para pessoas com limitações de fala ou mobilidade. Utilizando o microcontrolador **Raspberry Pi Pico**, o sistema apresenta menus interativos em um **display OLED**, que podem ser navegados com um **joystick** e **botões físicos**. O dispositivo também fornece **feedback sonoro (buzzer)** e **luminoso (LEDs)**, além de uma funcionalidade de **pedido de socorro** com acionamento rápido.

## 🎯 Funcionalidades

- 📋 Navegação por menus e submenus usando o joystick.
- 🔘 Botão para exibir mensagem de emergência "SOCORRO" e emitir som de alerta.
- 📢 Controle de buzzer com sons programados.
- 💡 LEDs reagem às ações realizadas nos menus.
- 🎵 Áudio com controle de frequência e tempo (PWM e DMA).
- 🖥️ Feedback visual com mensagens claras no display OLED.
- 🔄 Lógica de navegação estruturada e responsiva.

## 🛠️ Tecnologias Utilizadas

- **Linguagem C**
- **SDK do Raspberry Pi Pico**
- **Display OLED SSD1306 (via I2C)**
- **GPIOs para controle de LEDs, botões e buzzer**
- **ADC para leitura do joystick**
- **PWM e DMA para som no buzzer**
- **FreeRTOS (opcional)**

## ⚙️ Estrutura do Código

O projeto é dividido nos seguintes módulos principais:

- `main.c`: Loop principal, inicialização de periféricos e lógica de navegação.
- `menu.c / menu.h`: Estrutura de menus e submenus.
- `buzzer.c / buzzer.h`: Controle de som e toques.
- `display.c / display.h`: Exibição de mensagens no OLED.
- `input.c / input.h`: Leitura do joystick e botões.
- `utils.c / utils.h`: Funções auxiliares, delays, etc.

## 📷 Diagramas e Fluxogramas

- Estrutura de Menus e Submenus  
  <img width="469" height="583" alt="image" src="https://github.com/user-attachments/assets/43a3257c-770d-4957-8a0c-1814688ff2e9" />


- Fluxo de Funções dos Botões  
 

- Pedido de Socorro e Ação do Buzzer  
 

## 🧪 Testes Realizados

- Teste de navegação por menus
- Verificação do funcionamento dos botões
- Teste de acionamento sonoro e luminoso
- Análise da resposta do display OLED
- Validação da lógica de emergência

## 👤 Autoria

- Nome: Franklin Delano Costa Araújo  
- Projeto acadêmico com foco em acessibilidade e interação homem-máquina.



