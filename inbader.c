#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define WIDE 15
#define HIGH 20
#define EMAX 1

struct _piece
{
   int Block;
   int Enemy_N;
   //0=Empty   1=Enemy  2=Enemybullet  3=MyMachine    4=Mybullet  5=Block
   int pieceInf;
   //int myMachine;
};
struct _flag
{
   int MyReloadFlag;
   int BombReloadFlag;
   int MybulletFlag;
   int MyBombFlag;
   int BulletFlag;
};

int Empty, Enemy, Enemybullet, MyMachine, Mybullet, Block;
//自分の座標
int MyLocationX;
//プレイ画面座標系
struct _piece Masu[WIDE][HIGH];
//フラグ一覧
struct _flag Flag;

void EnemybulletUp();
void MybulletHit();
void MybulletUp();
//敵の動き
void Enemybehave();
//ヒット判定
void Hitenemy();
//自分の攻撃
void Mybulletbehave();
void Bombbehave();
void BombUP();
int Bomb=7;
//敵の攻撃
void Enemybulletbehave();
void EnemybulletHit();
//自分の動き
void Mybehave(char key);
//初期化
void Reset();
//キーボード
char input();
//本体
void Mainframe();
//画面更新
void GamePanel();
//ゲーム画面
int a[WIDE][HIGH];
//終了判定フラグ
int win;//勝ち
int lose;//負け
//スコア
int score;
//時間経過
int Time;
int F;//F=false
int T;//T=true
//攻撃の待機時間
int MyReload;
int BombReload;
int EnemyDown;
int MyPause;
int RorL;//0:右 1:左
int EnemySpeed;
int EBulletSpeed;
int BoReloadSpeed, BuReloadSpeed;

int main(void)
{
   KB_open();
   Reset();
   //ch=input();
   Mainframe();
   KB_close();
   printf("\033[49m");
   printf("\033[0m");
   printf("\033[2J");
   return 0;
}

char input(){
   if (kbhit()) {
      char key = linux_getch();
      return key;
    }
}

void Reset()
{
   Flag.MyBombFlag = 0;
   Flag.BombReloadFlag = 0;
   Flag.BulletFlag = 0;
   Flag.MybulletFlag = 0;
   Flag.MyBombFlag = 0;
   int f=0, n;
   KB_close();
   while(f==0){
      printf("難易度設定(入力してエンターを押してください) 1:イージー 2:ノーマル 3:ハード\n");
      scanf("%d",&n);
      f=1;
      switch (n){
         case 1:
            EnemySpeed = 300;
            EBulletSpeed = 1000;
            BuReloadSpeed = 300;
            BoReloadSpeed = 1000;
            break;
         case 2:
            EnemySpeed = 200;
            EBulletSpeed = 600;
            BuReloadSpeed = 600;
            BoReloadSpeed = 2000;
            break;
         case 3:
            EnemySpeed = 150;
            EBulletSpeed = 300;
            BuReloadSpeed = 1000;
            BoReloadSpeed = 4000;
            break;
         case 4:
            EnemySpeed = 50;
            EBulletSpeed = 150;
            BuReloadSpeed = 2000;
            BoReloadSpeed = 8000;
            break;
         case 12885165:
            EnemySpeed = 300;
            EBulletSpeed = 1000000000;
            BuReloadSpeed = 0;
            BoReloadSpeed = 0;
            break;
         default:
            printf("1~3で指定してください");
            f=0;
            break;
      }
   }
   Empty = 0;
   Enemy = 1;
   Enemybullet = 2;
   MyMachine = 3;
   Mybullet = 4;
   Block = 5;
   MyReload = 0;
   F=0;
   T=1;
   win=0;
   lose=0;
   score=0;
   EnemyDown = 0;
   lose = 0;
   win = 0;
   RorL = 0;
   //敵の初期位置設
   int EnemyNumber = 1;
   int x, y;
   for(y=0; y<HIGH; y++)
   {
      for(x=0; x<WIDE; x++)
      {
         Masu[x][y].pieceInf = Empty;
      }
   }
   for(y = HIGH - 1; y > HIGH - 4; y--)
   {
      //if(EnemyNumber-1 >= EMAX)
      //   break;
      for(x = 0; x < 5; x++)
      {
         Masu[x][y].pieceInf = Enemy;
         EnemyNumber++;
      }
   }

   //自分の初期位置設定
   MyLocationX = WIDE/2;
   Masu[MyLocationX][0].pieceInf = MyMachine;
   //ブロックの設定
   
   Masu[WIDE/4][3].pieceInf = 5;
   Masu[WIDE/4][3].Block = 9;

   Masu[(WIDE/4)*3][3].pieceInf = 5;
   Masu[(WIDE/4)*3][3].Block = 9;
   
   MyPause = 0;
   KB_open();
}

void Mainframe()
{
   char key;
   int i, j;
   score = 0;
   Time = 0;
   int s = 1000;
   while(win == F && lose == F)
   {
      if(MyPause == 1) 
      {
         key = input();
         if(key == 'p')
            MyPause = 0;
         else if(key == 'r')
         {
            MyPause = 0;
            Reset();
         }
         else if(key == 'q')
         {
            lose = 1;
            MyPause = 0;
         }
         continue;
      }
      //待機
      usleep(s);
      Time++;
      //操作
      key=input();
      if(key=='a' || key=='d')
      {
         Mybehave(key);
      }
      else if(key == ' ')
      {
         if(Flag.BombReloadFlag == 0)
         {
            Bombbehave();
            Flag.BombReloadFlag = 1;
            BombReload = 0;
         }
      }
      else if(key == 'w')
      {
         if(Flag.MyReloadFlag == 0)
         {
            Mybulletbehave();
            Flag.MyReloadFlag = 1;
            MyReload = 0;
         }
      }
      else if(key == 'p')
         MyPause = 1;
      //リロード時間
      if(Flag.MyReloadFlag == 1)
      {
         MyReload++;
         if(MyReload >= BuReloadSpeed)
            Flag.MyReloadFlag = 0;
      }
      if(Flag.BombReloadFlag == 1)
      {
         BombReload++;
         if(BombReload >= BoReloadSpeed)
            Flag.BombReloadFlag = 0;
      }
      //敵の動き
      int n = EnemySpeed;
      if(EnemyDown > HIGH * 2)
         n=EnemySpeed/2;
      else if(EnemyDown > (HIGH/4)*3)
         n=(EnemySpeed/3)*2;
      if(Time % n == 0)
         Enemybehave();
      if(Time % 20 == 0)
      {
         //Enemybulletbehave(); 
         MybulletUp();
         BombUP();
         EnemybulletUp();
      }
      if(Time % EBulletSpeed == 0){
         Enemybulletbehave();
      }
      
      //たまの当たり判定
      MybulletHit();
      EnemybulletHit();
      //画面表示
      GamePanel();
      //if(score == 14)
      //   win=T;
   }
   while (key != 'q')
   {
      printf("\033[2J");//画面初期化
      if(win == T)
         printf("WIN\n");
      else if(lose == T)
         printf("LOSE\n");
      printf("rを押すとリトライqを押すと終了\n");
      KB_close();
      scanf("%c",&key);
      if(key == 'r')
      {
         Reset();
         Mainframe();
         return;
      }
   }
}
//画面表示
void GamePanel()
{
   int x, y;
   //printf("%d\n",score);
   printf("\033[2J");//画面初期化
   printf("\033[40m");
   printf("pause:%d\n",MyPause);
   printf("操作方法 w:レーザー スペースキー:爆弾 a:左 d:右 リロード中は1\n");
   printf("BOMB:%d\n",Flag.BombReloadFlag);
   printf("BULLET:%d\n",Flag.MyReloadFlag);
   for(y=HIGH-1; y>=0; y--)
   {
      for(x=0; x<WIDE; x++)
      {
         if(Masu[x][y].pieceInf == Empty)
         {
            printf("[37m・");
         }
         else if(Masu[x][y].pieceInf == Enemy)
         {
            printf("[31m敵");
         }
         else if(Masu[x][y].pieceInf == Enemybullet)
         {
            printf("[32mＶ");
         }
         else if(Masu[x][y].pieceInf == MyMachine)
         {
            printf("[34m自");
         }
         else if(Masu[x][y].pieceInf == Mybullet)
         {
            printf("[33m＾");
         }
         else if(Masu[x][y].pieceInf == Block)
         {
            printf("\033[37m0%d",Masu[x][y].Block);
         }
         else if(Masu[x][y].pieceInf == Bomb)
         {
            printf("\033[37m■ ");
         }
      }
      printf("\033[37m");
      printf("\n");
   }
   if(MyPause == 1)
   {
      printf("PAUSE\n");
      printf("PUSH Q:QUIT   PUSH P:CONTINUE   PUSH R:RESET\n");
   }
}

int MybulletX, MybulletY;
void MybulletUp()
{
   int e_num = 0;
   int i, j;
   for(i=0;i<WIDE;i++){
      for(j=0;j<HIGH;j++){
         if(Masu[i][j].pieceInf == Enemy){
            e_num++;
         }
      }
   }
   if(e_num==0){
   win = T;   
   }
   if(Flag.MybulletFlag == 1){
      
         Masu[MybulletX][MybulletY].pieceInf = Empty; 
         MybulletY++;
         if(MybulletY == HIGH-1)
         {
            Flag.MybulletFlag = 0;
            return;
         }
         else if(Masu[MybulletX][MybulletY+1].pieceInf == Block)
         {
            Masu[MybulletX][MybulletY+1].Block--;
            if(Masu[MybulletX][MybulletY+1].Block <= 0) Masu[MybulletX][MybulletY+1].pieceInf = Empty;
            Flag.MybulletFlag = 0;
            return;

         }
         Masu[MybulletX][MybulletY].pieceInf = Mybullet;
   }
}

int MyBombX, MyBombY;
void MybulletHit()
{
   int i, j;
   if(Flag.MybulletFlag == 1){ 
      if(Masu[MybulletX][MybulletY+1].pieceInf == Enemy){
         score ++;
         Flag.MybulletFlag = 0;
         Masu[MybulletX][MybulletY].pieceInf = Empty;
         Masu[MybulletX][MybulletY+1].pieceInf = Empty;
         return;
      }
      else if(Masu[MybulletX+1][MybulletY].pieceInf == Enemy && MybulletX+1<WIDE){
         score ++;
         Flag.MybulletFlag = 0;
         Masu[MybulletX][MybulletY].pieceInf = Empty;
         Masu[MybulletX+1][MybulletY].pieceInf = Empty;
         return;
      }
      else if(Masu[MybulletX-1][MybulletY].pieceInf == Enemy && MybulletX-1>=0){
         score ++;
         Flag.MybulletFlag = 0;
         Masu[MybulletX][MybulletY].pieceInf = Empty;
         Masu[MybulletX-1][MybulletY].pieceInf = Empty;
         return;
      }
   }
   if(Flag.MyBombFlag == 1){ 
      if(Masu[MyBombX][MyBombY+1].pieceInf == Enemy){
         Flag.MyBombFlag = 0;
         for(i=-1;i<=1;i++)
         {
            for(j=-1;j<=1;j++)
            {
               if(MyBombX == -1 || MyBombX == WIDE)
                  continue;
               Masu[MyBombX+i][MyBombY+j].pieceInf = Bomb;
            }
         }
         return;
      }
   }
}
//自分の攻撃
void Bombbehave()
{
   if(Flag.MyBombFlag == 0)
   {
      MyBombY = 1;
      Masu[MyLocationX][MyBombY].pieceInf = Bomb;
      MyBombX = MyLocationX;
      Flag.MyBombFlag = 1;
   }
}
void BombUP()
{
   if(Flag.MyBombFlag == 1)
   {
      Masu[MyBombX][MyBombY].pieceInf = Empty;
      MyBombY++;
      if(MyBombY==HIGH)
      {
         Flag.MyBombFlag = 0;
         return;
      }
      Masu[MyBombX][MyBombY].pieceInf = Bomb;
   }
}
void Mybulletbehave()
{
   if(Flag.MybulletFlag == 0)
   {
      MybulletY = 1;
      Masu[MyLocationX][MybulletY].pieceInf = Mybullet;
      MybulletX = MyLocationX;
      Flag.MybulletFlag = 1;
   }
}
//自機の動き
void Mybehave(char key)
{
   if(key == 'a')//左
   {
      if(MyLocationX > 0)
      {
         if(Masu[MyLocationX-1][0].pieceInf == Enemybullet)
         {
            lose = 1;
            return;
         }
         Masu[MyLocationX][0].pieceInf = Empty;
         MyLocationX--;
         Masu[MyLocationX][0].pieceInf = MyMachine;
      }
   }
   else if(key == 'd')
   {
      if(MyLocationX < WIDE-1)
      {
         if(Masu[MyLocationX+1][0].pieceInf == Enemybullet)
         {
            lose = 1;
            return;
         }
         Masu[MyLocationX][0].pieceInf = Empty;
         MyLocationX++;
         Masu[MyLocationX][0].pieceInf = MyMachine;
      }
   }
}

//敵の動き
void Enemybehave()
{
   int x, y;
   for(y=HIGH; y>=0; y--)
   {
      for(x=0; x<WIDE; x++)
      {
         if(Masu[x][y].pieceInf == Enemy)
         {
            if(y == 0)
            {
               lose = 1;
               break;
            }
         }
      }
      if(lose == 1)
         return;
   }
   if(RorL == 0)
   {
      for(x=WIDE-1; x>=0; x--)
      {
         for(y=0; y<HIGH; y++)
         {
            if(Masu[x][y].pieceInf == Enemy)
            {
               if(x+1 == WIDE)//壁にあたる
               {
                  Masu[x][y-1].pieceInf = Enemy;
                  Masu[x][y].pieceInf = Empty;
                  RorL = 1;
                  EnemyDown++;
               }
               else if(RorL == 0)//壁に当たってない
               {
                  Masu[x+1][y].pieceInf = Enemy;
                  Masu[x][y].pieceInf = Empty;
               }
               else
               {
                  Masu[x][y-1].pieceInf = Enemy;
                  Masu[x][y].pieceInf = Empty;
               }
            }
         }
      }
   }
   else
   {
      for(x=0; x<WIDE; x++)
      {
         for(y=0; y<HIGH; y++)
         {
            if(Masu[x][y].pieceInf == Enemy)
            {
               if(x-1 == -1)
               {
                  Masu[x][y-1].pieceInf = Enemy;
                  Masu[x][y].pieceInf = Empty;
                  RorL = 0;
                  EnemyDown++;
               }
               else if(RorL == 1)
               {
                  Masu[x-1][y].pieceInf = Enemy;
                  Masu[x][y].pieceInf = Empty;
               }
               else
               {
                  Masu[x][y-1].pieceInf = Enemy;
                  Masu[x][y].pieceInf = Empty;
               }
            }
         }
      }
   }
}

//敵の弾の処理
struct eLocation
{
   int x;
   int y;
};

int r;
struct eLocation eL[900] = {0,0};
void Enemybulletbehave()
{
   int x;
   int y;
   int n;
   //敵の位置情報
   if(Flag.BulletFlag == F)
   {
      n=0;
      for(y=0; y<HIGH; y++)
      {
         if(eL[n-1].y != 0 && eL[n-1].y != y)
         {
            break;
         }
         for(x=0; x<WIDE; x++)
         {
            if(Masu[x][y].pieceInf == Enemy)
            {
               eL[n].x = x;
               eL[n].y = y;
               n++;
            }
         //printf("%d\n",Masu[x][y].Enemy_N);
         }
      }
      srand((unsigned int)time(NULL));
      if(n==1)
         r=0;
      else
         r = (rand())%(n-1);
      /*if(r==0)
      {
         r=1;
      }*/
      Masu[eL[r].x][eL[r].y-1].pieceInf = Enemybullet;
      Flag.BulletFlag = 1;
      eL[r].y--;
      return;
   }
}

void EnemybulletUp()
{ 
   if(Flag.BulletFlag == 1)
   {
      Masu[eL[r].x][eL[r].y].pieceInf = Empty;
      //if(eL[r].y != 0)
      eL[r].y = eL[r].y - 1;
         Masu[eL[r].x][eL[r].y].pieceInf = Enemybullet;
      
      if(Masu[eL[r].x][eL[r].y-1].pieceInf == MyMachine)
      {
         lose = 1;
         return;
      }
      else if(eL[r].y-1<0)
      {
         Flag.BulletFlag = 0;
         Masu[eL[r].x][eL[r].y].pieceInf = Empty;
      }
   }
}

void EnemybulletHit()
{
   if(Flag.BulletFlag == 1)
   {
      if(Masu[eL[r].x][eL[r].y-1].pieceInf == MyMachine)
      {
         lose = 1;
         return;
      }
      else if(eL[r].y-1<0)
      {
         Flag.BulletFlag = 0;
         Masu[eL[r].x][eL[r].y].pieceInf = Empty;
      }
      else if(Masu[eL[r].x][eL[r].y-1].pieceInf == Block)
      {
         Masu[eL[r].x][eL[r].y-1].Block--;
         if(Masu[eL[r].x][eL[r].y-1].Block <= 0)
            Masu[eL[r].x][eL[r].y-1].pieceInf = Empty;
         Flag.BulletFlag = 0;
         Masu[eL[r].x][eL[r].y].pieceInf = Empty;
      }
   }
}
