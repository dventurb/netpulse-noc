<div align="center">
    <img alt="NetPulse NOC logo" src="https://i.imgur.com/CymuZYC.png" height="128">
  <h1>NetPulse NOC - Network Operations Center</h1>
</div>

<p align="center">
  <a href="#about">About</a> • <a href="#build">Build</a> • <a href="#architecture">Architecture</a> • <a href="#roadmap">Roadmap</a>
</p>

![Preview of NetPulse NOC][preview_image]

[//]: # (LINKS)
[license]: LICENSE
[preview_image]: https://github.com/dventurb/netpulse-noc/blob/main/assets/preview.png "Preview of NetPulse NOC"

## About 

O **NetPulse NOC** foi desenvolvido em contexto académico para aprofundar e colocar em prática metodologias de desenvolvimento de software, estruturas de dados dinâmicas e algoritmos de ordenação e pesquisa. O projeto permite a autenticação de técnicos, operações de gestão de equipamentos, incidentes, sensores e verificação de testes de conectividade.

### Features 

* **Interface Gráfica:** Desenvolvida em C com recurso à lib GTK 4 e personalização via CSS.
* **Segurança:** Autenticação e Hashing de passwords utilizando salt, evitando assim ataques de dicionário e utilizando **Argon2id** recorrendo à biblioteca `libsodium`.
* **Threads:** Arquitetura assíncrona suportada por Threads (`pthreads`) para operações de dados pesadas (heavy work), evitando assim o bloqueio da UI.
* **Memory Ownership:** Gestão de memória relacionando o ciclo de vida dos widgets (`GtkWindow`) do GTK 4 com as alocações dinâmicas realizadas ao longo da execução.

## Build

### Linux 
```bash 
git clone https://github.com/dventurb/netpulse-noc.git
cd netpulse-noc
chmod +x install.sh
./install.sh
./netpulse-noc
```

### macOS
```bash 
git clone https://github.com/dventurb/netpulse-noc.git
cd netpulse-noc
chmod +x install.sh
./install.sh
./netpulse-noc
```

>**Note:** Requires Homebrew installed([https://brew.sh](https://brew.sh))


### Windows (via MSYS2)
Em Windows é recomendado utilizar o ambiente **MSYS2**, onde fornece o GCC, POSIX (`pthreads`), pacotes do GTK4 e outras ferramentas necessárias.

1. Transferir e instalar o **MSYS2**:
[https:www.msys2.org/](https:www.msys2.org/)

2. Abrir o terminal **MSYS2 UCRT64**:
```bash 
git clone https://github.com/dventurb/netpulse-noc.git
cd netpulse-noc/
chmod +x install.sh
./install.sh
./netpulse-noc
```

### Dependencies
All dependencies are automatically installed by the install script (install.sh).
- **GTK 4** (`libgtk-4-dev`) 
- **libsodium** (`libsodium-dev`)
- **Build tools**: `gcc`, `make`, `pkg-config`

## Architecture

O projeto adota uma variação do **MVC (Model-View-Controller)**, dividindo por domínios (ex: `equipment`, `incident`, `sensor`). Cada módulo contém o seu próprio modelo de dados, a apresentação visual, controllers para decisão do fluxo, e workers.

### Inicialização (main.c)

A persistência é carregada antes da execução da interface gráfica, garantindo que os dados internos estão prontos para consumo, ocorrendo ainda o `sodium_init()` para a utilização de operações criptográficas na autenticação dos técnicos.


## Roadmap 

Evoluções planeadas para desenvolvimento futuro:

* **Refactor da Arquitetura:** Migração do atual modelo MVC + Worker para um baseado num `app_worker` único, funcionando, controlando e executando as operações necessárias por thread, com services independentes.
* **Estatísticas Visuais:** Integração de componentes gráficos utilizando a biblioteca `GtkChart` e recorrendo ao Cairo para a apresentação de dashboards com métricas.
