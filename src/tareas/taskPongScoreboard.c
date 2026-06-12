#include "task_lib.h"

#define WIDTH TASK_VIEWPORT_WIDTH
#define HEIGHT TASK_VIEWPORT_HEIGHT

#define SHARED_SCORE_BASE_VADDR (PAGE_ON_DEMAND_BASE_VADDR + 0xF00)
#define CANT_PONGS 3


void task(void) {
	screen pantalla;
	// ¿Una tarea debe terminar en nuestro sistema?
	while (true)
	{
	// Completar:
	// - Pueden definir funciones auxiliares para imprimir en pantalla
	// - Pueden usar `task_print`, `task_print_dec`, etc. 
        task_draw_box(pantalla, 0, 0, WIDTH, HEIGHT, ' ', C_BG_BLACK);

        int game0score0 = ((uint32_t*) (SHARED_SCORE_BASE_VADDR + ((uint32_t) 0 * sizeof(uint32_t)*2)))[0];
        int game0score1 = ((uint32_t*) (SHARED_SCORE_BASE_VADDR + ((uint32_t) 0 * sizeof(uint32_t)*2)))[1];

        int game1score0 = ((uint32_t*) (SHARED_SCORE_BASE_VADDR + ((uint32_t) 1 * sizeof(uint32_t)*2)))[0];
        int game1score1 = ((uint32_t*) (SHARED_SCORE_BASE_VADDR + ((uint32_t) 1 * sizeof(uint32_t)*2)))[1];

        int game2score0 = ((uint32_t*) (SHARED_SCORE_BASE_VADDR + ((uint32_t) 2 * sizeof(uint32_t)*2)))[0];
        int game2score1 = ((uint32_t*) (SHARED_SCORE_BASE_VADDR + ((uint32_t) 2 * sizeof(uint32_t)*2)))[1];

        task_print_dec(pantalla, game0score0, 2, WIDTH / 2 - 3, HEIGHT / 2, C_FG_CYAN);
        task_print_dec(pantalla, game0score1, 2, WIDTH / 2 + 3, HEIGHT / 2, C_FG_MAGENTA);
        task_print_dec(pantalla, game1score0, 2, WIDTH / 2 - 3, HEIGHT / 2 + 1, C_FG_CYAN);
        task_print_dec(pantalla, game1score1, 2, WIDTH / 2 + 3, HEIGHT / 2 + 1, C_FG_MAGENTA);
        task_print_dec(pantalla, game2score0, 2, WIDTH / 2 - 3, HEIGHT / 2 + 2, C_FG_CYAN);
        task_print_dec(pantalla, game2score1, 2, WIDTH / 2 + 3, HEIGHT / 2 + 2, C_FG_MAGENTA);
        
		syscall_draw(pantalla);
	}
}
