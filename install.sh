#!/bin/bash

if clang -v &> /dev/null
then
	COMPILER=$(which clang)

elif gcc -v &> /dev/null
then
	COMPILER=$(which clang)

else
	echo
	printf "\033[31m[ERRO]\033[m "
	echo "Os compiladores 'clang' e 'gcc' não foram encontrados"
	echo

	exit 1
fi

## Essa função vai rodar o script `./configure` que a biblioteca (`$1`) deve
## ter em sua raís, o prefixo dessa configuração será alguma pasata no
## diretório local ($3).
function vendorize_local_library {
	local LIB_NAME="${1}"     #1: string
	local LIB_VERSION="${2}"  #2: string
	local VENDOR_DIR="${3}"   #3: string

	printf "\033[36m[LOGG]\033[m "
	echo "Configurando a biblioteca ${LIB_NAME}"

	local TARGET_PATH="${VENDOR_DIR}/${LIB_NAME}_${LIB_VERSION}"

	mkdir -vp "${TARGET_PATH}"
	bash -c "cd ${LIB_NAME} && ./configure --prefix=\$(pwd)/../${TARGET_PATH}"
}

vendorize_local_library "libao"  "1.2.2"  "vendor"
vendorize_local_library "mpg123" "1.26.4" "vendor"

function compile_statically {
	echo
	printf "\033[33m[WARN]\033[m "
	echo "Esse script não consegue compilar estaticamente ainda!"
	echo
}

function compile_shared {
	echo
	printf "\033[36m[LOGG]\033[m "
	echo "Compilando a aplicação com as bibliotecas do sistema"
	echo

	eval "${COMPILER} playmp3.c main.c \
		-lao -lavcodec -lavformat -lswresample -lavutil \
		-o playmp3"
}

[ "${1}" = "--static" ] || [ "${1}" = "-s" ] &&
	compile_statically ||
	compile_shared
