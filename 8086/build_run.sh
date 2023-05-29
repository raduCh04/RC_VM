#!/bin/bash

pushd ..
./premake5 gmake
make
cd bin/Debug-linux-x86/8086
./8086
popd
