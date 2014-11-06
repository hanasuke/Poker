//====================================================================
//  �H�w���u���������Q�v(�x�i)  C����v���O���~���O  ���
//  �|�[�J�[�Q�[���̐헪  �Q�[���{��
//--------------------------------------------------------------------
//  Poker  PokerExec.c
//  WinXP DOS  Borland C/C++ 5.5
//--------------------------------------------------------------------
//  �x�i������  tominaga �x�i�_�V
//  2013.10.22
//====================================================================


//====================================================================
//  �d�l
//====================================================================

/*--------------------------------------------------------------------

����^(��D����1�����̂ĂĂ���A�R�D����1����������)

---------------------------------------------------------------------*/


//====================================================================
//  ���O����
//====================================================================

//--------------------------------------------------------------------
//  �w�b�_�t�@�C��
//--------------------------------------------------------------------

#include <stdio.h>     // �W���D�o��
#include <stdlib.h>    // �W�����C�u����
#include <string.h>    // �����񏈗�
#include <time.h>      // ���Ԋ֐�
#include <math.h>      // ���w�֐�
#include <dirent.h>    // �f�B���N�g������

#include "Poker.h"     // �ۑ胉�C�u�����̃w�b�_�t�@�C��(���΃p�X�w��)


//====================================================================
//  ���錾
//====================================================================

//--------------------------------------------------------------------
//  �֐��錾
//--------------------------------------------------------------------

// �w�b�_�t�@�C��Poker.h���C���N���[�h

//--------------------------------------------------------------------
//  ���ϐ�
//--------------------------------------------------------------------

char Name[GAME_NAME_LEN];         // �Q�[����
int Trial;                        // �g���C��
char Stck[FILE_NAME_LEN];         // �R�D�t�@�C��
int Disp_Mode;                    // �\�����[�h
char Dir[DIR_NAME_LEN];           // ���O�f�B���N�g���ւ̃p�X��
FILE *Game_Log;                   // �Q�[�����O�ւ̃t�@�C���|�C���^
FILE *Result_Log;                     // ���ʃ��O�ւ̃t�@�C���|�C���^
int Hand_Value[10] = {P0, P1, P2, P3, P4, P5, P6, P7, P8, P9};
double Take_Weight[10] = {1.0, 1.0, 1.5, 1.5, 2.0, 2.0};


//====================================================================
//  �{�̊֐�
//====================================================================

int main(int argc, char *argv[]) 
{
  //----  �Ǐ��錾
  int point[GNUM][TAKE];    // �e�񓾓_
  FILE *fp;                 // �R�D�ւ̃t�@�C���|�C���^
  double total;             // �������_
  double ave;               // ���ϓ��_
  int k;                    // �����ϐ�

  //----  �R�}���h�����̊m�F
  if ( argc < 5 ) {
    puts("ERROR : �R�}���h�������s���ł��B");
    puts("���s�p�����^ : �Q�[���� �g���C�� �R�D�t�@�C�� �\�����[�h");
    exit(EXIT_FAILURE);
  }

  //----  �R�}���h�����̊i�[
  strcpy(Name, argv[1]);    // �Q�[����
  Trial = atoi(argv[2]);    // �g���C��
  strcpy(Stck, argv[3]);    // �R�D�t�@�C��
  Disp_Mode = atoi(argv[4]);     // �\�����[�h

  //----  �g���C�񐔂̊m�F
  if ( Trial > GNUM ) {
    puts("ERROR : �g���C�񐔂��������܂��B");
    exit(EXIT_FAILURE);
  }

  //----  �R�D�t�@�C���̃I�[�v���ƃG���[����
  fp = fopen(Stck, "r");
  if ( fp == NULL ) {
    puts("ERROR : �R�D�t�@�C���̎w�肪�s���ł��B");
    exit(EXIT_FAILURE);
  }

  //----  �t�@�C���̍쐬
  make_dir();    // �f�B���N�g��
  make_log();    // ���O�t�@�C��

  //----  �����̏�����
  srand(time(NULL));
  puts("");

  //----  �g���C�񐔂����̃Q�[���̔���
  total = 0.0;
  for ( k = 0; k < Trial; k++ ) {
    total += poker_exec(fp, point[k]);
  }

  //----  ���ʕ\�̏o��
  result_show(point);

  //----  ���ϓ_�̏o��
  ave = (double) total / Trial;
  my_printf(Result_Log, "\n");
  my_printf_d(Result_Log, "���ϓ��_ :  %lf\n", ave);

  //----  �t�@�C���̃N���[�Y
  fclose(fp);
  fclose(Game_Log);
  fclose(Result_Log);

  //----  �I��
  return 0;
}


//====================================================================
//  �|�[�J�[�̐i�s
//====================================================================

//--------------------------------------------------------------------
//  �e�g���C�̐i�s
//--------------------------------------------------------------------

double poker_exec(FILE *fp, int point[]) 
{
  //----  �Ǐ��ϐ�
  int stock[CNUM];            // �R�D
  int used[CNUM];             // �̎D�z��
  int us;                     // �̎D��
  double try_p;               // �g���C���_
  int tk;                     // �e�C�N��
  
  //----  �g���C�̏���
  card_stock(stock, fp);      // �R�D�̐���
  
  //----  �g���C�̍Œ�(�e�C�N���J��Ԃ�)
  us = 0;
  try_p = 0;
  for ( tk = 0; tk < TAKE; tk++ ) {
    point[tk] = poker_take(stock, tk, used, &us);
    try_p += point[tk] * Take_Weight[tk];
  }
  
  switch ( Disp_Mode ) {
  case 1:
    // �g���C���_�̕\��(���O�ۑ��t)
    my_printf_i(Game_Log, "�g���C���_ : %4d\n", try_p);
    break;
  case 0:
    // �g���C���_�̃��O�ւ̕ۑ�
    fprintf(Game_Log, "�g���C���_ : %4d\n", try_p);
    break;
  }
  
  return try_p;
}

//--------------------------------------------------------------------
//  �e�C�N�̎��s
//--------------------------------------------------------------------

int poker_take(const int stock[], int tk, int used[], int *us) 
{
  //----  �Ǐ��ϐ�
  int field[FNUM];            // ��D
  int hand[HNUM];             // ��D
  int state[CHNG+1][HNUM];    // �ǖ�(��D�̗�)
  int ope[CHNG];              // �Ŏ�D
  int the;                    // �Ŏ�
  int cg;                     // �`�F���W��
  int take_p;                 // �e�C�N���_
  int k;                      // �����ϐ�

  //----  �e�C�N�̏���
  if ( *us +5 < CNUM ) {
    arr_copy(hand, &stock[*us], HNUM); // ��D�̔z�z(5��)
    arr_copy(state[0], hand, HNUM);    // �ǖʂ̋L�^
  } else {
    return 0;
  }

  //----  �e�C�N�̍Œ�(�`�F���W�̌J�Ԃ�)
  for ( cg = 0; cg < CHNG; cg++ ) {
    //----  
    if ( *us < CNUM-5 ) {
      the = strategy(hand, field, cg, tk, used, *us);  // �헪�ɂ��̎D������
    } else {
      the = -1; break;
    }
    //----  
    ope[cg] = hand[the];                  // �Ŏ�D�̋L�^
    if ( the < 0 ) { break; }             // ��D�����������Ƀe�C�N�I��
    field[cg] = ope[cg];                  // �̎D����D�ɒǉ�
    used[(*us)++] = ope[cg];              // �̎D���̎D�z��ɒǉ�
    hand[the] = stock[HNUM-1+(*us)];      // ��D������
    arr_copy(state[cg+1], hand, HNUM);    // �ǖʂ̋L�^
  }

  //----  �e�C�N�̌㏈��
  take_p = poker_point(hand);                 // �e�C�N���_

  for ( k = 0; k < HNUM; k++ ) {
    used[*us] = hand[k];                      // ��D���̎D�z��ɒǉ�
    (*us)++;
  }

  //----  �e�C�N�̌o�ߕ\��
  switch ( Disp_Mode ) {
  case 1:
    take_show(state, ope, field, cg, take_p); // �e�C�N�̕\��
    //----  �e�C�N���_�̕\��
    my_printf_i(Game_Log, "�e�C�N�f�_ : %4d\n", take_p);
    my_printf_i(Game_Log, "�e�C�N���_ : %4d\n", take_p*Take_Weight[tk]);
    break;
  case 0:
    take_log(Game_Log, state, ope, field, cg, take_p);   // �e�C�N�̃��O
    //----  �e�C�N���_�̃��O�ւ̕ۑ�
    fprintf(Game_Log, "�e�C�N���_ : %3d\n", take_p);
    break;
  }
  
  return take_p;
}


//====================================================================
//  �Q�[���̕\��
//====================================================================

//--------------------------------------------------------------------
//  �e�C�N�̕\��
//--------------------------------------------------------------------

void take_show(int st[][HNUM], int ope[], int fd[], int cg, int tp) 
{
  int k;
  int p;
  for ( k = 0; k <= cg; k++ ) {
    //----  ��D�̕\��
    my_printf_i(Game_Log, "[%d] ", k); 
    card_show(st[k], HNUM); 
    card_show_log(Game_Log, st[k], HNUM);
    //----  �̎D�̕\��
    if ( k < cg ) { my_printf_c(Game_Log, " >%s", card_str(ope[k])); }
    my_printf(Game_Log, "\n");
  }
}

//--------------------------------------------------------------------
//  �e�C�N�̃��O�t�@�C���ւ̏��o��
//--------------------------------------------------------------------

void take_log(FILE *fp, int st[][HNUM], int ope[], int fd[], int cg, int tp) 
{
  int k;
  int p;
  for ( k = 0; k <= cg; k++ ) {
    //----  ��D�̕\��
    fprintf(Game_Log, "[%d] ", k); 
    //card_show(st[k], HNUM); 
    card_show_log(Game_Log, st[k], HNUM);
    //----  �̎D�̕\��
    if ( k < cg ) { fprintf(Game_Log, " >%s", card_str(ope[k])); }
    fprintf(Game_Log, "\n");
  }
}

//--------------------------------------------------------------------
//  ���ʕ\�̕\��
//--------------------------------------------------------------------

void result_show(int point[][TAKE]) 
{
  int i, j, k;
  int deg[POINT_NUM][TAKE];    // 
  int sum[POINT_NUM];          // 
  double scr[TAKE] = {0.0};    // 
  double total = 0.0;          // 

  //----  �z��̏�����
  for( k = POINT_MIN; k <= POINT_MAX; k++ ) {
    sum[k] = 0;
    for( j = 0; j < TAKE; j++ ) {
      deg[k][j] = 0;
    }
  }
  
  //----  �����Ƃ̏W�v
  for ( i = 0; i < Trial; i++ ) {
    for ( j = 0; j < TAKE; j++ ) {
      switch( point[i][j] ) {
      case P9: deg[9][j]++; break;
      case P8: deg[8][j]++; break;
      case P7: deg[7][j]++; break;
      case P6: deg[6][j]++; break;
      case P5: deg[5][j]++; break;
      case P4: deg[4][j]++; break;
      case P3: deg[3][j]++; break;
      case P2: deg[2][j]++; break;
      case P1: deg[1][j]++; break;
      case P0: deg[0][j]++; break;
      default : break;
      }
    }
  }

  //----  �S�e�C�N�ł̖��̑���
  for( k = POINT_MIN; k <= POINT_MAX; k++ ) {
    for( j = 0; j < TAKE; j++ ) {
      sum[k] += deg[k][j];
      scr[j] += deg[k][j] * Hand_Value[k];
    }
  }
  puts("");

  //----  ���ʕ\�̃e�C�N�ԍ��̕\��
  my_printf(Result_Log, "        ��         ");
  for( i = 0; i < TAKE; i++ ) {
    my_printf_i(Result_Log, "| take%d", i+1);
  }
  my_printf(Result_Log, "|  ���v  \n");
  //-----  ���ʕ\�̋�ؐ��̕\��
  my_printf(Result_Log, "---------------------");
  for( i = 0; i <= TAKE; i++ ) {
    my_printf(Result_Log, "+------");
  }
  my_printf(Result_Log, "\n");

  for ( i = POINT_MIN; i <= POINT_MAX; i++ ) {
    //----  ���ʕ\�̖𖼂̕\��
    switch( i ) {
      case 9: my_printf(Result_Log, "���C�����X�g���[�g   "); break;
      case 8: my_printf(Result_Log, "�X�g���[�g�t���b�V�� "); break;
      case 7: my_printf(Result_Log, "�t�H�[�J�[�h         "); break;
      case 6: my_printf(Result_Log, "�t���n�E�X           "); break;
      case 5: my_printf(Result_Log, "�t���b�V��           "); break;
      case 4: my_printf(Result_Log, "�X�g���[�g           "); break;
      case 3: my_printf(Result_Log, "�X���[�J�[�h         "); break;
      case 2: my_printf(Result_Log, "�c�[�y�A             "); break;
      case 1: my_printf(Result_Log, "�����y�A             "); break;
      case 0: my_printf(Result_Log, "�m�[�y�A             "); break;
      default : break;
    }
    for ( j = 0; j < TAKE; j++ ) {
      //----  ���ʕ\�̖�p�x�̕\��
      my_printf_i(Result_Log, "|%6d", deg[i][j]);
      if( j == TAKE-1 ) {
        my_printf_i(Result_Log, "|%6d", sum[i]);
      }
    }
    my_printf(Result_Log, "\n");
  }

  //-----  ���ʕ\�̋�ؐ��̕\��
  my_printf(Result_Log, "---------------------");
  for( i = 0; i <= TAKE; i++ ) {
    my_printf(Result_Log, "+------");
  }
  my_printf(Result_Log, "\n");

  //----  
  my_printf(Result_Log, "�P���f�_             ");
  for ( j = 0; j < TAKE; j++ ) {
    my_printf_d(Result_Log, "|%6.2f", scr[j] / Trial);
  }
  my_printf(Result_Log, "|\n");

  //----  
  my_printf(Result_Log, "�X�Γ��_             ");
  for ( j = 0; j < TAKE; j++ ) {
    my_printf_d(Result_Log, "|%6.2f", scr[j] * Take_Weight[j]/Trial);
    total += scr[j] * Take_Weight[j];
  }
  my_printf_d(Result_Log, "|%6.2f\n", total / Trial);
}


//====================================================================
//  �|�[�J�[�̖��Ɠ��_
//====================================================================

//--------------------------------------------------------------------
//  �|�[�J�[�̓��_
//--------------------------------------------------------------------

int poker_point(int hand[]) 
{
  //----  �Ǐ��錾
  int num[13] = {0};     // ���ʂ��Ƃ̌����i�[����z��(A23456789TJQK)
  int sut[4] = {0};      // ��ނ��Ƃ̌����i�[����z��(SHDC)
  int p0, p1, p2, p3;    // �ŏI����ьn�ʂ̖��̓��_
  int k;    // �����ϐ�
  int t;    // �ꎞ�ϐ�

  //----  ���ʂ���ю�ނ��Ƃ̌v��
  for ( k = 0; k < HNUM; k++ ) { t = hand[k] % 13; num[t]++; }    // ����
  for ( k = 0; k < HNUM; k++ ) { t = hand[k] / 13; sut[t]++; }    // ���

  //----  �n�ʂ̖��̔���
  p1 = poker_point_pair(num);            // �y�A�n
  p2 = poker_point_flash(sut);           // �t���b�V���n
  p3 = poker_point_straight(num, p2);    // �X�g���[�g�n(���C��������̂��߂̈����t)

  //----  �ō��_�̖����̗p
  p0 = p1;                       // �y�A�n�̖��̓��_
  if ( p0 < p2 ) { p0 = p2; }    // �t���b�V���n�̖��ւ̕ύX
  if ( p0 < p3 ) { p0 = p3; }    // �X�g���[�g�n�̖��ւ̕ύX

  //----  �m�肵�����̕ԋp
  return p0;
}

//--------------------------------------------------------------------
//  �y�A����
//--------------------------------------------------------------------

int poker_point_pair(int num[]) 
{
  //----  �Ǐ��錾
  int c2 = 0, c3 = 0;
  int k;

  //----  �t�H�[�J�[�h
  for ( k = 0; k < 13; k++ ) { if ( num[k] == 4 ) { return P7; } }

  //----  ���̑��̃y�A�v�f�̌��o
  for ( k = 0; k < 13; k++ ) { 
    if ( num[k] == 3 ) { c3++; } 
    if ( num[k] == 2 ) { c2++; } 
  }

  //----  �t���n�E�X
  if ( c3 == 1 && c2 == 1 ) { return P6; }
  //----  �X���[�J�C���Y
  if ( c3 == 1 ) { return P3; }
  //----  �c�[�y�A
  if ( c2 == 2 ) { return P2; }
  //----  �����y�A
  if ( c2 == 1 ) { return P1; }
  //----  �m�[�y�A
  return 0;
}

//--------------------------------------------------------------------
//  �t���b�V������
//--------------------------------------------------------------------

int poker_point_flash(int sut[]) 
{
  int k;
  for ( k = 0; k < 4; k++ ) { if ( sut[k] == 5 ) { return P5; } }
  return P0;
}

//--------------------------------------------------------------------
//  �X�g���[�g����
//--------------------------------------------------------------------

int poker_point_straight(int num[], int p) 
{
  int len = 0;
  int k;
  for ( k = 0; k < 13; k++ ) {
    if ( num[k] > 0 ) { 
      len++; 
      if ( len == 5 ) { break; }
    } else { len = 0; }
  }

  //----  ���C�����X�g���[�g(TJQKA)
  if ( len == 4 && num[0] == 1 ) { len = 6; }
  //----  ���C�����X�g���[�g�t���b�V��
  if ( len == 6 ) { if ( p > 0 ) { return P9; } else { return P4; } }
  //----  �m�[�}���X�g���[�g
  if ( len == 5 ) { if ( p > 0 ) { return P8; } else { return P4; } }

  //----  
  return 0;
}


//====================================================================
//  �t�@�C������
//====================================================================

//--------------------------------------------------------------------
//  ���O�f�B���N�g���̍쐬
//--------------------------------------------------------------------

void make_dir(void) 
{
  char command[COMM_LEN];         // �V�X�e���R�}���h
  DIR *log_dir;                   // ���O�f�B���N�g��

  //----  ���O�f�B���N�g���ւ̃p�X
  strcpy(Dir, "..\\..\\log\\");
  strcat(Dir, Name);

  //----  ���O�f�B���N�g���̗L��
  if ( (log_dir = opendir(Dir)) == NULL ) {
    //----  ���O�f�B���N�g���̍쐬(md�R�}���h�̌ďo)
    strcpy(command, "md ");
    strcat(command, Dir);
    system(command);
  }
}

//--------------------------------------------------------------------
//  ���O�t�@�C���̍쐬
//--------------------------------------------------------------------

void make_log(void) 
{
  char path1[FILE_NAME_LEN];            // Game.log�ւ̃p�X
  char path2[FILE_NAME_LEN];            // Result.txt�ւ̃p�X
  char path3[FILE_NAME_LEN];            // Stock.ini�ւ̃p�X
  char command[COMM_LEN];               // �V�X�e���R�}���h

  //---- Game.log�ւ̃p�X���쐬
  strcpy(path1, Dir); 
  strcat(path1, "\\Game.log");

  //---- Result.txt�ւ̃p�X���쐬
  strcpy(path2, Dir); 
  strcat(path2, "\\Result.txt");

  //---- Stock.ini�ւ̃p�X���쐬
  strcpy(path3, Dir); 
  strcat(path3, "\\Stock.ini"); 

  //---- Game.log�̃I�[�v��
  if( (Game_Log = fopen(path1, "w")) == NULL ) {
    printf("Game.log���I�[�v���ł��Ȃ�\n");
    exit(1);
  }

  //---- Result.txt�̃I�[�v��
  if( (Result_Log = fopen(path2, "w")) == NULL ) {
    printf("Result.txt���I�[�v���ł��Ȃ�\n");
    exit(1);
  }

  //---- Stock.ini�̃R�s�[(copy�R�}���h�̌ďo)
  strcpy(command, "copy ");
  strcat(command, Stck);
  strcat(command, " ");
  strcat(command, path3);
  system(command);
}

//--------------------------------------------------------------------
//  �W���o�͂ƃt�@�C���o��
//--------------------------------------------------------------------

//----  �W���o�́{�t�@�C���o��
void my_printf(FILE *fp, char st[]) 
{
  printf(st);
  fprintf(fp, st);
}

//----  �W���o�́{�t�@�C���o��(char�^�̔z��)
void my_printf_c(FILE *fp, char st[], char x[]) 
{
  printf(st, x);
  fprintf(fp, st, x);
}

//----  �W���o�́{�t�@�C���o��(int�^)
void my_printf_i(FILE *fp, char st[], int x) 
{
  printf(st, x);
  fprintf(fp, st, x);
}

//----  �W���o�́{�t�@�C���o��(double�^)
void my_printf_d(FILE *fp, char st[], double x) 
{
  printf(st, x);
  fprintf(fp, st, x);
}

