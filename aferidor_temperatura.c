// João Henrique Viana de Oliveira - 15462907
// Pedro Brandi Pereira - 15640990

// Aferidor de temperatura

// Pinos do LCD
sbit LCD_RS at PORTB.B4;
sbit LCD_EN at PORTB.B5;
sbit LCD_D4 at PORTB.B0;
sbit LCD_D5 at PORTB.B1;
sbit LCD_D6 at PORTB.B2;
sbit LCD_D7 at PORTB.B3;

sbit LCD_RS_Direction at TRISB.B4;
sbit LCD_EN_Direction at TRISB.B5;
sbit LCD_D4_Direction at TRISB.B0;
sbit LCD_D5_Direction at TRISB.B1;
sbit LCD_D6_Direction at TRISB.B2;
sbit LCD_D7_Direction at TRISB.B3;


// Preload dos Timers

// Clock do sistema : 8 MHz
// Ciclo de instrução: 8 MHz / 4 = 2 MHz  (0,5 µs por ciclo)

// Timer0: modo 16 bits, prescaler 1:32
//   f_timer = 2 MHz / 32 = 62500 Hz
//   62500 contagens = 1 segundo
//   Preload  = 65536 - 62500 = 3036 = 0x0BDC

// Timer1: modo 16 bits, prescaler 1:8
//   f_timer = 2 MHz / 8 = 250000 Hz
//   62500 contagens = 250 ms
//   Preload  = 65536 - 62500 = 3036 = 0x0BDC

// Ambos compartilham o mesmo preload.
#define TIMERH_PRELOAD 0x0B
#define TIMERL_PRELOAD 0xDC

// Variáveis globais utilizadas no código final
signed char contagem_longa_duracao = 60;
signed char contagem_curta_duracao = 10;
signed char contagem_curta_250ms   =  4;

// Ponteiro para a contagem regressiva ativa (curta ou longa)
signed char *contagem = &contagem_curta_duracao;

// Flag que indica se o sistema está em operação
bit timer_ligado;

// Inicialização dos Timers
void init_timers() {
     // Timer0: 16 bits, clock interno (FOSC/4), prescaler 1:32, desligado
     T0CON = 0x04;
     TMR0H = TIMERH_PRELOAD;
     TMR0L = TIMERL_PRELOAD;

     // Timer1: clock interno (FOSC/4), prescaler 1:8, desligado
     T1CON = 0x30;
     TMR1H = TIMERH_PRELOAD;
     TMR1L = TIMERL_PRELOAD;

     // Habilita interrupções dos Timers 0 e 1
     INTCON.TMR0IE = 1;
     PIE1.TMR1IE = 1;

     INTCON.PEIE = 1;  // Habilita interrupções de periféricos
     INTCON.GIE = 1;  // Habilita interrupções globais
}


// Rotina de interrupção de ambos os timers
void interrupt() {
     // Timer0: base de tempo de 1 segundo para contagem
     if (INTCON.TMR0IF) {
        // Recarrega o preload e limpa a flag
        TMR0H = TIMERH_PRELOAD;
        TMR0L = TIMERL_PRELOAD;
        INTCON.TMR0IF = 0;

        // Decrementa e, ao chegar em zero, reinicia a contagem
        if (--contagem_longa_duracao <= 0) {
           contagem_longa_duracao = 60;
           // Força o encerramento quando a contagem termina
           timer_ligado = 0;
           T0CON.TMR0ON = 0;
        }
     }

     //  Timer1: base de tempo de 250 ms para contagem curta
     if (PIR1.TMR1IF) {
        // Recarrega o preload e limpa a flag
        TMR1H = TIMERH_PRELOAD;
        TMR1L = TIMERL_PRELOAD;
        PIR1.TMR1IF = 0;

        // A cada 4 interrupções, decrementa 1 segundo
        if (--contagem_curta_250ms <= 0) {
           contagem_curta_250ms = 4;
           if (--contagem_curta_duracao <= 0) {
              contagem_curta_duracao = 10;
              // Força o encerramento quando a contagem termina
              timer_ligado = 0;
              T1CON.TMR1ON = 0;
           }
        }
     }
}

// Código principal baseado no checkpoint 2
void main() {
     // Declaração de variáveis locais
     bit  botao0_pressionado; // Flag de acionamento do botão 0
     bit  botao1_pressionado; // Flag de acionamento do botão 1
     char temp_str[17]; // String da linha de temperatura
     char tempo_str[17]; // String da linha de contagem regressiva
     int  i;
     unsigned int adc_val; // Leitura bruta do ADC
     unsigned int temp_dec; // Temperatura dos décimos de °C

     // Inicialização das flags
     timer_ligado = 0;
     botao0_pressionado = 0;
     botao1_pressionado = 0;


     // Montagem da string da linha 1 do LCD
     temp_str[0] = 'T';
     temp_str[1] = 'e';
     temp_str[2] = 'm';
     temp_str[3] = 'p';
     temp_str[4] = ':';
     temp_str[5] = ' ';
     temp_str[6] = '0';
     temp_str[7] = '0';
     temp_str[8] = ',';
     temp_str[9] = '0';
     temp_str[10] = ' ';
     temp_str[11] = 'C';
     for (i = 12; i < 16; i++) {
         temp_str[i] = ' ';
     }
     temp_str[16] = '\0';

     // Montagem da string da linha 2 do LCD
     tempo_str[0] = 'T';
     tempo_str[1] = 'e';
     tempo_str[2] = 'm';
     tempo_str[3] = 'p';
     tempo_str[4] = 'o';
     tempo_str[5] = ':';
     for (i = 6; i < 16; i++) {
         tempo_str[i] = ' ';
     }
     tempo_str[15] = 'S'; // Inicia em contagem rápida
     tempo_str[16] = '\0';

     // Inicialização dos Timers
     init_timers();

     // Inicialização do ADC
     ADCON0 = 0b00000001;

     // Configuração do clock e tempo de aquisição dos valores
     ADCON2 = 0b10111110;

     // Configuração da referência e portas analógicas
     ADCON1 = 0b00111011;

     // Configuração dos pinos de I/O
     TRISA.F0 = 1; // Entrada do potenciômetro
     TRISA.F2 = 1; // Vref- (GND)
     TRISA.F3 = 1; // Vref+
     TRISD.F0 = 1; // Botão 0 (inicia/para medição)
     TRISD.F1 = 1; // Botão 1 (alterna modo curto/longo)
     TRISD.F2 = 0; // Saída - LED (acende para temperatura > 50°C)
     PORTD.F2 = 0; // LED inicialmente apagado

     // Inicialização do LCD
     Lcd_Init();
     Lcd_Cmd(_LCD_CLEAR);
     Lcd_Cmd(_LCD_CURSOR_OFF);
     Lcd_Out(1, 1, temp_str);
     Lcd_Out(2, 1, tempo_str);

     // Loop principal
     while (1) {
           // Inicia ou para a contagem e medição de temperatura
           if (!PORTD.F0 && !botao0_pressionado) {
              botao0_pressionado = 1;
              Delay_ms(20); // Debouncing

              // Alterna o estado do sistema entre ligado e desligado
              timer_ligado = !timer_ligado;

              // Liga ou desliga o timer correspondente ao modo atual
              if (contagem == &contagem_curta_duracao) {
                 T1CON.TMR1ON = timer_ligado; // Timer1 para modo curto
              } else {
                 T0CON.TMR0ON = timer_ligado; // Timer0 para modo longo
              }
           } else if (PORTD.F0) {
             botao0_pressionado = 0;
           }

           // Alterna entre temporizador de curta ou longa duração
           if (!PORTD.F1 && !botao1_pressionado) {
              botao1_pressionado = 1;
              Delay_ms(20); // Debouncing

              if (contagem == &contagem_curta_duracao) {
                 // Longa duração (Timer0)
                 contagem = &contagem_longa_duracao;
                 contagem_curta_duracao = 10;
                 if (timer_ligado) {
                    T0CON.TMR0ON = 1;
                    T1CON.TMR1ON = 0;
                 }
                 tempo_str[15] = 'L';
              } else {
                // Curta duração (Timer1)
                contagem = &contagem_curta_duracao;
                contagem_longa_duracao = 60;
                if (timer_ligado) {
                   T0CON.TMR0ON = 0;
                   T1CON.TMR1ON = 1;
                }
                tempo_str[15] = 'S';
              }
           } else if (PORTD.F1) {
             botao1_pressionado = 0;
           }

           // Leitura da
           if (timer_ligado) {
              // Inicialização da conversão ADC no canal AN0
              ADCON0.F1 = 1;
              while (ADCON0.F1); // Aguarda fim da conversão

              adc_val = (((unsigned int)(ADRESH & 0x03)) << 8) | ((unsigned int)ADRESL);

              // Conversão para temperatura em décimos de graus celsius
              temp_dec = (unsigned int)(((unsigned long)adc_val * 125UL) / 128UL);

              // LED acende quando temperatura for maior que 50 °C
              PORTD.F2 = (temp_dec > 500) ? 1 : 0;

              // Formatação do resultado
              temp_str[9] = (char)(temp_dec % 10) + '0';
              temp_dec /= 10;
              temp_str[7] = (char)(temp_dec % 10) + '0';
              temp_dec /= 10;
              temp_str[6] = (char)(temp_dec % 10) + '0';

              Lcd_Out(1, 1, temp_str); // Atualiza linha 1 do LCD
           }


           // Atualiza a contagem regressiva no display (linha 2)
           tempo_str[8] = (char)(*contagem % 10) + '0';

           if (*contagem >= 10) {
              tempo_str[7] = (char)(*contagem / 10) + '0';
           } else {
              tempo_str[7] = ' ';
           }
           Lcd_Out(2, 1, tempo_str);
     }
}
