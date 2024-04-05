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

# TODO: Ver se é realmente necessário rodar o make

echo "Configurando as bibliotecas locais"

mkdir -p vendor/{libao_1.2.2,mpg123_1.26.4}

cd libao
./configure --prefix=$(pwd)/../vendor/libao_1.2.2
# make
cd ..

cd mpg123
./configure --prefix=$(pwd)/../vendor/mpg123_1.26.4
# make
cd ..

echo "Compilando com $COMPILER..."

# TODO: Incluir as bibliotecas da mpg123 localmente também
# TODO: Fazer o processo de compilação linkar as bibliotecas estaticamente

$COMPILER clang playmp3.c main.c \
	-Wl,-Bstatic \
	-I./vendor/libao_1.2.2/include -L./vendor/libao_1.2.2/.libs -lao \
	-lavcodec -lavformat -lswresample -lavutil \
	-o playmp3

#$COMPILER -o playaudio main.c playmp3.c -Ilibao/include -Llibao/lib -Wl,-Bstatic -lao -Wl,-Bdynamic -lavformat -lavcodec -lswresample -lavutil -lm -lpthread -lz -lssl -lcrypto
if [ $? -eq 0 ]; then
	echo "Compilação concluída com sucesso!"
else
	echo "Erro durante a compilação."
	exit 1
fi
