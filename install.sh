#!/bin/bash

# Verifica se clang está disponível
if command -v clang &>/dev/null; then
	COMPILER="clang"
# Caso contrário, verifica se gcc está disponível
elif command -v gcc &>/dev/null; then
	COMPILER="gcc"
else
	echo "Nenhum compilador (clang ou gcc) encontrado no sistema."
	exit 1
fi

echo "Compilando com $COMPILER..."
$COMPILER -o playaudio main.c playmp3.c -lavformat -lavcodec -lswresample -lao -lavutil -Bstatic
#$COMPILER -o playaudio main.c playmp3.c -Ilibao/include -Llibao/lib -Wl,-Bstatic -lao -Wl,-Bdynamic -lavformat -lavcodec -lswresample -lavutil -lm -lpthread -lz -lssl -lcrypto
if [ $? -eq 0 ]; then
	echo "Compilação concluída com sucesso!"
else
	echo "Erro durante a compilação."
	exit 1
fi
