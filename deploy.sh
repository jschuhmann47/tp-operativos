#!/bin/bash
CWD=$PWD
cd ..
mkdir swap
sudo chmod 777 swap
echo -e "\n\nInstalando las commons...\n\n"
COMMONS="so-commons-library"
git clone "https://github.com/sisoputnfrba/${COMMONS}.git" $COMMONS
cd $COMMONS
sudo make uninstall
make all
sudo make install
cd $CWD
echo -e "\n\nBuildeando el proyecto\n\n"
make all
echo -e "\e[1;32mDeploy done!\e[0m"