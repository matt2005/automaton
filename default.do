#!/bin/bash
# automaton
# (c) 2013 Justin Gottula
# The source code of this project is distributed under the terms of the
# simplified BSD license. See the LICENSE file for details.


exec >&2


# redo appears to be broken
set -e


TARGET=$1
TARGET_BASE=$2
OUTPUT=$3


CC="avr-gcc"
AS="avr-as"
LD="avr-ld"
NM="avr-nm"
OBJCOPY="avr-objcopy"
OBJDUMP="avr-objdump"


PROJ_NAME="automaton"


MCU="atmega324a"
CPU_KHZ="20000"

DEFINES=("F_CPU=${CPU_KHZ}000UL")
INC_DIRS=(src)
LIB_DIRS=()
LIBS=()

CFLAGS_CPU=("-mmcu=$MCU" -fpack-struct -funsigned-bitfields)
CFLAGS_DEBUG=(-ggdb)
CFLAGS_OPT=(-Os -flto -fuse-linker-plugin -fwhole-program -mcall-prologues \
	-mrelax -mstrict-X -fmerge-all-constants -ffast-math -fno-jump-tables \
	-fshort-enums)
CFLAGS_WARN=(-Wall -Wextra -Waddr-space-convert -Wno-unused-function)
CFLAGS_ETC=(-fno-diagnostics-show-caret -pipe)

CFLAGS="-std=gnu11 ${CFLAGS_CPU[@]} ${CFLAGS_DEBUG[@]} ${CFLAGS_OPT[@]} \
${CFLAGS_WARN[@]} ${CFLAGS_ETC[@]}"
CFLAGS_COMPILE="$CFLAGS ${DEFINES[@]/#/-D} ${INC_DIRS[@]/#/-I}"
CFLAGS_LINK="$CFLAGS ${LIB_DIRS[@]/#/-L} ${LIBS[@]/#/-l}"


SOURCES=$(find src -type f -iname "*.c")
OBJECTS=${SOURCES//.c/.o}

OUT_ELF="out/$PROJ_NAME.elf"
OUT_BIN="out/$PROJ_NAME.bin"
OUT_HEX="out/$PROJ_NAME.hex"
OUT_MAP="out/$PROJ_NAME.map"
OUT_DUMP="out/$PROJ_NAME.dump"
OUT_LST="out/$PROJ_NAME.lst"


case "$TARGET" in
all)
	redo-ifchange $OUT_ELF $OUT_BIN $OUT_HEX $OUT_MAP $OUT_DUMP $OUT_LST
	;;
$OUT_ELF)
	redo-ifchange $OBJECTS
	$CC $CFLAGS_LINK -o $OUTPUT $OBJECTS
	;;
$OUT_BIN)
	redo-ifchange $OUT_ELF
	$OBJCOPY -O binary $OUT_ELF $OUTPUT
	stat --printf='%n: %s bytes\n' $OUTPUT
	;;
$OUT_HEX)
	redo-ifchange $OUT_ELF
	$OBJCOPY -O ihex $OUT_ELF $OUTPUT
	;;
$OUT_MAP)
	redo-ifchange $OUT_ELF
	$NM -Cn $OUT_ELF >$OUTPUT
	;;
$OUT_DUMP)
	redo-ifchange $OUT_ELF
	$OBJDUMP -aCdfhp $OUT_ELF >$OUTPUT
	;;
$OUT_LST)
	redo-ifchange $OUT_ELF
	$OBJDUMP -CdS -j '.text' $OUT_ELF >$OUTPUT
	;;
*.o)
	$CC $CFLAGS_COMPILE -o${TARGET//.o/.dep} -MM -MG ${TARGET//.o/.c}
	
	read DEPS <${TARGET//.o/.dep}
	redo-ifchange ${DEPS#*:}
	
	$CC $CFLAGS_COMPILE -o$OUTPUT -c ${TARGET//.o/.c}
	;;
clean)
	rm -rf $(find out -type f)
	rm -rf $(find src -type f -iname "*.o")
	rm -rf $(find src -type f -iname "*.dep")
	;;
load)
	redo all
	sudo avrdude -u -p ${MCU/atmega/m} -c usbasp -B 460800 -U flash:w:$OUT_HEX
	;;
*)
	echo "unknown target '$TARGET'"
	exit 1
	;;
esac
