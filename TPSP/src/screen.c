/* ** por compatibilidad se omiten tildes **
==============================================================================
TALLER System Programming - Arquitectura y Organizacion de Computadoras - FCEN
==============================================================================

  Definicion de funciones de impresion por pantalla.
*/

#include "screen.h"

#include "colors.h"
#include "defines.h"



void print(const char* text, uint32_t x, uint32_t y, uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  int32_t i;
  for (i = 0; text[i] != 0; i++) {
    p[y][x].c = (uint8_t)text[i];
    p[y][x].a = (uint8_t)attr;
    x++;
    if (x == VIDEO_COLS) {
      x = 0;
      y++;
    }
  }
}

void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  uint32_t i;
  uint8_t letras[16] = "0123456789";

  for (i = 0; i < size; i++) {
    uint32_t resto = numero % 10;
    numero = numero / 10;
    p[y][x + size - i - 1].c = letras[resto];
    p[y][x + size - i - 1].a = attr;
  }
}

void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  int32_t i;
  uint8_t hexa[8];
  uint8_t letras[16] = "0123456789ABCDEF";
  hexa[0] = letras[(numero & 0x0000000F) >> 0];
  hexa[1] = letras[(numero & 0x000000F0) >> 4];
  hexa[2] = letras[(numero & 0x00000F00) >> 8];
  hexa[3] = letras[(numero & 0x0000F000) >> 12];
  hexa[4] = letras[(numero & 0x000F0000) >> 16];
  hexa[5] = letras[(numero & 0x00F00000) >> 20];
  hexa[6] = letras[(numero & 0x0F000000) >> 24];
  hexa[7] = letras[(numero & 0xF0000000) >> 28];
  for (i = 0; i < size; i++) {
    p[y][x + size - i - 1].c = hexa[i];
    p[y][x + size - i - 1].a = attr;
  }
}

void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize,
                     uint32_t cSize, uint8_t character, uint8_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  uint32_t f;
  uint32_t c;
  for (f = fInit; f < fInit + fSize; f++) {
    for (c = cInit; c < cInit + cSize; c++) {
      p[f][c].c = character;
      p[f][c].a = attr;
    }
  }
}


void aperture_science(void){

screen_draw_box(4 +0,9+1,1,2, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +1,9+0,4,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +1,9+3,4,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +2,9+1,1,2, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(4 +0,9+5*1+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +0,9+5*1+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +2,9+5*1+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +1,9+5*1+3,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(4 +0,9+5*2+0,1,4, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +2,9+5*2+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +4,9+5*2+0,1,4, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +0,9+5*2+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(4 +0,9+5*3+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +0,9+5*3+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +2,9+5*3+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +1,9+5*3+3,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +3,9+5*3+2,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +4,9+5*3+3,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(4 +0,9+5*4+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +1,9+5*4+1,4,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(4 +0,9+5*5-1+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +0,9+5*5-1+3,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +4,9+5*5-1+0,1,4, 0,C_FG_CYAN +C_BG_LIGHT_GREY);


screen_draw_box(4 +0,9+5*6-1+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +0,9+5*6-1+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +2,9+5*6-1+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +1,9+5*6-1+3,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +3,9+5*6-1+2,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +4,9+5*6-1+3,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);



screen_draw_box(4 +0,9+5*7-1+0,1,4, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +2,9+5*7-1+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +4,9+5*7-1+0,1,4, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(4 +0,9+5*7-1+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

//science
    volatile int i =400000;
    while(i!=0){i--;}

screen_draw_box(11 +0,9+5*0+1,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +1,9+5*0+0,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +2,9+5*0+1,1,2, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +3,9+5*0+3,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +4,9+5*0+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(11 +0,9+5*1+1,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +1,9+5*1+0,3,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +4,9+5*1+1,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(11 +0,9+5*2+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(11 +0,9+5*3-3+0,1,4, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +2,9+5*3-3+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +4,9+5*3-3+0,1,4, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +0,9+5*3-3+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(11 +0,9+5*4-3+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +0,9+5*4-3+3,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +2,9+5*4-3+1,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +3,9+5*4-3+2,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(11 +0,9+5*5-3+1,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +1,9+5*5-3+0,3,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +4,9+5*5-3+1,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(11 +0,9+5*6-3+0,1,4, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +2,9+5*6-3+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +4,9+5*6-3+0,1,4, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(11 +0,9+5*6-3+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

}
void glados(void){
  volatile int i =1000000;
    while(i!=0){i--;}
screen_draw_box(18 +0,9+5*0-0+1,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +1,9+5*0-0+0,3,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +4,9+5*0-0+1,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +2,9+5*0-0+3,3,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +2,9+5*0-0+2,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(18 +0,9+5*1-0+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +4,9+5*1-0+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(18 +0,9+5*2-1+1,1,2, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +1,9+5*2-1+0,4,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +1,9+5*2-1+3,4,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +2,9+5*2-1+1,1,2, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(18 +0,9+5*3-1+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +0,9+5*3-1+0,5,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +4,9+5*3-1+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +1,9+5*3-1+3,3,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
//.os
i =20000000;
    while(i!=0){i--;}
screen_draw_box(18 +3,9+5*4-1+0,2,2, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(18 +0,9+5*5-3+1,1,2, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +4,9+5*5-3+1,1,2, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +1,9+5*5-3+0,3,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +1,9+5*5-3+3,3,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);

screen_draw_box(18 +0,9+5*6-3+1,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +1,9+5*6-3+0,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +2,9+5*6-3+1,1,2, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +3,9+5*6-3+3,1,1, 0,C_FG_CYAN +C_BG_LIGHT_GREY);
screen_draw_box(18 +4,9+5*6-3+0,1,3, 0,C_FG_CYAN +C_BG_LIGHT_GREY);



}




void aperture_logo(void){

    volatile int i =400000;
    while(i!=0){i--;}
    screen_draw_box(1,52+9,1,7, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(2,52+10,1,7, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(3,52+11,1,6, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(4,52+12,1,5, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(5,52+13,2,4, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(7,52+15,2,2, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(9,52+16,1,1, 0,C_FG_CYAN +C_BG_CYAN);

    i =80000000;
    while(i!=0){i--;}


    screen_draw_box(3,52+18,10,2, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(4,52+20,8,2, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(5,52+22,6,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(7,52+23,3,1, 0,C_FG_CYAN +C_BG_CYAN);
i =80000000;
    while(i!=0){i--;}
    screen_draw_box(11,52+23,7,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(12,52+22,6,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(13,52+20,5,2, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(14,52+19,4,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(15,52+18,3,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(16,52+17,2,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(17,52+16,1,1, 0,C_FG_CYAN +C_BG_CYAN);
    //screen_draw_box(1,58,1,9, 32,C_FG_GREEN +C_BG_GREEN);
i =80000000;
    while(i!=0){i--;}

    screen_draw_box(19,52+12,1,11, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(20,52+13,1,10, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(21,52+14,1,8, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(22,52+15,1,5, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(23,52+16,2,2, 0,C_FG_CYAN +C_BG_CYAN);
i =80000000;
    while(i!=0){i--;}
    screen_draw_box(17,52+8,8,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(18,52+9,7,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(19,52+10,6,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(20,52+11,5,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(21,52+12,4,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(22,52+13,3,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(23,52+14,2,1, 0,C_FG_CYAN +C_BG_CYAN);


i =80000000;
    while(i!=0){i--;}

    screen_draw_box(13,52+5,10,2, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(14,52+4,8,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(15,52+3,7,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(16,52+2,5,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(17,52+1,2,1, 0,C_FG_CYAN +C_BG_CYAN);

i =80000000;
    while(i!=0){i--;}
    screen_draw_box(8,52+1,2,8, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(10,52+0,1,8, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(11,52+0,1,7, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(12,52+0,1,6, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(13,52+0,1,3, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(14,52+0,1,2, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(15,52+0,1,1, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(16,52+0,1,1, 0,C_FG_CYAN +C_BG_CYAN);
    i =80000000;
    while(i!=0){i--;}

    screen_draw_box(5,52+2,2,10, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(4,52+3,1,8, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(3,52+5,1,5, 0,C_FG_CYAN +C_BG_CYAN);
    screen_draw_box(2,52+7,1,2, 0,C_FG_CYAN +C_BG_CYAN);


  } 



char *Welcome = "Welcome to Glad.OS Aperture's operating system.";
char *dead = "Yesterday's dead test subjects are:";
char *Agus = " -Agustin Noli              LU := 649/24";
char *Santi= " -Santiago Roussineau       LU := 757/23";
char *Mateo= " -Mateo Gallia              LU := 321/24";
char *Tasks = "Today's tests will be Snake and Pong.";
char *ThisIsNotALie = "Today's reward will be cake.";
char *Continue = "Deploying new test subjects.";
char *Testing = "Testing starts in:";
char *tres = "3...";
char *dos = "2...";
char *uno = "1";

void texto(void){

volatile int i =200000000;
    while(i!=0){i--;}
    screen_draw_box(SIZE_N/2+ 9, 0, 1, SIZE_M, 205,  C_FG_CYAN );

    //screen_draw_box(SIZE_N/2+ 9, SIZE_M/2+7, 16, 1, 186,  C_FG_CYAN );
    //screen_draw_box(SIZE_N/2+ 9, SIZE_M/2+7, 1, 1, 203,  C_FG_CYAN );

    i =200000000;
    while(i!=0){i--;}
    print(Welcome,0,SIZE_N/2+ 10, C_FG_CYAN );

    i =200000000;
    while(i!=0){i--;}
    print(dead,0,SIZE_N/2+ 11, C_FG_CYAN );
    print(Mateo,0,SIZE_N/2+ 12, C_FG_RED );
    print(Agus,0,SIZE_N/2+ 13, C_FG_RED );
    print(Santi,0,SIZE_N/2+ 14, C_FG_RED );
    i =200000000;
    while(i!=0){i--;}
    print(Tasks,0,SIZE_N/2+ 15, C_FG_CYAN );
    
    i =200000000;
    while(i!=0){i--;}
    print(ThisIsNotALie,0,SIZE_N/2+ 16, C_FG_CYAN );
    i =200000000;
    while(i!=0){i--;}
    print(Continue,0,SIZE_N/2+ 17, C_FG_CYAN );
    i =200000000;
    while(i!=0){i--;}
    print(Testing,0,SIZE_N/2+ 18, C_FG_CYAN );
    i =200000000;
    while(i!=0){i--;}
    print(tres,0,SIZE_N/2+ 20, C_FG_WHITE );
    i =200000000;
    while(i!=0){i--;}
    print(dos,0,SIZE_N/2+ 22, C_FG_WHITE );
    i =200000000;
    while(i!=0){i--;}
    print(uno,0,SIZE_N/2+ 24, C_FG_RED );
    i =200000000;
    while(i!=0){i--;}
    


}



void screen_draw_layout(void) {
    /*
   screen_draw_box(0, 0, SIZE_N, SIZE_M, ' ', C_FG_CYAN + C_BG_CYAN);
    ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
   
    print(Agus, 0, 0, C_FG_BLACK + C_BG_CYAN);
    print(Mateo, 0, 1, C_FG_BLACK + C_BG_CYAN);
    print(Santi, 0, 2, C_FG_BLACK + C_BG_CYAN);
    */
    // espero que los tiempos de espera de los while no sea afectado porque que procesador tiene cada computadora
    // que quemu tenga un clock fijo para todas las pc 

    screen_draw_box(0, 0, SIZE_N, SIZE_M, 0, 0 );

    aperture_science();
    aperture_logo();
    glados();
    texto();   

}
