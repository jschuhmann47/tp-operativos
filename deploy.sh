#!/bin/bash
mkdir utnso
cd utnso
CWD=$PWD
echo -e "\n\nInstalando las commons...\n\n"
COMMONS="so-commons-library"
git clone "https://github.com/sisoputnfrba/${COMMONS}.git" $COMMONS
cd $COMMONS
sudo make uninstall
make all
sudo make install
cd $CWD
mkdir swap
echo -e "\n\nBuildeando el proyecto\n\n"
make all
echo -e "\n\nDeploy done!\n\n"