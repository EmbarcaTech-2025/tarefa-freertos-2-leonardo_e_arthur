
# Tarefa: Roteiro de FreeRTOS #2 - EmbarcaTech 2025

Autores: **Leonardo Takehana** e **Arthur Damacena**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, junho de 2025

---


# Estimativa de Estado de Carga (SoC) de Bateria LFP com EKF no Raspberry Pi Pico W

Este projeto implementa um estimador de Estado de Carga (SoC) para baterias LiFePO₄ (LFP), utilizando Filtro de Kalman Estendido (EKF), embarcado em um **Raspberry Pi Pico W** com a plataforma **BitDogLab**. A estimativa é exibida em tempo real em um display OLED.

## Visão Geral

O SoC (State of Charge) é uma métrica fundamental para o gerenciamento eficiente de baterias. Neste projeto, a estimativa é baseada em um modelo matemático da célula LFP, obtido por meio de testes experimentais. As medições de corrente, tensão e temperatura são utilizadas como entradas para o EKF.

## Plataforma

- **Microcontrolador**: Raspberry Pi Pico W (RP2040)
- **Base**: BitDogLab
- **Sensores**:
  - Corrente: ACS712 (30A)
  - Temperatura: Termistor NTC (com divisor resistivo)
  - Tensão: Divisor resistivo adequado ao ADC do Pico
- **Display**: OLED (ex: SSD1306 via I2C)
- **Software**:
  - SDK oficial do Raspberry Pi Pico (C/C++)
  - Biblioteca customizada de EKF
  - Biblioteca gráfica para SSD1306
  - FreeRTOS (opcional, para tarefas paralelas)

![Sistema completo de estimação de SoC](images/sistema%20completo.jpg)


## Funcionamento

O sistema utiliza como base um modelo matemático equivalente da célula LFP, construído a partir de testes experimentais. Esse modelo consiste em um ramo RC em série com uma resistência R₀, representando a dinâmica da célula.

A corrente medida é usada como entrada para o sistema, enquanto a tensão e a temperatura são utilizadas como observações no EKF. Um método auxiliar de Coulomb Counting também é implementado para alimentar o modelo com a estimativa de SoC baseada em corrente integrada, para que o modelo funcione corretamente, é necessário que ele seja executado em realtime, a uma amostragem de 1 segundo.


### Modelo RC Simplificado

![Modelo Bateria](images/ecm.png)

A tensão terminal da célula é modelada como:

```math
V_t = OCV(SoC) - I * R₀ - V_RC
```

Onde:
- `OCV(SoC)` é a curva de tensão em circuito aberto obtida experimentalmente
- `V_RC` é a queda de tensão no ramo RC

## Visualização OLED

O display exibe em tempo real:
- SoC estimado (via EKF)
- Tensão da bateria
- Corrente instantânea
- Temperatura

![Visualização dos dados da bateria em tempo real](images/visualização%20dos%20dados.jpg)

## Estrutura do Projeto

```bash
.
├── include/
│   ├── adc_read.c/.h           # Leitura de sensores analógicos
│   ├── battery_model.c/.h      # Implementação do modelo da célula
│   ├── ekf.c/.h                # Estimador de Kalman Estendido
│   ├── ssd1306.c/.h            # Interface gráfica com display OLED
│   ├── font.h                  # Fonte bitmap para o display
│   ├── FreeRTOSConfig.h        # (opcional) Configuração do FreeRTOS
├── main.c                      # Código principal
├── CMakeLists.txt
├── pico_sdk_import.cmake
├── .gitignore
└── README.md
```

## Compilação e Upload

1.  **Clonar o Repositório:**
    *   Abra um terminal ou prompt de comando.
    *   Clone o repositório utilizando o comando:
        ```bash
        git clone --recurse-submodules https://github.com/EmbarcaTech-2025/tarefa-freertos-2-leonardo_e_arthur.git
        ```
    *   A flag `--recurse-submodules` garante que o FreeRTOS (que é um submódulo) seja clonado junto.
    *   Se você já clonou o repositório sem `--recurse-submodules`, navegue até a pasta do projeto e execute:
        ```bash
        git submodule update --init --recursive
        ```

### Compilação

```bash
mkdir build && cd build
cmake ..
make
```

### Upload para o Pico W

Coloque o Pico W em modo bootloader e arraste o arquivo `.uf2` gerado.

## Referências

- [RP2040 Datasheet](https://www.raspberrypi.com/documentation/microcontrollers/)
- PLETT, Gregory L. Battery Management Systems, Volume I: Battery Modeling. Artech House, 2015.
- Dados experimentais fornecidos por colaboradores do projeto BMS

## Agradecimentos

Este projeto agradece ao **Projeto BMS**, à **FUNDEP**, ao **Prof. Renato Vilela Lopes** e ao **mestrando Camilo Solis** pela disponibilização do modelo experimental da célula LFP, que foi essencial para o desenvolvimento e validação do estimador EKF.

---

## 📜 Licença
GNU GPL-3.0.
