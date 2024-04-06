#!/bin/bash

# RODAR COM SUDO O SCRIPT

if clang -v &>/dev/null; then
	COMPILER=$(which clang)
elif gcc -v &>/dev/null; then
	COMPILER=$(which gcc)
else
	echo
	printf "\033[31m[ERROR]\033[m "
	echo "Compilers 'clang' and 'gcc' were not found."
	echo
	exit 1
fi

#function vendorize_local_library {
#	local LIB_NAME="${1}"
#	local LIB_VERSION="${2}"
#	local VENDOR_DIR="${3}"

#	printf "\033[36m[LOG]\033[m "
#	echo "Configuring library ${LIB_NAME}"

#	local TARGET_PATH="${VENDOR_DIR}/${LIB_NAME}_${LIB_VERSION}"

#	mkdir -vp "${TARGET_PATH}"
#	bash -c "cd ${LIB_NAME} && ./configure --prefix=\$(pwd)/../${TARGET_PATH} && make && make install"
#}

function vendorize_local_library {
	local LIB_NAME="${1}"
	local LIB_VERSION="${2}"
	local VENDOR_DIR="${3}"

	printf "\033[36m[LOG]\033[m "
	echo "Configuring library ${LIB_NAME}"

	local TARGET_PATH="${VENDOR_DIR}/${LIB_NAME}_${LIB_VERSION}"

	mkdir -vp "${TARGET_PATH}"
	bash -c "cd ${LIB_NAME} && ./configure --prefix=\$(pwd)/../${TARGET_PATH} && make clean && make && make install"
}

VENDOR_DIR="vendor"

vendorize_local_library "libao" "1.2.2" "${VENDOR_DIR}"
vendorize_local_library "mpg123" "1.26.4" "${VENDOR_DIR}"

function compile_statically {
	echo
	printf "\033[36m[LOG]\033[m "
	echo "Compiling the application with static libraries"
	echo

	eval "${COMPILER} playmp3.c main.c \
        -I${VENDOR_DIR}/libao_1.2.2/include -I${VENDOR_DIR}/mpg123_1.26.4/include \
        -L${VENDOR_DIR}/libao_1.2.2/lib -L${VENDOR_DIR}/mpg123_1.26.4/lib \
        -lao -lmpg123 -lavcodec -lavformat -lswresample -lavutil -lm -lpthread -lz -lssl -lcrypto \
        -o playmp3"
}

function compile_shared {
	echo
	printf "\033[36m[LOG]\033[m "
	echo "Compiling the application with system libraries"
	echo

	eval "${COMPILER} playmp3.c main.c \
        -lao -lavcodec -lavformat -lswresample -lavutil \
        -o playmp3"
}

if [ "${1}" = "--static" ] || [ "${1}" = "-s" ]; then
	compile_statically
else
	compile_shared
fi


# sudo docker run --rm -it \
#     --device /dev/snd \
#     -e PULSE_SERVER=unix:${XDG_RUNTIME_DIR}/pulse/native \
#     -v ${XDG_RUNTIME_DIR}/pulse/native:${XDG_RUNTIME_DIR}/pulse/native \
#     -v ~/.config/pulse/cookie:/root/.config/pulse/cookie \
#     --group-add $(getent group audio | cut -d: -f3) \
#     teste /bin/bash
#cd /usr/src/app/CSoundWave
#./install.sh --static
#./playmp3 /usr/src/app/CSoundWave/audios/1.mp3

