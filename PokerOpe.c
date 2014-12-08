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

int check_flash(int hd[], int fd[], int suite[]);
int check_pair(int hd[], int fd[], int num[]);
int check_straight(int hd[], int fd[], int num[]);
int check_not_continue(int hd[], int num[]);

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

int strategy(const int hd[], const int fd[], int cg, int tk, const int ud[], int us)
{
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

void check_myhd(int hd[], int *num, int *suite)
{
  int k;
  int num_index;
  int suite_index;

  for ( k = 0 ; k < HNUM; k++ ) {
    num_index = hd[k] % 13;
    suite_index = hd[k] / 13;
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

int  burn_index(int hd[], int fd[], int cg, int tk, int ud[], int us, int num[], int suite[])
{
  int index_flash;
  int index_straight;
  int index_pair;

  if ( tk > 2 ){
    if ( poker_point(hd) >= P4 ) { return -1; }
  } else {
    if ( poker_point(hd) >= P5 ) { return -1; }
  }

  index_straight = check_straight(hd, fd, num);
  index_flash = check_flash(hd, fd, suite);
  index_pair = check_pair(hd, fd, num);


  if ( index_straight == index_flash && index_straight >= -1 ) {
    // ストレートフラッシュの可能性
    return index_flash;
  } else if ( index_straight >= 0 && index_flash >= 0 ) {
    // とりあえずフラッシュ狙い
    return index_flash;
  }

  if ( index_straight == -1 ) {
    return -1;
  } else if ( index_straight >= 0 ) {
    return index_straight;
  }

  // フラッシュ成立
  if ( index_flash == -1 ) {
    return -1;
  } else if ( index_flash >= 0 ){
    return index_flash;
  } else {
    return index_pair;
  }
}

//--------------------------------------------------------------------
// フラッシュチェック
// 引数: 手札配列、場札配列、図種配列
// 返却: 最も少ない図種の最小の手札の配列添字
//--------------------------------------------------------------------

int check_flash(int hd[], int fd[], int suite[]) {
  int j;
  int k;
  int max;
  int hdsuite;

  for ( k = 0; k < 4; k++ ) {
    hdsuite = k;
    // 既にフラッシュがある場合
    if ( suite[k] == 5 ) {
      return -1;
    } else if ( suite[k] >= 3 ) {
      // フラッシュ-2以上がある場合
      for ( j = 0; j < HNUM; j++ ) {
        if ( hd[j]/13 != hdsuite ) {
          return j;
        }
      }
    }
  }

  return -2;
}


//--------------------------------------------------------------------
// ペアチェック
// 引数: 手札配列、場札配列、数位配列
// 返却: ペアでない最小の手札の配列添字
//--------------------------------------------------------------------

int check_pair(int hd[], int fd[], int num[])
{
  int k;
  int hdnum;
  int ct = 0;

  // 既にペアがないか確認
  for ( k = 0; k < 13; k++ ) {
    if ( num[k] >= 2 ) {
      ct++;
    }
  }
  if ( ct == 2 ) {
    return -1;
  }

  for ( k = 0; k < HNUM; k++ ) {
    hdnum = hd[k]%13;
    if ( num[hdnum] < 2 ) {
      return k;
    }
  }
}

//--------------------------------------------------------------------
// ストレートチェック
// 引数: 手札配列、場札配列、数位配列
// 返却: ふっとんだ変な手札の数位札の配列添字
//--------------------------------------------------------------------

int check_straight(int hd[], int fd[], int num[])
{
  int k;
  int hdnum;
  int length = 0;
  int max_length = 0;
  int ret = 0;

  for ( k = 0; k < 13; k++ ) {
    if ( num[k] > 0 ) {
      length++;
    } else {
      if ( length == 5 ) {
        return -1;
      }
      if ( length > max_length ) {
        max_length = length;
        length = 0;
      }
    }
  }
  if ( max_length == 4 ) {
    return check_not_continue(hd, num);
  }
  return -2;
}

//--------------------------------------------------------------------
// 連番の仲間はずれを探すやつ
// 引数: 手札配列、数位配列
// 返却: 連番になってない手札の添字
//--------------------------------------------------------------------

int check_not_continue(int hd[], int num[])
{
  int i;
  int hdnum;

  for ( i = 0; i < HNUM; i++ ) {
    hdnum = hd[i]%13;
    if ( num[hdnum] > 1 ) {
      return i;
    }
    if ( ! hdnum ) {
      if ( ! num[hdnum+1] ) {
        return i;
      }
    } else if ( hdnum == 12 ) {
      if ( ! num[hdnum-1] ) {
        return i;
      }
    }
    if ( ! num[hdnum-1] && ! num[hdnum+1]) {
      return i;
    }
  }
  return -2;
}
