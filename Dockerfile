# Use uma imagem base com suporte para as ferramentas necessárias
FROM ubuntu:noble-20240904.1

# Evita prompts interativos durante a instalação de pacotes
ARG DEBIAN_FRONTEND=noninteractive

# Instala as ferramentas e bibliotecas necessárias
RUN apt update && apt install -y \
    bash \
    clang \
    gcc \
    git \
    make \
    sudo \
    yt-dlp \
    pkg-config \
    libao-dev \
    libmpg123-dev \
    libavcodec-dev \
    libavformat-dev \
    libswresample-dev \
    libavutil-dev \
    zlib1g-dev \
    libssl-dev \
    libmpg123-0 \
    && rm -rf /var/lib/apt/lists/*

# Cria um diretório para o código fonte
WORKDIR /usr/src/app

# Clona o repositório Git na branch dev
RUN git clone -b dev https://github.com/alvarorichard/CSoundWave.git


# Torna o script executável
RUN chmod +x /usr/src/app/CSoundWave/install.sh


# Define o comando padrão para executar o script com sudo (como root)
CMD ["sudo", "./CSoundWave/install.sh", "--static"]
