#!/bin/bash

if clang -v &> /dev/null
then
	COMPILER=$(which clang)

elif gcc -v &> /dev/null
then
	COMPILER=$(which clang)

else
	printf "\033[31m[ERRO]\033[m "
	printf "Os compiladores 'clang' e 'gcc' não foram encontrados\n"

	exit 1
fi

function vendorize_local_library {
	local LIB_NAME="${1}"
	local LIB_VERSION="${2}"
	local VENDOR_DIR="${3}"

	printf "\033[36m[LOGG]\033[m "
	echo "Configurando a biblioteca ${LIB_NAME}"

	local TARGET_PATH="${VENDOR_DIR}/${LIB_NAME}_${LIB_VERSION}"

	mkdir -vp "${TARGET_PATH}"
	bash -c "cd ${LIB_NAME} && ./configure --prefix=\$(pwd)/../${TARGET_PATH}"
}

vendorize_local_library "libao"  "1.2.2"  "vendor"
vendorize_local_library "mpg123" "1.26.4" "vendor"

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
