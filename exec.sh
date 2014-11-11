#!/bin/sh

strategy_name="s12t253_${1}"
file_path="usr/${strategy_name}"
log="s12t253_${1}_${2}"

./make.sh PokerOpe.c

./Poker.out ${log} 10000 test.ini 0

if [ ! -d ${file_path} ]; then
    mkdir ${file_path}
fi
cp test.ini ${file_path}/
cp PokerOpe.c ${file_path}/
