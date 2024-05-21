#!/bin/bash

# Função para compilar o projeto
function compile() {
  CC="clang"  # ou "gcc" se preferir
  CFLAGS="-lm -O3"
  SRC_FILES=("src/main.c" "src/duckwave.c")
  OUTPUT="duckwave"
  BUILD_DIR="build"
  
  # Criar diretório de build se não existir
  mkdir -p "$BUILD_DIR"
  
  # Compilar o projeto
  $CC "$CFLAGS" -I"include" -o "$BUILD_DIR/$OUTPUT" "${SRC_FILES[@]}"
  
  if [ $? -ne 0 ]; then
    echo "Erro na compilação do projeto"
    exit 1
  fi
  
  echo "Projeto compilado com sucesso"
}

# Função para instalar no macOS
function install_macos() {
  sudo mv build/duckwave /usr/local/bin/duckwave
  sudo ln -sf /usr/local/bin/duckwave /usr/local/bin/duckwave
}

# Função para instalar em outros sistemas
function install_others() {
  sudo mv build/duckwave /usr/bin/duckwave
  sudo ln -sf /usr/bin/duckwave /usr/bin/duckwave
}

# Função principal para compilar e instalar
function start() {
  compile
  if [ "$(uname)" == "Darwin" ]; then
    install_macos
  else
    install_others
  fi
}

# Verificar se o script está sendo executado como sudo
if [ "$EUID" -eq 0 ]; then
  start
else
  echo "Este programa deve ser executado como sudo"
  exit 1
fi
