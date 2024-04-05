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
	local LIB_NAME="${1}"     #$1: string - Nome da pasta da lib local
	local LIB_VERSION="${2}"  #$2: string - Versao da lib, arbitrário
	local VENDOR_DIR="${3}"   #$3: string - Nome da pasta de vendor local

	printf "\033[36m[LOGG]\033[m "
	echo "Configurando a biblioteca ${LIB_NAME}"

	local TARGET_PATH="${VENDOR_DIR}/${LIB_NAME}_${LIB_VERSION}"

	mkdir -vp "${TARGET_PATH}"
	bash -c "cd ${LIB_NAME} && ./configure --prefix=\$(pwd)/../${TARGET_PATH}"
}

#TODO: Seria melhor se a pasta `/vendor` tivese a sua própria variável
#TODO: Essa variável poderia ser alterada com uma opção `--vendor`
#TODO: O usuário deve ter a opção de skippar essa parte se ele quiser
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

	#TODO: Colocar os arquivos de source em constantes também
	#TODO: Colocar as flagas de bibliotecas em uma constante
	#TODO: Deixar o usuário usar flags customizadas se ele quiser
	#TODO: Por o nome do binário final em uma variável
	#TODO: O path  do binário pode ser customizado pelo usuário também
	eval "${COMPILER} playmp3.c main.c \
		-lao -lavcodec -lavformat -lswresample -lavutil \
		-o playmp3"
}

#TODO: A função de compilar estaticamente não deve funcionar com `--skip-conf`
#TODO: Criar a opção `--static` do jeito certo
[ "${1}" = "--static" ] || [ "${1}" = "-s" ] &&
	compile_statically ||
	compile_shared
