type="減-07-05"
gcc -g -I ./src -o Poker.out -DTHYPE=${type} -DCHNG=7 -DTAKE=5 ${1} ./src/CardLib.c ./src/PokerExec.c
