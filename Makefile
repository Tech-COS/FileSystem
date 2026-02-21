SRC_PATH = src
BUILD_PATH = build
COMPILER = gcc
FILE_TYPE = .c

SRC_MAIN =\
	${SRC_PATH}/main.c\
	${SRC_PATH}/mkfs_functions.c\

SRC =\
	${SRC_PATH}/blocks_functions.c\
	${SRC_PATH}/read_write.c\

SRC_LIB =\
	${SRC_PATH}/filesystem_input_output.c\
	${SRC_PATH}/ext4_tree_functions.c\
	${SRC_PATH}/stack_protection.c\
	${SRC_PATH}/wrappers.c\
	${SRC_PATH}/syscalls/write_read.c\
	${SRC_PATH}/syscalls/detect_drive.c\
	${SRC_PATH}/cd_command.c\
	${SRC_PATH}/env.c\

INCLUDE =\
	-I./include/\
	-I../include/\

OBJ = ${SRC:${SRC_PATH}/%${FILE_TYPE}=${BUILD_PATH}/%.o}
OBJ_MAIN = ${SRC_MAIN:${SRC_PATH}/%${FILE_TYPE}=${BUILD_PATH}/%.o}
OBJ_LIB = ${SRC_LIB:${SRC_PATH}/%${FILE_TYPE}=${BUILD_PATH}/%.o}
OBJ_FLAGS = -W -Wall -Wextra -Werror ${INCLUDE} -m64 -mcmodel=large -mlarge-data-threshold=2147483647 -ffreestanding -mno-red-zone -nostdlib -g3 -Wall -Wextra -z noexecstack -z max-page-size=0x1000 -fPIC -pedantic -pedantic-errors
BIN_FLAGS =
BIN_NAME = cos_mkfs
BOOT_COMPILER = nasm
LIB_NAME = libfilesystem.a

all: ${BIN_NAME}
lib: ${LIB_NAME}

debug: OBJ_FLAGS += -DDEBUG -g3

# Syscall
SRC_SYSCALL_DIR = ./src/syscalls
SRC_SYSCALL =\

BUILD_SYSCALL_PATH = ${BUILD_PATH}/syscalls
OBJ_SYSCALL = ${SRC_SYSCALL:${SRC_SYSCALL_DIR}/%.asm=${BUILD_SYSCALL_PATH}/%.o}
SYSCALL_FLAGS = -f elf64

${BUILD_SYSCALL_PATH}/%.o: ${SRC_SYSCALL_DIR}/%.asm
	mkdir -p ${dir $@}
	${BOOT_COMPILER} ${SYSCALL_FLAGS} $< -o $@

syscall: ${OBJ_SYSCALL}

${BUILD_PATH}/%.o: ${SRC_PATH}/%${FILE_TYPE}
	mkdir -p ${dir $@}
	${COMPILER} -MD ${OBJ_FLAGS} -c $< -o $@

${BIN_NAME}: ${OBJ} ${OBJ_MAIN}
	${COMPILER} -o ${BIN_NAME} ${OBJ} ${OBJ_MAIN} ${BIN_FLAGS}

${LIB_NAME}: ${OBJ_SYSCALL} ${OBJ_LIB} ${OBJ}
	ar rcs ${LIB_NAME} ${OBJ_SYSCALL} ${OBJ_LIB} ${OBJ}

clean:
	rm -rf ${BUILD_PATH}

fclean: clean
	rm -rf ${LIB_NAME}
	rm -rf ${BIN_NAME}

-include ${OBJ:%.o=%.d}

.PHONY: all clean fclean
