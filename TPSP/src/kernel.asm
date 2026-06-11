; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TALLER System Programming - Arquitectura y Organizacion de Computadoras - FCEN
; ==============================================================================

%include "print.mac"

global start


; COMPLETAR - Agreguen declaraciones extern según vayan necesitando
extern GDT_DESC
extern screen_draw_layout

; Interrupciones
extern IDT_DESC
extern idt_init
extern pic_reset
extern pic_enable

; Paginacion
extern mmu_init_kernel_dir
extern copy_page
extern mmu_init_task_dir

; Tareas
extern tss_init
extern tasks_screen_draw
extern sched_init
extern tasks_init

; COMPLETAR - Definan correctamente estas constantes cuando las necesiten
%define CS_RING_0_SEL 08h ; 0000000000001_0_00 -> selector de segmento de codigo nivel kernel (IE ring 0)
%define DS_RING_0_SEL 18h ; 0000000000011_0_00 -> selector de segmento de datos nivel kernel (IE ring 0)  

%define SELECTOR_TAREA_INICIAL 58h
%define SELECTOR_TAREA_IDLE 60h

BITS 16
;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
start_rm_msg db     'Iniciando kernel en Modo Real'
start_rm_len equ    $ - start_rm_msg

start_pm_msg db     'Iniciando kernel en Modo Protegido'
start_pm_len equ    $ - start_pm_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; ==============================
    ; ||  Salto a modo protegido  ||
    ; ==============================

    ; COMPLETAR - Deshabilitar interrupciones (Parte 1: Pasake a modo protegido)
    cli 
    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; COMPLETAR - Imprimir mensaje de bienvenida - MODO REAL (Parte 1: Pasake a modo protegido)
    ; (revisar las funciones definidas en print.mac y los mensajes se encuentran en la
    ; sección de datos)
    print_text_rm start_rm_msg, start_rm_len, 0xF, 0, 0

    ; COMPLETAR - Habilitar A20 (Parte 1: Pasake a modo protegido)
    ; (revisar las funciones definidas en a20.asm)
    call A20_check
    cmp ax, 1
    je .a20enabled
    call A20_enable
    .a20enabled:
    

    ; COMPLETAR - los defines para la GDT en defines.h y las entradas de la GDT en gdt.c
    ; COMPLETAR - Cargar la GDT (Parte 1: Pasake a modo protegido)
    lgdt [GDT_DESC]

    ; COMPLETAR - Setear el bit PE del registro CR0 (Parte 1: Pasake a modo protegido)
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; COMPLETAR - Saltar a modo protegido (far jump) (Parte 1: Pasake a modo protegido)
    ; (recuerden que un far jmp se especifica como jmp CS_selector:address)
    ; Pueden usar la constante CS_RING_0_SEL definida en este archivo
    jmp CS_RING_0_SEL:modo_protegido

BITS 32
modo_protegido:
    ; COMPLETAR (Parte 1: Pasake a modo protegido) - A partir de aca, todo el codigo se va a ejectutar en modo protegido
    ; Establecer selectores de segmentos DS, ES, GS, FS y SS en el segmento de datos de nivel 0
    ; Pueden usar la constante DS_RING_0_SEL definida en este archivo
    mov eax, DS_RING_0_SEL
    mov ds, eax
    mov es, eax
    mov gs, eax
    mov fs, eax
    mov ss, eax

    ; COMPLETAR - Establecer el tope y la base de la pila (Parte 1: Pasake a modo protegido)
    mov esp, 0x25000
    mov ebp, 0x25000

    ; COMPLETAR - Imprimir mensaje de bienvenida - MODO PROTEGIDO (Parte 1: Pasake a modo protegido)
    print_text_pm start_pm_msg, start_pm_len, 0xF, 2, 0

    ; COMPLETAR - Inicializar pantalla (Parte 1: Pasake a modo protegido)
    call screen_draw_layout
    
    ; ===================================
    ; ||     (Parte 3: Paginación)     ||
    ; ===================================

    ; COMPLETAR - los defines para la MMU en defines.h
    ; COMPLETAR - las funciones en mmu.c
    ; COMPLETAR - reemplazar la implementacion de la interrupcion 88 (ver comentarios en isr.asm)
    ; COMPLETAR - La rutina de atención del page fault en isr.asm
    ; COMPLETAR - Inicializar el directorio de paginas
    call mmu_init_kernel_dir
    ; COMPLETAR - Cargar directorio de paginas 
    mov cr3, eax
    ; COMPLETAR - Habilitar paginacion 
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    
    ;prueba de copy_page
    ;xor ebx, ebx
    ;.ciclo:
    ;cmp ebx, 1024
    ;je .salida
    ;mov dword [ebx*4 + 0x20000], ebx
    ;mov dword [ebx*4 + 0x21000], 0
    ;inc ebx
    ;jmp .ciclo
    ;.salida:
    ;mov ebx, 0x20000
    ;mov eax, 0x21000
    ;push ebx
    ;push eax
    ;call copy_page
    ;add esp, 8
    ;mov ebx, 1 ;breakpoint

    ; ========================
    ; ||  (Parte 4: Tareas) ||
    ; ========================

    ; COMPLETAR - reemplazar la implementacion de la interrupcion 88 (ver comentarios en isr.asm)
    ; COMPLETAR - las funciones en tss.c
    ; COMPLETAR - Inicializar tss
    call tss_init

    ; COMPLETAR - Inicializar el scheduler
    call sched_init

    ; COMPLETAR - Inicializar las tareas
    call tasks_init


    ; ===================================
    ; ||   (Parte 2: Interrupciones)   ||
    ; ===================================

    ; COMPLETAR - las funciones en idt.c

    ; COMPLETAR - Inicializar y cargar la IDT
    call idt_init
    lidt [IDT_DESC]

    ; COMPLETAR - Reiniciar y habilitar el controlador de interrupciones (ver pic.c)
    call pic_reset
    call pic_enable
    ;sti

    ; COMPLETAR - Rutinas de atención de reloj, teclado, e interrupciones 88 y 89 (en isr.asm)

    ; COMPLETAR - Habilitar interrupciones (!! en etapas posteriores, evaluar si se debe comentar este código !!)
    
    ; NOTA: Pueden chequear que las interrupciones funcionen forzando a que se
    ;       dispare alguna excepción (lo más sencillo es usar la instrucción
    ;       `int3`)
    ;int3

    ; COMPLETAR - Probar Sys_call (para etapas posteriores, comentar este código)

    ; COMPLETAR - Probar generar una excepción (para etapas posteriores, comentar este código)
    
    ; ========================
    ; ||  (Parte 4: Tareas)  ||
    ; ========================
    
    ;prueba de pagefault handler
    ; COMPLETAR - Inicializar el directorio de paginas de la tarea de prueba

    ;mov ebx, 0x18000
    ;push ebx
    ;call mmu_init_task_dir
    ;add esp, 4

    ; COMPLETAR - Cargar directorio de paginas de la tarea
    ;mov cr3, eax
    ;mov eax, [0x7000000]
    ;mov ebx, 1

    ; COMPLETAR - Restaurar directorio de paginas del kernel

    ; COMPLETAR - Saltar a la primera tarea: Idle
    call tasks_screen_draw
    mov ax, SELECTOR_TAREA_INICIAL ; selector 11:000
    ltr ax
    jmp SELECTOR_TAREA_IDLE:0

    ; Ciclar infinitamente 
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
