# SEL0433 - PIC18F4550 - Projeto Final
### Integrantes: 
* João Henrique Viana de Oliveira - 15462907
* Pedro Brandi Pereira - 15640990

Este projeto consiste em um dispositivo de aferição de temperatura e tempo para um forno industrial. O sistema foi programado em linguagem C para o microcontrolador PIC18F4550 (arquitetura RISC da família Microchip) e validado por meio do simulador SimulIDE, tendo como base o pinout do Kit EasyPIC v7.

O objetivo do aferidor é medir a temperatura interna do forno durante um intervalo de tempo predeterminado pelo usuário, utilizando o módulo de conversão analógico-digital para ler um sensor LM35 (emulado neste projeto por um potenciômetro). O equipamento exibe a temperatura captada, dimensionada para a faixa de 0°C a 100°C, juntamente com a contagem regressiva em um display LCD operando de forma contínua. Além disso, o circuito possui botões com tratamento de *bouncing* para a configuração do sistema e um LED dedicado que sinaliza quando a temperatura ultrapassa os 50°C.

A descrição técnica de cada decisão de implementação, o detalhamento da configuração dos registradores, dos *Timers* e a inicialização do módulo ADC estão atribuídos diretamente no arquivo C presente neste repositório em formato de comentário.

## Funcionalidades Apresentadas

* Leitura e conversão de temperatura: O sistema realiza a leitura analógica do sensor (potenciômetro) e formata a exibição no display LCD apresentando três algarismos significativos (formato "XX,X C") na faixa de 0°C a 100°C.

* Controle da duração da contagem regressiva: O tempo de monitoramento pode ser alternado por meio de um botão, permitindo ao usuário selecionar entre um modo de aferição de curta duração (10 segundos) ou longa duração (60 segundos).

* Acionamento interativo: O início e a pausa da medição de temperatura e da contagem do tempo são controlados por um segundo botão independente.

* Feedback visual de aquecimento: O programa acende um LED no painel sempre que a temperatura exceder 50°C.

* Interface de monitoramento: O display LCD apresenta os dados do forno em tempo real, informando o modo de contagem ativo ("S" para curta e "L" para longa) ao lado do tempo restante.

## Arquitetura Implementada

* Implementação de temporizadores de 16 bits para as bases de tempo, utilizando o Timer0 configurado para gerar interrupções da ordem de 1 segundo (modo longo) e o Timer1 para gerar interrupções a cada 250 milissegundos (modo curto).

* Configuração do conversor analógico-digital (ADC) com resolução de 10 bits no registrador ADCON1, parametrizando uma alimentação de 1V como tensão externa de referência no pino A3 para otimizar a sensibilidade de leitura do sensor LM35.

* Tratamento de *bouncing* solucionado puramente via software, manipulando *flags* de estado e atrasos após as detecções de borda nos botões de controle.

* Inicialização da interface gráfica através das bibliotecas nativas do MikroC, operando a escrita de dados para o display LCD no modo serial de 4 bits exigido pelo esquemático.

* Formatação do cálculo e impressão dos valores decimais de temperatura utilizando lógicas de divisão com inteiros e resto de divisão, evitado completamente a declaração de variáveis do tipo *float* visando otimização da memória de dados do microcontrolador.

## Imagens de simulação e compilação
<img width="642" height="480" alt="Image" src="https://github.com/user-attachments/assets/c5927709-ec49-4a36-b8b9-3066e4a82317" />

Simulação - Contagem rápida e LED acionado

<img width="619" height="486" alt="Image" src="https://github.com/user-attachments/assets/8e1fdc5e-9af7-41f5-81f8-f372f02001df" />

Simulação - Contagem lenta e LED desligado

<img width="1341" height="209" alt="Image" src="https://github.com/user-attachments/assets/cc3ad9f6-a452-42ed-ad1b-71fcb21923fd" />

Arquivo de Compilação - Projeto Final

## Resultados obtidos

O resultado obtido foi um aferidor de temperatura dependente da variação de um potenciômetro em ambiente de simulação. O dispositivo exibe seus resultados apenas durante o intervalo de contagem selecionado previamente pelo usuário. Ao final do tempo de contagem, a averiguação da temperatura é pausada, e só é permitida uma nova aferição com um novo início do cronômetro por meio dos botões responsáveis.

Tais resultados são satisfatórios e cumprem todos os objetivos esperados da prática.



