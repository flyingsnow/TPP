#!/bin/bash

if [ ! -e plugin ];then
cd ..
fi
cd STAGING
rm -rfv *
cd .. 
#rm *.ipk
#mkdir -p STAGING
rsync -r --exclude=.DS_Store --exclude=.svn enyo/ STAGING/
# rsync -r --exclude=.DS_Store --exclude=.svn mojo/ STAGING/
./linux/buildit_for_device.sh
cp -v touch_mupdf STAGING
echo "filemode.755=touch_mupdf" > STAGING/package.properties
palm-package STAGING
palm-install com.ivan.touch-mupdf_1.0.0_all.ipk
