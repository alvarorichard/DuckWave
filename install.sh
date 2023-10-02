#!/bin/bash

# Verifica se clang está disponível
if command -v clang &> /dev/null; then
    COMPILER="clang"
# Caso contrário, verifica se gcc está disponível
elif command -v gcc &> /dev/null; then
    COMPILER="gcc"
else
    echo "Nenhum compilador (clang ou gcc) encontrado no sistema."
    exit 1
fi

echo "Compilando com $COMPILER..."
$COMPILER -o mp3player main.c playmp3.c -lmpg123 -lao

if [ $? -eq 0 ]; then
    echo "Compilação concluída com sucesso!"
else
    echo "Erro durante a compilação."
    exit 1
fi
