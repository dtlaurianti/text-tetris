#! /usr/bin/env bash

cd src
rm hs.csv
make || exit 1
./text-tetris 2>error
cat error
cd ..

