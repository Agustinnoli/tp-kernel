# Respuestas a las preguntas

---

## Pasaje a Modo Protegido:

#### 1.1.1) ¿A qué nos referimos con modo real y con modo protegido en un procesador Intel? ¿Qué particularidades tiene cada modo?


Modo Real es el modo en el que empiezan los procesadores de la arquitectura Intel x86 luego de su encendido o activación del PIN #RESET. El procesador trabaja con el conjunto de registros AX, BX, CX, DX, SI, DI, BP, SP, CS, SS, DS, ES, IP y Flags, con un espacio de direccionamiento de 1 MB físico. En este modo solo existen las direcciones de memoria físicas: al acceder a memoria, estamos accediendo directamente a la RAM sin pasar por ningún tipo de estructura, por lo que cada proceso puede acceder a cualquier parte de memoria, pudiendo así interactuar con otros procesos.
Modo Protegido es el modo al que se accede luego de setear el bit CR0.PE en 1, que define el uso de descriptores, privilegios, paginación y demás estructuras con el fin de establecer un esquema de protección de memoria. En particular nos da acceso a los esquemas de segmentación y paginación, y una diferenciación entre memoria lógica (que es la que ve el programa), virtual (que con segmentación flat será igual a la lógica) y física. Los procesos solo pueden acceder a la memoria que les permite estas estructuras mediante un sistema de privilegios que va de 0 (rango máximo, Kernel) a 3 (rango mínimo, Usuario). Además, permite direccionar hasta 4 GiB de memoria lineal.


#### 1.1.2) ¿Por qué debemos hacer el pasaje de modo real a modo protegido? ¿No podríamos simplemente tener un sistema operativo en modo real? ¿Qué desventajas tendría?


Por temas de seguridad, cada tarea solo debería de poder acceder a la memoria que le fue asignada; sin embargo, esto sólo es posible en Modo Protegido ya que, como mencionamos en la respuesta anterior, el Modo Real direcciona directamente a memoria física. Además, Modo Protegido nos provee un sistema de privilegios que impide que las tareas de menor privilegio interactúen con código de mayor privilegio, en particular el Kernel.


#### 1.1.3) ¿Qué es la GDT? ¿Cómo es el formato de un descriptor de segmento, bit a bit? Expliquen brevemente para qué sirven los campos Limit, Base, G, P, DPL, S.


La Global Descriptors Table (GDT) es un lugar de memoria en el que se guardan descriptores de segmento, descriptores de LDT, descriptores de tarea, entre otros. La ubicamos mediante el registro GDTR, el cual posee la dirección física en la que se encuentra, y su longitud.


Un descriptor de segmento es una estructura de 64 bits este tiene el siguiente formato, indexando a partir de sus bits menos significativos:
* 0-15 (Limit): Parte baja del límite del segmento. El límite indica el máximo offset que puede tener un byte direccionable dentro del segmento (Límite = Tamaño - 1).
* 16-39 (Base): Parte baja de la base del segmento. La base indica la dirección a partir de la cual se despliega de forma contínua el segmento. 
* 40 (A): Accessed. Se setea cada vez que se accede a una dirección en el segmento, y permite al SO contabilizar los accesos para elaborar estadísticas de uso que permitan identificar cuál es el segmento a ser desalojado llegado el momento.
* 41-43: Dependiendo del tipo de segmento, estos bits pueden adquirir distintos significados. Cuando se trata de un segmento de código (S=1, 43=1), el bit 42 significa C (Conforming. El segmento ajustará su nivel de privilegio al del código que lo invoque) y el 41 significa R (Readable. Protege el código de lecturas). Cuando se trata de un segmento de datos (S=1, 43=0), el bit 42 significa ED (Expand Down. Para cuando el segmento de datos va a ser usado como pila y queremos que sea explícito, creciendo hacia direcciones numéricamente inferiores), y el bit 41 significa W (Writable. Indica si el segmento podrá escribirse o no).
* 44 (S): Segmento de Sistema o de Código/Datos. Si está en 1, indica un descriptor de código/datos (en este segmento se puede ejecutar código, leer y escribir en memoria); si está en 0, el descriptor es de sistema (el descriptor pasa a ser de otras cosas del sistema, como por ejemplo un descriptor de tareas. En tal caso, los 4 bits anteriores (del 40 al 43) pasan a ser un código donde se definen los 16 tipos de descriptores de sistema que se soportan).
* 45-46 (DPL): Nivel de privilegio del segmento. Solo se va a poder acceder a ese segmento si se tiene un nivel de privilegio mayor o igual al indicado por estos 2 bits, siendo 00 el máximo nivel y 11 el mínimo.
* 47 (P): Present. Indica si el segmento está presente en memoria física o no. Junto con el bit A, dan pie al mecanismo de memoria virtual.
* 48-51 (Limit): Parte alta del límite del segmento.
* 52 (AVL): Available.
* 53 (L): Long. Indica si el segmento es de 64 bits (solo se lo mira en el modo IA-32e).
* 54 (D/B): Default/Big. Indica si el segmento es de 16 o 32 bits.
* 55 (G): Granularity. Sirve para saber si el límite del segmento se va a multiplicar por 4 KiB o no.
* 56-63 (Base): Parte alta del indicador de la base del segmento.


#### 1.1.4) ¿Qué combinación de bits tendríamos que usar si queremos especificar un segmento para ejecución y lectura de código?


Los bits 44-40 deberían de ser 11(0/1)1(0/1). El bit 42 podría estar en 1 o en 0.
S = 1, Bit11 = 1, ED/C = 0/1, W/R = 1, A = 0/1


#### 1.1.6) En el archivo `gdt.h` observen las estructuras: `struct gdt_descriptor_t` y el `struct gdt_entry_t`. ¿Qué creen que contiene la variable `extern gdt_entry_t gdt;` y `extern gdt_descriptor_t GDT_DESC;`?


`gdt` es un array de descriptores que está armado en `gdt.c`. `GDT_DESC` es un struct que tiene la longitud de la gdt y en qué posición de memoria se encuentra. El valor de esta dirección será lo que carguemos en el registro GDTR.


#### 1.1.10) Busquen qué hace la instrucción LGDT en el Volumen 2 del manual de Intel. Expliquen con sus palabras para qué sirve esta instrucción. En el código, ¿qué estructura indica donde está almacenada la dirección desde la cual se carga la GDT y su tamaño? ¿dónde se inicializa en el código?


LGDT lee una posición de memoria que tiene un tamaño de 48 bits,y guarda en GDTR su valor, siendo la dirección base de la gdt los primeros 32 bits, y su longitud los restantes 16. Esta posición de memoria siendo GDT_DESC, este se inicializa en la compilación de gdt.c, que designa a este struct una posición de memoria


#### 1.2.13) Investiguen en el manual de Intel sección 2.5 Control Registers, el registro CR0. ¿Deberíamos modificarlo para pasar a modo protegido?
si, hay que habilitar el bit 0


#### 1.2.15) Miren el volumen 2 de Intel para ver los distintos tipos de JMPs disponibles y piensen cuál sería el formato adecuado. ¿Qué usarían como selector de segmento?

Para terminar el pasaje a modo protegido habría que hacer un jump far al selector de segmento que corresponde al segmento de código de kernel.
este tiene esta estructura
0000000000001_0_00
Este sería el segundo descriptor de la gdt,  los primeros 2 bits indican el nivel de privilegio del segmento, 00 es kernel, y el tercer bit es para saber si es un descriptor de segmento de gdt o ldt, en este caso como es de gst es 0

#### 1.3.21) Declaren un segmento adicional que describa el área de la pantalla en memoria que pueda ser utilizado sólo por el kernel. ¿Qué tamaño deberá tener considerando lo descrito en el párrafo anterior?

Como es un array de 50*80 estructuras de 2 bytes, el límite debería de ser 50*80*2 -1, ya que el límite = 0 cuenta como un byte de límite, esta cuenta da 7999, y esto entra con granularidad = 0 

#### 1.3.22) Observen el método `screen_draw_box` en `screen.c` y la estructura `ca` en `screen.h` . ¿Qué creen que hace el método `screen_draw_box`? ¿Cómo hace para acceder a la pantalla? ¿Qué estructura usa para representar cada carácter de la pantalla y cuanto ocupa en memoria?


Este método imprime en pantalla un cuadrado del mismo carácter con los mismo atributos en pantalla, como tenemos una parte de memoria específica para la pantalla lo que hacemos es escribir en ella los structs que representa cada píxel, estos struct ocupan 2 bytes, y es un byte para el carácter y otro para el color del carácter y el color del fondo del carácter


#### 1.3.24) Resumen final, discutan en el grupo qué pasos tuvieron que hacer para activar el procesador en modo protegido. Repasen todo el código que estuvieron completando y traten de comprenderlo en detalle ¿Qué cosas les parecieron más interesantes?
En resumen: completamos la gdt, la cargamos con lgdt, prendemos el bit 0 de cr0 y realizamos un jump far al segmento de código de nivel 0. 
Lo que nos pareció más interesante es pensar en segmentaciones no flat y cómo se organizaría la memoria de esta manera

---

## Interrupciones:


### Primera parte: definiendo la IDT


#### 2.1.a) Observen que la macro `IDT_ENTRY0` corresponde a cada entrada de la IDT de nivel 0 ¿A qué se refiere cada campo? ¿Qué valores toma el campo offset?




El campo de offset (compuesto por offset_31_16 y offset_15_0) junto con el campo segsel proveen la dirección lógica del código que maneja la interrupción, el campo .
Type indica que el descriptor es una puerta de interrupción y de 32 bits.
DPL indica el nivel de privilegio de la interrupción. En este contexto 11 indicaría que la interrupción puede ser llamada por código de usuario, efectivamente definiendo una interfaz para que el código de usuario pueda llamar código de kernel.
Present indica que el descriptor está presente




#### 2.1.b) Completar los campos de Selector de Segmento (`segsel`) y los atributos (`attr`) de manera que al usarse la macro defina una *Interrupt Gate* de nivel 0. Para el Selector de Segmento, recuerden que la rutina de atención de interrupción es un código que corre en el nivel del kernel. ¿Cuál sería un selector de segmento apropiado acorde a los índices definidos en la `GDT[segsel]`? ¿Y el valor de los atributos si usamos _Gate Size_ de 32 bits?


El selector correspondiente al segmento de código de nivel 0: 0x8
El valor de los atributos serían: Present=1, DPL=00 Type=01110


#### Completar la función idt_init() con las entradas correspondientes a las interrupciones de reloj y teclado ¿Qué macro utilizarían?


IDT_ENTRY0();
IDT_ENTRY3();


### Segunda parte: Rutinas de Atención de Interrupción


#### 2.2.3 ¿Qué oficiaría de prólogo y epílogo de estas rutinas? ¿Qué marca el `iret` y por qué no usamos `ret`?


De prólogo pushad para guardar los registros EAX, EBX, ECX, EDX, EDI, ESI, EBP, ESP en el stack del handler de interrupción. De epílogo popular para restaurar los mismos registros.
Usamos iret para retornar específicamente de una interrupción restaurando la pila y cargando correctamente el EIP, las EFLAGS y el CS


### Opcionales


#### b) ¿Cómo harían para que la impresión del caracter giratorio sea una vez por segundo?


Calculamos la frecuencia de la interrupción de reloj y luego en el handler de interrupción de reloj llamamos next_clock (girar el caracter de clock) solo si la cuenta de ticks (que incrementa con cada clock) es divisible por esta frecuencia


---


## Paginación


#### 3.1.a)  ¿Cuántos niveles de privilegio podemos definir en las estructuras de paginación?


2 niveles: User (Usuario, 1) y Supervisor (Kernel, 0).


#### 3.1.b)  ¿Cómo se traduce una dirección lógica en una dirección física? ¿Cómo participan la dirección lógica, el registro de control `CR3`, el directorio y la tabla de páginas? Recomendación: describan el proceso en pseudocódigo


Partimos de una dirección lógica que pasa por la unidad de segmentación y la traduce en una dirección lineal de 32 bits. La unidad de paginación va a utilizarlos en tres partes: los 10 más significativos son para ubicarnos dentro del directorio (desplazamiento relativo dentro del directorio de tablas de página), los siguientes 10 cumplen la misma función pero ya en una tabla de páginas (desplazamiento relativo dentro de la tabla a la que nos dirija el DPT), y 12 menos significativos son el offset final dentro de la página que encontremos luego de la traducción.


Un DPT ocupa una página, y una página mide 4KB. Esta página va a estar poblada con descriptores de 4 bytes cada uno, así que vamos a tener 1024 (2^10) descriptores en este directorio. Por esta razón solo necesitamos partir la dirección lineal en 3 secciones de las cuales 2 son de 10 bits: una para ubicarnos en el directorio, y otra para ubicarnos en la tabla a la que nos mande el directorio.


El CR3, a su vez, nos marca el comienzo del DPT en sus 20 bits más significativos, ya que los comienzos de página van a estar alineados a 4KB, y por lo tanto los 12 bits menos significativos de dichas direcciones serán 0.


Direción lineal = Dir[10] + Tab[10] + Offset[12]


CR3 = Init[20] + Irrelevante_ahora[12]


Paso 1: Init + Dir*4 será la dirección donde buscaremos el descriptor de la PT. Este descriptor tendrá la siguiente estructura:


PT_Desc = PT_Address[20] + Atributos[12]


Paso 2: Address + Tab*4 será la dirección donde buscaremos el descriptor de la página efectiva. Este descriptor tendrá la siguiente estructura:


P_Desc = P_Address[20] + Atributos[12]


Paso 3: P_Address + Offset será la dirección física a la que llegaremos luego de traducir esta dirección lineal, que a su vez es producto de haber traducido la dirección lógica mediante la Unidad de Segmentación.






#### 3.1.c)  ¿Cuál es el efecto de los siguientes atributos en las entradas de la tabla de página?


 - `D`: Dirty. Indica que la página ha sido modificada (está “sucia”). El Sistema Operativo lo inicializa en 0, y se setea en forma automática en cada escritura en la página. Por este motivo, solo tiene sentido para Descriptores de Página. Esta información es utilizada por el algoritmo de swap, donde si el bit D de la página a desalojar está apagado, entonces no es copiada al disco.
 - `A`: Accessed. Indica si se accedió a memoria controlada por esa Page Table Entry. Se setea cada vez que la página es accedida, y el Sistema Operativo puede contabilizar los accesos para de elaborar estadı́sticas de uso que permitan identificar cuál es la página candidata a ser desalojada cuando sea necesario.
 - `PCD`: Page Cache Disable. Deshabilita cachear los datos de la página asociada.
 - `PWT`: Page Write-Through. Establece el modo de escritura que tendrá la página en el caché. Si la página se va a cachear, PWT nos indica cómo se va a administrar la coherencia de la página (si está seteado en 1, habrá total coherencia entre la caché y la memoria principal, ya que se escribirá en el caché y en la memoria principal a la vez).
 - `U/S`: User/Supervisor. Determina si un proceso en modo usuario puede acceder a la memoria controlada por esta PTE.
 - `R/W`: Readable/Writable. Establece si la página es Read Only (0) o si puede ser escrita (1), o sea, si un proceso puede escribir la memoria controlada por esta PTE.
 - `P`: Present. Indica si la página está en memoria (1), generando una Page Fault cuando se intenta acceder a una dirección de memoria que tiene al menos un descriptor con P=0 a lo largo de la estructura de tablas. Cuando P=0, el resto del contenido del descriptor se ignora.




#### 3.1.d)  ¿Qué sucede si los atributos U/S y R/W del directorio y de la tabla de páginas difieren? ¿Cuáles terminan siendo los atributos de una página determinada en ese caso? Hint: buscar la tabla *Combined Page-Directory and Page-Table Protection* del manual 3 de Intel


En ambos casos gana el criterio más restrictivo: para U/S el nivel de privilegio queda Supervisor, y para R/M dependerá del bit CR0.WP: si es 0 (Write Protect está deshabilitado), queda habilitada la escritura solo para el nivel Kernel, y si es 1, queda de solo lectura para todos los niveles. O sea, el acceso efectivo es siempre el más restrictivo, salvo CR0.WP esté desactivado, en cuyo caso el nivel Kernel puede ignorar la restricción de escritura.


#### 3.1.e)  Suponiendo que el código de la tarea ocupa dos páginas y utilizaremos una página para la pila de la tarea. ¿Cuántas páginas hace falta pedir a la unidad de manejo de memoria para el directorio, tablas de páginas y la memoria de una tarea?


Page Directory: 1 página,


Código: 2 páginas,


Pila: 1 página,


Page Table: 1 página.


Total: 5 páginas.




#### 3.1.g)  ¿Qué es el buffer auxiliar de traducción (*translation lookaside buffer* o **TLB**) y por qué es necesario purgarlo (`tlbflush`) al introducir modificaciones a nuestras estructuras de paginación (directorio, tabla de páginas)? ¿Qué atributos posee cada traducción en la TLB? Al desalojar una entrada determinada de la TLB ¿Se ve afectada la homóloga en la tabla original para algún caso?


El TLB es un buffer de traducciones utilizado para ahorrarnos tiempo de cómputo en la búsqueda de memoria por paginación. A efectos prácticos, cuando tenemos una dirección lineal y queremos encontrar su dirección física, lo único que importa para encontrar la página que la contiene son los primeros 20 bits de la misma (ya que con estos hacemos la búsqueda en la DPT y PT correspondiente). Los 12 bits menos significativos de esa dirección lineal serán el offset a sumar luego de todo ese proceso de traducción, pero hasta ese entonces dos direcciones lineales con el mismo prefijo de 20 bits tendrán el mismo proceso de traducción. Sería ineficiente tener que hacer todo el proceso de nuevo por cada lectura, escritura o fetch que se quiera realizar. Por este motivo la TLB tiene entradas de 52 bits: los 20 del patrón de la dirección lineal, y el PTE correspondiente a la traducción, que contiene los 20 bits de dirección física del inicio de la página, y los 12 bits de atributos (G, PAT, D, A, PCD, PWT, U/S, R/W, P).


Es necesario flushearlo cuando realizamos cambios en nuestras estructuras de paginación porque el procesador primero chequea la TLB, y solo realiza la traducción si no encuentra el tag que estamos solicitando. Si se quedara con la TLB anterior, no tendría efecto nuestra modificación.
No, no se ve afectada en la tabla original al desalojar una entrada de la TLB ya que esta caché no tiene protocolo de coherencia


#### 3.2.a) Recuerden que las entradas del directorio y la tabla deben realizar un mapeo por identidad (las direcciones lineales son iguales a las direcciones físicas) para el rango reservado para el kernel, de 0x00000000 a 0x003FFFFF, como ilustra la figura [2]. Esta función debe inicializar también el directorio de páginas en la dirección 0x25000 y las tablas de páginas según muestra la figura [1] ¿Cuántas entradas del directorio de página hacen falta?


Hace falta una sola entrada ya que se requiere de una sola tabla de páginas para mapear un rango de 4 MB (1024 entradas de 4 KB)


#### 3.3.b) Completen el código de copy_page, ¿por qué es necesario mapear y desmapear las páginas de destino y fuente? ¿Qué función cumplen SRC_VIRT_PAGE y DST_VIRT_PAGE? ¿Por qué es necesario obtener el CR3 con rcr3()?


Es necesario mapear las páginas porque vienen dadas por direcciones físicas y para poder acceder a la memoria se debe pasar por la unidad de paginación. Es decir, para acceder a los contenidos que están en estas direcciones físicas debemos tener direcciones virtuales que mapean estas páginas y las hacen “visibles”. SRC_VIRT_PAGE y DST_VIRT_PAGE actuan de direcciones virtuales reservadas con el propósito específico de ser mapeadas a las direcciones físicas dadas. Es necesario obtener el cr3 ya que esta función puede ser llamada desde cualquier tarea y por lo tanto no es constante, sino dependiente del contexto en que se ejecute


es necesario mapear y desmapearlas porque para hacer una copia de una página de una dirección física a la otra, tenes que accederlas a memoria, por lo tanto estas direcciones físicas hay que mapearlas a unas direcciones virtuales en el page directory de la tarea que se esta ejecutando actualmente, (como cada tarea tiene un cr3 propio, tenemos que llamar a la función para obtenerlo), hay que desmappear estas direcciones físicas por razones de seguridad, porque en principio uno puede mapear 2 direcciones físicas cualquiera, incluidas la de kernel, y no sería conveniente que quedarán mapeadas al page directory de la tarea.
las funciones que cumplen esas direcciones virtuales es que son a donde vamos a mapear las físicas, estas son posiciones de memoria reservadas, ya que si se fueran usadas por las tareas, podemos llegar a mapear/desmapear cosas indeseadas, 

---

## Tareas


#### 4)  Esta va a ser una syscall para que una tarea dibuje en su pantalla ¿Cuál es la convención de pasaje de parámetros para esta syscall?


La convención es que el syscall espera que el parámetro pasado se encuentre en el registro EAX


#### 4.1.1)Si queremos definir un sistema que utilice sólo dos tareas, ¿Qué nuevas estructuras, cantidad de nuevas entradas en las estructuras ya definidas, y registros tenemos que configurar?¿Qué formato tienen? ¿Dónde se encuentran almacenadas?


Si quisiéramos un sistema que utilice solo dos tareas tendríamos que definir tres estructuras nuevas, una TSS para cada tarea y una TSS para la tarea inicial. Luego tres nuevas entradas en la gdt para los descriptores de estas tareas y haría falta configurar el registro TR con el descriptor de la tarea inicial para realizar el primer cambio de contexto


#### 4.1.2)¿A qué llamamos cambio de contexto? ¿Cuándo se produce? ¿Qué efecto tiene sobre los registros del procesador? Expliquen en sus palabras que almacena el registro TR y cómo obtiene la información necesaria para ejecutar una tarea después de un cambio de contexto.


Llamamos cambio de contexto a la iniciación o restauración de los valores de todos los registros (tanto de propósito general como específico) del procesador para iniciar o resumir una tarea. Se produce cuando el scheduler decide que es momento de realizar un cambio de tareas. Los valores de los registros de la tarea siendo pausada se guardan en su TSS y de la TSS de la tarea a resumir se restauran los valores en los registros. El registro TR (Task register) guarda el selector que apunta a la TSS en la gdt de la tarea en ejecución para que al realizar el cambio de contexto se guarden allí los registros de la tarea que vamos a pausar


#### 4.1.3)Al momento de realizar un cambio de contexto el procesador va almacenar el estado actual de acuerdo al selector indicado en el registro TR y ha de restaurar aquel almacenado en la TSS cuyo selector se asigna en el jmp far. ¿Qué consideraciones deberíamos tener para poder realizar el primer cambio de contexto? ¿Y cuáles cuando no tenemos tareas que ejecutar o se encuentran todas suspendidas?


Para realizar el primer cambio de contexto debemos considerar que el estado no lo queremos guardar, ya que es el estado de la ejecución del binario del kernel. Para efectivamente deshacerse de este estado definimos una tarea inicial a la que nunca se volverá la ejecución ya que no figura dentro de la cola del scheduler.
Cuando no tenemos tareas  por ejecutar o están todas suspendidas, cargamos la tarea idle que consiste de un ciclo infinito que no hace nada.


#### 4.1.4)¿Qué hace el scheduler de un Sistema Operativo? ¿A qué nos referimos con que usa una política?


El scheduler es el encargado de decir cuando se debe de realizar un cambio de tarea y a que tarea cambiar. La política del scheduler es la estrategia usada para decidir cuando se realiza el cambio de tarea. En el tp, al haber una cantidad de tareas fijas, se le da la misma cantidad de tiempo de ejecución a cada una, y por cada interrupción de clock salta a la siguiente tarea.


#### 4.1.5)En un sistema de una única CPU, ¿cómo se hace para que los programas parezcan ejecutarse en simultáneo? 


Se itera sobre la ejecución de cada tarea muy rápidamente de modo que el cambio es imperceptible y parece estar ejecutando todas en simultáneo


#### 4.1.9) Utilizando info tss, verifiquen el valor del TR. También, verifiquen los valores de los registros CR3 con creg y de los registros de segmento CS, DS, SS con sreg. ¿Por qué hace falta tener definida la pila de nivel 0 en la tss?


Hace falta definir la pila de nivel 0 en la tss porque cada tarea tiene una pila de nivel 0 distinta.
Esto es necesario ya que en esta pila es donde se guardan los registros propios al contexto de ejecución de una tarea al entrar a un handler de interrupción. Si en este handler de interrupción se realizará un cambio de contexto y la pila fuera compartida al restaurar los registros con popad la tarea iniciada cargaria los valores de la tarea anterior en sus registros.


#### 4.2.11.b) En la línea que dice jmp far [sched_task_offset] ¿De que tamaño es el dato que estaría leyendo desde la memoria? ¿Qué indica cada uno de estos valores? ¿Tiene algún efecto el offset elegido?


Se leen 48 bits de memoria y se interpretan como 16 bits de selector y 32 bits de offset. En este caso se trata del selector de la tarea a resumir y un offset que no tiene ningún efecto ya que se efectúa un cambio de contexto.


#### 4.2.11.c)¿A dónde regresa la ejecución (eip) de una tarea cuando vuelve a ser puesta en ejecución?


Vuelve a la siguiente instrucción de la última que se ejecutó en el contexto de esa tarea. Como en este caso el cambio ocurre dentro del handler de interrupción de clock, se vuelve a esta, se hace el iret y continua la ejecución ya en el código de la siguiente tarea.


#### 4.2.12.a)En los archivos sched.c y sched.h se encuentran definidos los métodos necesarios para el Scheduler. Expliquen cómo funciona el mismo, es decir, cómo decide cuál es la próxima tarea a ejecutar. Pueden encontrarlo en la función sched_next_task.


Itera sobre un arreglo de selectores de tarea hasta encontrar uno que esté marcado como “RUNNABLE” y lo devuelve.


#### 4.3.14.a)¿Qué está haciendo la función tss_gdt_entry_for_task?


Devuelve el descriptor de segmento que iría en la gdt para esa tarea


#### 4.3.14.b)¿Por qué motivo se realiza el desplazamiento a izquierda de gdt_id al pasarlo como parámetro de sched_add_task?


Porque este método recibe un selector de segmento, y en este los primeros 3 bits se utilizan para decidir el privilegio y en qué tabla se encuentra el descriptor. Luego en el resto de bits queda el task id, que debería de ser el índice del descriptor de TSS en la gdt


#### 4.3.15.a) ¿Qué mecanismos usan para comunicarse con el kernel?


Utilizan syscalls


#### 4.3.15.b) ¿Por qué creen que no hay uso de variables globales? ¿Qué pasaría si una tarea intentase escribir en su .data con nuestro sistema?


No hay uso de variables globales ya que en nuestro sistema las tareas no tienen un espacio virtual designado para datos globales a una tarea. Si una tarea intentase escribir en su .data estaría intentando acceder a un espacio de memoria no mapeada, provocando un page fault


#### 4.3.16.a)¿Por qué la tarea termina en un loop infinito?


Porque por como está diseñado nuestro kernel, no hay implementado un sistema de desalojo de tareas. Esto implica que una tarea no tiene manera de finalizar su ejecución propiamente.


#### 4.3.16.b)[Opcional] ¿Qué podríamos hacer para que esto no sea necesario?


Implementar un sistema de desalojo de tareas.
Esto se podría hacer definiendo una syscall reservada para que una tarea llame al momento de finalizar. En esta syscall se podría indicarle al scheduler que quite al selector de su cola para que nunca más se retorne al cambiar entre tareas entre otras cosas para terminar de desalojar la tarea.


#### 4.4.18) Analicen el Makefile provisto. ¿Por qué se definen 2 "tipos" de tareas? ¿Cómo harían para ejecutar una tarea distinta?


Se definen dos tipos de tareas ya que se refieren a dos bloques de código distintos. Para ejecutar una tarea distinta habría que compilarla en el makefile como las demás y en task_init habría que llamar a create task con el código de esa tarea. Habría que definir donde empieza el codigo de esa tarea en task_code_start


#### 4.4.19) Mirando la tarea Pong, ¿En que posición de memoria escribe esta tarea el puntaje que queremos imprimir? ¿Cómo funciona el mecanismo propuesto para compartir datos entre tareas?


Escribe a la direccion:  PAGE_ON_DEMAND_BASE_VADDR + 0xF00 + task_id * 8 que cae en el rango de memoria virtual mapeada a una página on-demand. Este mapeo lo comparten todas las tareas y de este manera implementamos un mecanismo por el cual las tareas comparten datos.


#### 4.4.22) [Opcional] ¿Qué pasa cuando una tarea dispara una excepción? ¿Cómo podría mejorarse la respuesta del sistema ante estos eventos?

Cuando una tarea levanta una excepción nuestro sistema notifica la excepción y se frena completamente.
Para mejorar la respuesta podríamos implementar una manera de manejar las excepciones sin tener que frenar la ejecución de la máquina. Una manera sencilla seria reiniciando la ejecución de la tarea:
Dentro del handler de excepción modificamos en la pila de nivel 0 la EIP para apuntar al comienzo del código de la tarea, la ESP para apuntar a la base de la pila de la tarea y seteamos las EFLAGS. Luego desmapeamos la página on-demand del directorio de la tarea en caso de estar mapeada. De esta manera al retornar la ejecución se comenzará como si fuera nueva.
