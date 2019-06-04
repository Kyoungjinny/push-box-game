#include <ncurses.h>
#include <stdlib.h>
#define N 10
using namespace std;
enum {NONE, WALL, BOX, DEST, SKY, PLAYER};

struct Object{
  int xPosition;
  int yPosition;
  unsigned char rawChar;       // sky = '-', wall = '#'
                               // destination = 'X', box = 'O'
                               // player = 'P'
  chtype outputChar;
};
struct Object obj[N] = {};
int wbox = 0;
int lev = 0;
int push = 0;
int step = 0;

int levWidth(int level){
  int width[4] = {6, 8, 7, 10};
  return width[level];
}

void levList(int *h, int *w, int *array, int y, int x, int n){
  if(n == 0){
    *h = 7;
    *w = 6;
    int map0[7][6] = {
      {1,1,1,1,1,4},
      {1,0,0,0,1,4},
      {1,3,3,3,1,4},
      {1,2,2,2,1,1},
      {1,0,0,0,0,1},
      {1,0,5,0,0,1},
      {1,1,1,1,1,1}};
      *array = map0[y][x];
  }else if(n == 1){
    *h = 6;
    *w = 8;
    int map1[6][8]={
      {1,1,1,1,1,1,1,1},
      {1,3,0,0,0,0,0,1},
      {1,0,3,2,2,2,5,1},
      {1,3,0,0,0,0,0,1},
      {1,1,1,1,1,0,0,1},
      {4,4,4,4,1,1,1,1}};
      *array=map1[y][x];
  }else if(n == 2){
    *h = 8;
    *w = 7;
    int map2[8][7]={
      {1,1,1,1,1,1,1},
      {1,0,0,0,0,0,1},
      {1,0,3,2,3,0,1},
      {1,0,2,3,2,0,1},
      {1,0,3,2,3,0,1},
      {1,0,2,3,2,0,1},
      {1,0,0,5,0,0,1},
      {1,1,1,1,1,1,1}};
      *array=map2[y][x];
  }else if(n == 3){
    *h = 8;
    *w = 10;
    int map3[8][10] = {
      {4,1,1,1,1,4,4,4,4,4},
      {4,1,0,0,1,1,1,1,4,4},
      {4,1,0,0,0,0,0,1,1,4},
      {1,1,0,1,1,0,0,0,1,4},
      {1,3,0,3,1,0,5,2,1,1},
      {1,0,0,0,1,0,2,2,0,1},
      {1,0,0,3,1,0,0,0,0,1},
      {1,1,1,1,1,1,1,1,1,1}};
      *array = map3[y][x];
  }
}

void palette(){
  init_color(COLOR_BLACK, 0, 0, 0);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_WHITE, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_CYAN, COLOR_BLACK);
}

void level(int n){
  clear();
  mvprintw(5,levWidth(lev) + 15,"Level : %d", lev + 1);
  mvprintw(1,1,"Welcome! Use keypad to move.");
  mvprintw(2,1,"Press 'R' to Restart, M' to switch map, 'Q' to Quit.");
  int x = 0, y = 0, h = 1, w = 1, map;
  wbox = 0;
  step = push = 0;

  for(y=0; y<h; y++){
    for(x=0; x<w; x++){
      levList(&h, &w, &map, y, x, n);
      switch(map){
        case NONE : mvaddch(y+5, x+10, '-' | COLOR_PAIR(4)); break;
        case SKY : mvaddch(y+5, x+10, ' ' | COLOR_PAIR(4)); break;  //빈칸, 하늘
        case WALL : mvaddch(y+5, x+10, '#' | COLOR_PAIR(1)); break;  //벽
        case DEST : mvaddch(y+5, x+10, 'X' | COLOR_PAIR(2)); break;  //목적지
        case BOX :
          mvaddch(y+5, x+10, '-' | COLOR_PAIR(4));
          wbox += 1;
          obj[wbox].outputChar = mvinch(y+5, x+10);
          obj[wbox].yPosition = y+5;
          obj[wbox].xPosition = x+10;
          obj[wbox].rawChar = 'O';           //상자
          mvaddch(obj[wbox].yPosition, obj[wbox].xPosition, obj[wbox].rawChar | COLOR_PAIR(5));
          break;
        case PLAYER :
          mvaddch(y+5, x+10, '-' | COLOR_PAIR(4));
          obj[0].outputChar = mvinch(y+5, x+10);
          obj[0].yPosition = y+5;
          obj[0].xPosition = x+10;
          obj[0].rawChar = 'P';                //player
          mvaddch(obj[0].yPosition, obj[0].xPosition, obj[0].rawChar | COLOR_PAIR(3));
        break;
      }
    }
  }
  move(obj[0].yPosition, obj[0].xPosition);
}

void play(int input){
  mvprintw(8,levWidth(lev) + 15,"Steps: %d", step);
  mvprintw(11,levWidth(lev) + 15,"Push: %d", push);
  bool restart = FALSE;

  chtype up, lf, dw, rg, oup, olf, odw, org;
  up = (mvinch(obj[0].yPosition-1, obj[0].xPosition) & A_CHARTEXT);
  lf = (mvinch(obj[0].yPosition, obj[0].xPosition-1) & A_CHARTEXT);
  dw = (mvinch(obj[0].yPosition+1, obj[0].xPosition) & A_CHARTEXT);
  rg = (mvinch(obj[0].yPosition, obj[0].xPosition+1) & A_CHARTEXT);
  oup = (mvinch(obj[0].yPosition-2, obj[0].xPosition) & A_CHARTEXT);
  olf = (mvinch(obj[0].yPosition, obj[0].xPosition-2) & A_CHARTEXT);
  odw = (mvinch(obj[0].yPosition+2, obj[0].xPosition) & A_CHARTEXT);
  org = (mvinch(obj[0].yPosition, obj[0].xPosition+2) & A_CHARTEXT);

  for(int o=0; o<=wbox; o++){
    mvaddch(obj[o].yPosition, obj[o].xPosition, obj[o].outputChar);
  }

  switch(input){
    // 35 = '#', 79 = 'O', 45 = '-', 88 = 'X'
    case KEY_UP:
      if(up != 35){
        if(up == 79 && (oup == 45 || oup == 88)){
          obj[0].yPosition -= 1;
          step++;
          for(int o=1; o<=wbox; o++){
            if((obj[0].yPosition == obj[o].yPosition) && (obj[0].xPosition == obj[o].xPosition)){
              obj[o].yPosition -= 1;
              push++;
            }
          }
        }else if(up != 79){
          obj[0].yPosition -= 1;
          step++;
        }
      }
      break;
    case KEY_DOWN:
      if(dw != 35){
        if(dw == 79 && (odw == 45 || odw == 88)){
          obj[0].yPosition += 1;
          step++;
          for(int o=1; o<=wbox; o++){
            if((obj[0].yPosition == obj[o].yPosition) && (obj[0].xPosition == obj[o].xPosition)){
              obj[o].yPosition += 1;
              push++;
            }
          }
        }else if(dw != 79){
          obj[0].yPosition += 1;
          step++;
        }
      }
      break;
    case KEY_LEFT:
      if(lf != 35){
        if(lf == 79 && (olf == 45 || olf == 88)){
          obj[0].xPosition -= 1;
          step++;
          for(int o=1; o<=wbox; o++){
            if((obj[0].yPosition == obj[o].yPosition) && (obj[0].xPosition == obj[o].xPosition)){
              obj[o].xPosition -= 1;
              push++;
            }
          }
        }else if(lf != 79){
          obj[0].xPosition -= 1;
          step++;
        }
      }
      break;
    case KEY_RIGHT:
      if(rg != 35){
        if(rg == 79 && (org == 45 || org == 88)){
          obj[0].xPosition += 1;
          step++;
          for(int o=1; o<=wbox; o++){
            if((obj[0].yPosition == obj[o].yPosition) && (obj[0].xPosition == obj[o].xPosition)){
              obj[o].xPosition += 1;
              push++;
            }
          }
        }else if(rg != 79){
          obj[0].xPosition += 1;
          step++;
        }
      }
      break;
    case 'm':
    case 'M':
      restart = TRUE;
      if(lev < 3){lev += 1;}
      else{lev = 0;}
      level(lev);
      break;
    case 'r':
    case 'R':
      restart = TRUE;
      level(lev);
      break;
    default:
      break;
  }
  if(!restart){
    for(int o=0; o<=wbox; o++){
      obj[o].outputChar = mvinch(obj[o].yPosition, obj[o].xPosition);
      mvaddch(obj[o].yPosition, obj[o].xPosition, obj[o].rawChar | ((o == 0) ? COLOR_PAIR(3) : COLOR_PAIR(5)));
    }
    move(obj[0].yPosition, obj[0].xPosition);
  }else{
    restart = FALSE;
  }
}

int main(){
  int ch;
  initscr();
  keypad(stdscr, TRUE);
  curs_set(0);
  noecho();

  if(!has_colors()){
    endwin();
    printf("Error initialising colors\n");
    exit(1);
  }

  start_color();

  palette();

  level(lev);

  while((ch = getch()) != 'q'){
    play(ch);
  }
  endwin();
  return 0;
}
