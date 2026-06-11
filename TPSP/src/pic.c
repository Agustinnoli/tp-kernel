/* ** por compatibilidad se omiten tildes **
================================================================================
 TALLER System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Rutinas del controlador de interrupciones.
*/
#include "pic.h"

#define PIC1_PORT 0x20
#define PIC2_PORT 0xA0

static __inline __attribute__((always_inline)) void outb(uint32_t port,
                                                         uint8_t data) {
  __asm __volatile("outb %0,%w1" : : "a"(data), "d"(port));
}
void pic_finish1(void) { outb(PIC1_PORT, 0x20); }
void pic_finish2(void) {
  outb(PIC1_PORT, 0x20);
  outb(PIC2_PORT, 0x20);
}

// COMPLETAR: implementar pic_reset()
void pic_reset() {
    outb(PIC1_PORT, 0x11);
    outb(PIC1_PORT + 1, 0x20); // base en 32
    outb(PIC1_PORT + 1, 0x04);
    outb(PIC1_PORT + 1, 0x01);
    outb(PIC1_PORT + 1, 0xFF);

    outb(PIC2_PORT, 0x11);
    outb(PIC2_PORT + 1, 0x28); // base en 40
    outb(PIC2_PORT + 1, 0x02);
    outb(PIC2_PORT + 1, 0x01);
    outb(PIC2_PORT + 1, 0xFF);
}

// Opcional 1.a: Duplicar frecuencia de interrupcion de reloj
void set_clock_count(uint32_t cuenta){
    outb(0x40, cuenta & 0xFF); //parte baja
    outb(0x40, cuenta >> 8); //parte alta
}

void pic_enable() {
  outb(PIC1_PORT + 1, 0x00);
  outb(PIC2_PORT + 1, 0x00);
  set_clock_count(0x1000); // setea la cuenta del clock, menores valores producen interrupciones de clock mas seguidas, 0x1000 parece ser una buena frecuencia
}

void pic_disable() {
  outb(PIC1_PORT + 1, 0xFF);
  outb(PIC2_PORT + 1, 0xFF);
}
