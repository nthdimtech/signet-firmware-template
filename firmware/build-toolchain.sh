ARM_NONE_TOOLCHAIN=$HOME/x-tools/arm-none-eabi
ARM_NONE_SYSROOT=$ARM_NONE_TOOLCHAIN/arm-none-eabi/
export CPLUS_INCLUDE_PATH=
git submodule update --init &&
cd crosstool-ng &&
./bootstrap &&
./configure &&
make &&
sudo make install &&
cd .. &&
cd ct-ng &&
ct-ng build &&
cd ..
