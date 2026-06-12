/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"

#include "kassert.h"

static pd_entry_t* kpd = (pd_entry_t*)KERNEL_PAGE_DIR;
static pt_entry_t* kpt = (pt_entry_t*)KERNEL_PAGE_TABLE_0;

static const uint32_t identity_mapping_end = 0x003FFFFF;
static const uint32_t user_memory_pool_end = 0x02FFFFFF;

static paddr_t next_free_kernel_page = 0x100000;
static paddr_t next_free_user_page = 0x400000;

/**
 * kmemset asigna el valor c a un rango de memoria interpretado
 * como un rango de bytes de largo n que comienza en s
 * @param s es el puntero al comienzo del rango de memoria
 * @param c es el valor a asignar en cada byte de s[0..n-1]
 * @param n es el tamaño en bytes a asignar
 * @return devuelve el puntero al rango modificado (alias de s)
*/
static inline void* kmemset(void* s, int c, size_t n) {
  uint8_t* dst = (uint8_t*)s;
  for (size_t i = 0; i < n; i++) {
    dst[i] = c;
  }
  return dst;
}

/**
 * zero_page limpia el contenido de una página que comienza en addr
 * @param addr es la dirección del comienzo de la página a limpiar
*/
static inline void zero_page(paddr_t addr) {
  kmemset((void*)addr, 0x00, PAGE_SIZE);
}


void mmu_init(void) {}


/**
 * mmu_next_free_kernel_page devuelve la dirección física de la próxima página de kernel disponible. 
 * Las páginas se obtienen en forma incremental, siendo la primera: next_free_kernel_page
 * @return devuelve la dirección de memoria de comienzo de la próxima página libre de kernel
 */
paddr_t mmu_next_free_kernel_page(void) {
    paddr_t next_page = next_free_kernel_page;
    next_free_kernel_page = next_free_kernel_page + PAGE_SIZE;
    
    zero_page(next_page);
    return next_page;
}

/**
 * mmu_next_free_user_page devuelve la dirección de la próxima página de usuarix disponible
 * @return devuelve la dirección de memoria de comienzo de la próxima página libre de usuarix
 */
paddr_t mmu_next_free_user_page(void) {
    paddr_t next_page = next_free_user_page;
    next_free_user_page = next_free_user_page + PAGE_SIZE;
    
    //zero_page(next_page); acceso invalido a memoria -> crashea qemu
    return next_page;
}

/**
 * mmu_init_kernel_dir inicializa las estructuras de paginación vinculadas al kernel y
 *  realiza el identity mapping
 * @return devuelve la dirección de memoria de la página donde se encuentra el directorio
 * de páginas usado por el kernel
 */

paddr_t mmu_init_kernel_dir(void) {
    zero_page((paddr_t) kpd);
    zero_page((paddr_t) kpt);
    
    kpd[0] = (pd_entry_t) {
        .attrs = (MMU_P | MMU_W),
        .pt = (KERNEL_PAGE_TABLE_0 >> 12)
    };

    for (int i = 0; i < 1024; i++) {
        kpt[i] = (pt_entry_t) {
            .attrs = (MMU_P | MMU_W),
            .page = i
        };
    }

    return (paddr_t) kpd;
}



/**
 * mmu_map_page agrega las entradas necesarias a las estructuras de paginación de modo de que
 * la dirección virtual virt se traduzca en la dirección física phy con los atributos definidos en attrs
 * @param cr3 el contenido que se ha de cargar en un registro CR3 al realizar la traducción
 * @param virt la dirección virtual que se ha de traducir en phy
 * @param phy la dirección física que debe ser accedida (dirección de destino)
 * @param attrs los atributos a asignar en la entrada de la tabla de páginas
 */
void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint32_t attrs) {
    
    pd_entry_t *page_directory = (pd_entry_t*) CR3_TO_PAGE_DIR(cr3);
    pd_entry_t directory_entry = page_directory[VIRT_PAGE_DIR(virt)];
    pt_entry_t *page_table;

    if (directory_entry.attrs & MMU_P) {
        page_table = (pt_entry_t*) (directory_entry.pt << 12);
        page_directory[VIRT_PAGE_DIR(virt)] = (pd_entry_t) {
            .attrs = directory_entry.attrs | attrs, //tabla queda con atributos menos restrictivos
            .pt = directory_entry.pt
        };
    } else {
        page_table = (pt_entry_t*) mmu_next_free_kernel_page();
        page_directory[VIRT_PAGE_DIR(virt)] = (pd_entry_t) {
            .attrs = (MMU_P | MMU_W | MMU_U),  //atributos menos restrictivos
            .pt = ((paddr_t) page_table) >> 12
        };
    }

    page_table[VIRT_PAGE_TABLE(virt)] = (pt_entry_t) {
        .attrs = attrs, // me quedo con los attributos mas restrictivos
        .page = phy >> 12
    };

    tlbflush();
}

/**
 * mmu_unmap_page elimina la entrada vinculada a la dirección virt en la tabla de páginas correspondiente
 * @param virt la dirección virtual que se ha de desvincular
 * @return la dirección física de la página desvinculada
 */
paddr_t mmu_unmap_page(uint32_t cr3, vaddr_t virt) {

    pd_entry_t *page_directory =  (pd_entry_t*) CR3_TO_PAGE_DIR(cr3);
    pd_entry_t directory_entry = page_directory[VIRT_PAGE_DIR(virt)];

    if (!(directory_entry.attrs & MMU_P)) {
        return 0; // intento de desmapear una direccion no mapeada -> error
    } else {
        pt_entry_t *page_table = (pt_entry_t*) (directory_entry.pt << 12);

        paddr_t direccion_fisica_desvinculada = page_table[VIRT_PAGE_TABLE(virt)].page << 12;

        page_table[VIRT_PAGE_TABLE(virt)] = (pt_entry_t) {
            .attrs = 0,
            .page = 0 
        };

        tlbflush();
        
        return direccion_fisica_desvinculada;
    }
}

#define DST_VIRT_PAGE 0xA00000
#define SRC_VIRT_PAGE 0xB00000

/**
 * copy_page copia el contenido de la página física localizada en la dirección src_addr a la página física ubicada en dst_addr
 * @param dst_addr la dirección a cuya página queremos copiar el contenido
 * @param src_addr la dirección de la página cuyo contenido queremos copiar
 *
 * Esta función mapea ambas páginas a las direcciones SRC_VIRT_PAGE y DST_VIRT_PAGE, respectivamente, realiza
 * la copia y luego desmapea las páginas. Usar la función rcr3 definida en i386.h para obtener el cr3 actual
 */
void copy_page(paddr_t dst_addr, paddr_t src_addr) {
    
    uint32_t cr3 = rcr3();

    pd_entry_t *page_directory = (pd_entry_t*) CR3_TO_PAGE_DIR(cr3);

    mmu_map_page(cr3, SRC_VIRT_PAGE, src_addr, MMU_P);
    mmu_map_page(cr3, DST_VIRT_PAGE, dst_addr, MMU_P | MMU_W);

    for (int i = 0; i < 1024; i++) {
        ((int *) DST_VIRT_PAGE)[i] = ((int *) SRC_VIRT_PAGE)[i];
    }

    mmu_unmap_page(cr3, SRC_VIRT_PAGE);
    mmu_unmap_page(cr3, DST_VIRT_PAGE);

    //tlblfush()? En principio no
}

 /**
 * mmu_init_task_dir inicializa las estructuras de paginación vinculadas a una tarea cuyo código se encuentra en la dirección phy_start
 * @pararm phy_start es la dirección donde comienzan las dos páginas de código de la tarea asociada a esta llamada
 * @return el contenido que se ha de cargar en un registro CR3 para la tarea asociada a esta llamada
 */
paddr_t mmu_init_task_dir(paddr_t phy_start) {
    
    paddr_t cr3 = mmu_next_free_kernel_page();

    for(int i = 0; i < 1024; i++) {
        mmu_map_page(cr3, i*PAGE_SIZE, i*PAGE_SIZE, (MMU_P | MMU_W)); //identity mapping de area de kernel
    }

    mmu_map_page(cr3, TASK_CODE_VIRTUAL, phy_start, (MMU_P | MMU_U)); 
    mmu_map_page(cr3, TASK_CODE_VIRTUAL + PAGE_SIZE, phy_start + PAGE_SIZE, (MMU_P | MMU_U));
    
    paddr_t pila = mmu_next_free_user_page();
    mmu_map_page(cr3, TASK_STACK_BASE - PAGE_SIZE, pila, (MMU_P | MMU_W | MMU_U));

    mmu_map_page(cr3, TASK_SHARED_PAGE, SHARED, (MMU_P | MMU_U));

    return cr3;
}

// COMPLETAR: devuelve true si se atendió el page fault y puede continuar la ejecución 
// y false si no se pudo atender
bool page_fault_handler(vaddr_t virt) {
  print("Atendiendo page fault...", 0, 0, C_FG_WHITE | C_BG_BLACK);
  // Chequeemos si el acceso fue dentro del area on-demand
  // En caso de que si, mapear la pagina
    if (ON_DEMAND_MEM_START_VIRTUAL <= virt && virt < ON_DEMAND_MEM_END_VIRTUAL) {
        mmu_map_page(rcr3(), ON_DEMAND_MEM_START_VIRTUAL, ON_DEMAND_MEM_START_PHYSICAL, (MMU_P | MMU_W | MMU_U));
        return true;
    } else {
        return false;
    }
}
