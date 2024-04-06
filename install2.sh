#!/bin/bash

# Path to vendored libraries
LIBAO_PATH="vendor/libao_1.2.2"
MPG123_PATH="vendor/mpg123_1.26.4"
FFMPEG_PATH="/usr/include/ffmpeg" # Update this path to the correct one for your system

# if [ ! -f $LIBAO_PATH/libao.a ]; then
#	echo "Static library libao.a not found in $LIBAO_PATH"
#	exit 1
#fi

#if [ ! -f $MPG123_PATH/libmpg123.a ]; then
#	echo "Static library libmpg123.a not found in $MPG123_PATH"
#	exit 1
#fi

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

# Add the FFMPEG_PATH to the include path
#$COMPILER -o playaudio main.c playmp3.c \
#	-I$LIBAO_PATH/include -I$MPG123_PATH/include -I$FFMPEG_PATH \
#	-L$LIBAO_PATH/lib -L$MPG123_PATH/lib \
#	-Wl,-Bstatic -l:libao.a -l:libmpg123.a \
#	-Wl,-Bdynamic -lavformat -lavcodec -lswresample -lavutil -lm -lpthread -lz -lssl -lcrypto

# Update the library paths and link against the shared libraries.

# Compile and link against the shared libraries.
$COMPILER -o playaudio main.c playmp3.c \
	-I$LIBAO_PATH/include -I$MPG123_PATH/include \
	-L$LIBAO_PATH/lib -L$MPG123_PATH/lib \
	-lao -lmpg123 \
	-lavformat -lavcodec -lswresample -lavutil -lm -lpthread -lz -lssl -lcrypto

if [ $? -eq 0 ]; then
	echo "Compilação concluída com sucesso!"
else
	echo "Erro durante a compilação."
	exit 1
fi
