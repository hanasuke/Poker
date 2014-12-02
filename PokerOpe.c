//====================================================================
//  工学部「情報環境実験２」(富永)  C言語プログラミング  例題
//  ポーカーゲームの戦略
//--------------------------------------------------------------------
//  Poker  PokerOpe.c
//  Linux2.x gcc 4.x
//  MacOS X clang 6.0
//--------------------------------------------------------------------
//  富永研究室  s12t253 花川直己
//  2014.11.11
//====================================================================


//====================================================================
//  仕様
//====================================================================

/*--------------------------------------------------------------------

手札、場札(以前の捨札の列)、回数を入力とし、捨札を決める。
返却値は、捨札の位置である。-1のときは、交換しないで、手札を確定させる。
関数 strategy() は、戦略のインタフェースであり、この関数内で、
実際の戦略となる関数を呼び出す。手札と捨札は、不正防止のため、
変更不可なので、局所配列にコピーして、整列などの処理を行う。
複数の戦略を比較したり、パラメタを変化させて、より強い戦略を目指す。

---------------------------------------------------------------------*/


//====================================================================
//  前処理
//====================================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "Poker.h"

//--------------------------------------------------------------------
//  関数宣言
//--------------------------------------------------------------------

void check_myhd(int hd[], int *num, int *suite);
int  burn_index(int hd[], int fd[], int cg, int tk, int ud[], int us, int num[], int suite[]);

//====================================================================
//  戦略
//====================================================================

/*--------------------------------------------------------------------
//  ユーザ指定
//--------------------------------------------------------------------

手札を表示して、ユーザが捨札の位置を指定する。
位置が範囲外のときは、交換しないで、手札を確定させる。

hd : 手札配列
fd : 場札配列
cg : チェンジ数
tk : テイク数
ud : 捨札配列
us : 捨札数

--------------------------------------------------------------------*/

int strategy(const int hd[], const int fd[], int cg, int tk, const int ud[], int us) {
  int myhd[HNUM];
  int hdnum[13] = {0};  // 数位
  int hdsuite[4] = {0}; // マーク
  arr_copy(myhd, hd, HNUM);
  check_myhd(myhd, hdnum, hdsuite);

  if ( poker_point(myhd) > P7 ) { return -1; }

  return burn_index(myhd, fd, cg, tk, ud, us, hdnum, hdsuite);
}


//====================================================================
//  補助関数
//====================================================================

//--------------------------------------------------------------------
// 数位・図種別に数値を格納
// 引数: 手札、数位配列、図種配列
// 返却: なし
//--------------------------------------------------------------------

void check_myhd(int hd[], int *num, int *suite) {
  int k;
  int num_index;
  int suite_index;

  for ( k = 0 ; k < HNUM; k++ ) {
    num_index = hd[k] % 13;
    suite_index = hd[k] % 4;
    num[num_index]++;
    suite[suite_index]++;
  }
}

//--------------------------------------------------------------------
// 捨て札決定
// 引数: strategy()の引数+数位配列、図種配列
// 返却: 捨て札のインデックス(0-4)
//--------------------------------------------------------------------
// strategy()の引数
// hd : 手札配列
// fd : 場札配列
// cg : チェンジ数
// tk : テイク数
// ud : 捨札配列
// us : 捨札数
//--------------------------------------------------------------------

int  burn_index(int hd[], int fd[], int cg, int tk, int ud[], int us, int num[], int suite[]) {

  if ( tk > 2 ){
    if ( poker_point(hd) > P3 ) { return -1; }
  } else {
    if ( poker_point(hd) > P2 ) { return -1; }
  }

  return rand()%5;
}
