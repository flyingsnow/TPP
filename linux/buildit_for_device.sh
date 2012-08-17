#!/bin/bash

if [ ! -e plugin ];then
cd ..
fi

if [ ! "$PalmPDK" ];then
PalmPDK=/opt/PalmPDK
MuPDF=/opt/mupdf
fi

PATH=$PATH:${PalmPDK}/arm-gcc/bin

CC="arm-none-linux-gnueabi-gcc"

SYSROOT="${PalmPDK}/arm-gcc/sysroot"

INCLUDEDIR="${PalmPDK}/include"
MUDDF_INCLUDE="${MuPDF}/thirdparty"
MUPDF_FLAGS="-I${MUPDF_INCLUDE}/freetype-2.4.4/include -I${MUPDF_INCLUDE}/jbig2dec -I${MUPDF_INCLUDE}/jpeg-8c" 
MUPDF_FLAGS+="-I${MUPDF_INCLUDE}/zlib-1.2.5 -I${MUPDF_INCLUDE}/openjpeg-1.4/libopenjpeg"
LIBDIR="${PalmPDK}/device/lib"
MUPUDF_LIBDIR="/opt/mupdf"

CPPFLAGS="-I${INCLUDEDIR} -I${INCLUDEDIR}/SDL --sysroot=$SYSROOT"
LDFLAGS="-L${LIBDIR} -Wall,-rpath-link,${LIBDIR},-noinhibit-exec"

LIBS="-lSDL -lGLESv2 -lpdl"
MUPDF_LIBS="-I${MuPDF}/fitz -I${MuPDF}/pdf -I${MuPDF}/xps -I${MuPDF}/scripts"
MUPDF_LDFLAGS="-L${MUPDF_LIBDIR} -Wall -pipe -g -mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp -ffast-math"
MUPDF_LDFLAGS+=" -fsingle-precision-constant"
STATIC_LIBDIR="${MuPDF}/build/release"
rm plugin/*.o
rm touch_mupdf

$CC $CPPFLAGS $MUPDF_FLAGS $MUPDF_LDFLAGS $MUPDF_LIBS  $LIBS -o plugin/SDL_rotozoom.o  -c plugin/SDL_rotozoom.c
$CC $CPPFLAGS $MUPDF_FLAGS $MUPDF_LDFLAGS $MUPDF_LIBS  $LIBS -o plugin/pdfapp.o  -c plugin/pdfapp.c
$CC $CPPFLAGS $MUPDF_FLAGS $MUPDF_LDFLAGS $MUPDF_LIBS  $LIBS -o plugin/tp_main.o  -c plugin/tp_main.c

$CC -o touch_mupdf plugin/tp_main.o plugin/pdfapp.o plugin/SDL_rotozoom.o ${STATIC_LIBDIR}/libmupdf.a  ${STATIC_LIBDIR}/libmuxps.a  ${STATIC_LIBDIR}/libfitz.a  ${STATIC_LIBDIR}/libfreetype.a  ${STATIC_LIBDIR}/libjbig2dec.a ${STATIC_LIBDIR}/libjpeg.a ${STATIC_LIBDIR}/libopenjpeg.a ${STATIC_LIBDIR}/libz.a $LDFLAGS  -g $LIBS -lm

#$CC $CPPFLAGS $LDFLAGS $LIBS -o shapespin_plugin plugin/simple.cpp
