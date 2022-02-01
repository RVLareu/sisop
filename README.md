# Sistemas operativos 2021 - 2c - Mendez

### labFork

primer lab con ejercicio de fork y pipe

# Parcialito 1: procesos y memoria

### Procesos y Kernel

1. Escriba un programa que permita determinar cómo son los tiempos de ejecución entre una system call (getpid() por ejemplo ) y un library call, que no dependa de syscall internamente. Utilice gettimeofday() para realizar la medición. A su criterio y sin ejecutar el mismo cuáles deberían ser los resultados, justifique.

```

#include <sys/time.h> //getTimeofday
#include <stdlib.h> //abs
int main(int argc, char* argv[]) {
    struct timeval tisys, tfsys;
    struct timeval tilib, tflib;
    int n = 1;
    
    //tiempo para syscall
    gettimeofday(&tisys, NULL);//arranco a contar
    for (int i = 0; i < 1000; i++) {
        getpid() //syscall
    }    
    gettimeofday(&tfsys, NULL)//termino de contar
    int tsys = tfsys.tv_usec - tisys.tv_usec; // tiempo total
    printf("%d para system call", tsys);
    
    //tiempo para library call
    gettimeofday(&tylib, NULL); //arranco a contar
    for (int i = 0; i < 1000; i++) {
        abs(1);//library call (asumo no wrappea una syscall)
    }
    gettimeofday(&tflib, NULL);//termino de contar
    int tlib = tflib.tv_usec - tilib_tv_usec; // tiempo toal
    printf("%d para library call", tlib);
    
    return 0; 
}


```

El llamado a systemCall implica un context switch, lleva mucho más tiempo. Va a tardar más la syscall. Cuando se 
hace una library call, los recursos del proceso se mantienen. Al hacer la syscall deben salvarse para poder hacer el context
switch y pasar a modo kernel, que es lo que implica la realización de una syscall. El for es para asegurarse de que 
la diferencia entre tiempos se pueda ver





2. ¿Qué son los modos de transferencia? Ponga un ejemplo lo más concreto posible de cada uno.

Hace referencia a la forma en que se puede pasar de modo usuario a modo kernel. Existen 3 maneras de pasar de
modo usuario a modo kernel:
    -Interrupción: señal es asincrónica, corresponde a evento externo. Por ejemplo de algun componente de E/S, como
    puede ser un network Device indicando la llegada de un paquete
    -Excepción: igual a interrupción pero es de hardware generado por un programa usuario (entonces son sincrónicas),
    un ejemplo sería dividir por cero en un programa
    -ejecutando una SystemCall: en el tp cuando hacemos la llamada a fork() (systemCall proporcionada por el kernel),
    permite desde modo usuario indicarle al kernel que realice una operacion, en este caso crear un proceso hijo duplicando al proceso padre.
    Asi se pasa a modo kernel, se crea el hijo y se retoma luego modo usuario
A su vez hay formas de pasar de modo kernel a usuario:
    - Al volver de una system call (hago getpid), se restaura los registros del proceso que se llamó y se retoma su
    ejecución en modo usuario
    -Al intercambiar la ejecucion de procesos
    - El kernel es el encargado de indicar la ejecución de un proceso, para lo que es necesario que lo cargue en memoria, 
    prepare el contexto  para su ejecución y switchee a modo usuario para que se ejecute
    
3. Que mecanismo concreto de hardware permite que este programa no capture el procesador para sí mismo (describa como funciona):

```
#include<stdio.h>
int main (){
 while(1){
        printf(“I have the CPU\n”)
    }
}
```

El mecanismo es el hardware counter, determino un tiempo de manera arbitraria pasado el cual el proceso que
está corriendo en modo usuario le pasa el control al kernel (modo kernel). De esta manera, el kernel se asegura
que siempre cada x cantidad de tiempo va a poder hacer uso del procesador. Esto entra en el marco de limitar la
ejecución directa de un programa.
Al ocurrir un context switch, el kernel debe salvar todos los registros del actual proceso, hacer un flush de la TLB (o no si se puede 
identificar cada proceso con un ID) y setear un time slice para cortar la ejecucion del nuevo proceso. De esto se encarga el Scheduler.

### Memoria Virtual

Se tiene un espacio de direcciones virtuales de 6-bits (64 bytes); que está mapeado utilizando segmentación a un espacio de direcciones físicas de 8-bits (256 bytes). Se utilizan dos segmentos para traducir las direcciones: cs para traducir instrucciones y ds para traducir datos. Ambos segmentos crecen de forma normal, y tienen 16-bits: 8 para guardar la base, 6 para guardar el tamaño y 2 para flags varios (sin especificar).

1. ¿Cuál es el límite teórico de procesos que pueden estar cargados en memoria a la vez con su espacio de direcciones entero? Considerar al kernel como un proceso más, y no permitir solapamiento entre los segmentos.

2. Simular la traducción de las siguientes direcciones (C para code y D para data). Indicar, para cada una, si resulta en segmentation fault o bien a qué dirección física resuelve.

CS: base=0x32 (dec 50) limit=0x3c (dec 60) DS: base=0x64 (dec 100) limit=0x20 (dec 32)

1. C 0x08 (dec 8) = ??

2. C 0x1e (dec 30) = ??

3. C 0x3e (dec 62) = ??

4. D 0x00 (dec 0) = ??

5. D 0x0c (dec 12) = ??

6. D 0x28 (dec 40) = ??

7. Indicar el rango de direcciones físicas que son accesibles desde ambos segmentos. Incluir además qué direcciones virtuales (para cada segmento) traducen a tales direcciones físicas).

Ayuda: recordar que una misma dirección virtual x podría traducirse como instrucción (usando cs) o como datos (usando ds)

Resolucion:

CS: base = 0x32 (50)
    bound = 0x3c (60)
    
    PA = VA + base
    
    1. 0x08 (8) ->  >bound? NO -> 0x08 + 0x32 -> 58 (dec) direccion fisica
    2. 0x1e (30) ->  >bound? NO -> 0x1e + 0x32 -> 80 (dec) direccion fisica
    3. 0x3e (62) ->  >bound? SI -> Raise exception
    
    direccion virtual mínima es 0 (0x00) y máxima 60 (0x3c), entonces los límites en la física serán 50 (dec) - 110 (dec)
    
D:  base = 0x64 (100)
    bound = 0x20 (32)
    
    PA = VA + base
    
    4.0x00 (0) -> >bound? NO -> 0x00 + 0x64 = 100 (dec) direccion fisica
    5.0x0c (12) -> >bound? NO -> 0x0c + 0x20 = 112 (dec) direccion fisica
    6.0x28 (40) -> >bound? SI -> Raise exception
    
    dirección virtual mínima es 0 (0x00) y máxima 32 (0x20), entonces los límites en la física serán 100 (dec) - 132 (dec)
    
    desde ambos puedo acceder a 100-110. Las virtuales para eso serán:
        desde CS: 0x32 (50) -> 50 + base = 100
                    0x3c (60) -> 60 + base = 110
        desde D: 0x00 (0) -> 0 + base = 100
                0xA (10) -> 10 + base = 110
                
                
El límite teórico sería de tal manera que entren 256 / 64 = 4 procesos con su espacio completo. Si el kernel 
tambien ocupa entonces quedaria lugar para 3 procesos más


# Segundo Parcialito: Schedulling y concurrencia

### Scheduling y Memoria Virtual
a. Se tiene el siguiente esquema de procesos:

Proc	Duración
P1	69
P2	17
P3	68
P4	8
P5	17
P6	24
Q1 = RR (quantum=15

Q2 = RR (quantum=25)

Q3 = SJFS

El time arrival de los procesos es 0 para todo, no hay Boost time, se pide calcular utilizando MLFC :

* Completion time: tiempo t en el que se completo el proceso.
* Turnaround time (CT-AT)
* Waiting time( TAT - duracion)
b. Elija dos políticas de planificación y comparelas con un ejemplo concreto. explicando cual es mejor

a)

MLFQ:

Asumo SJFS es SJF y no STCF

Asumo que si un proceso termina antes de su time slice, el scheduler ejecuta en la cola de maxima prioridad
el proceso siguiente, reiniciando el time slice si fuera que es la cola actual


Un proceso llega y va a entrar en Q1. Llegan todos al mismo tiempo asi que elijo que empiece P1.
P1 se ejecuta por un time slice de 15 y como lo consume entero baja de prioridad (P1 = 69 - 15 = 54). Tomo P2
en Q1, se ejecuta, consume su time slice y baja a Q2 (P2 = 17 - 15 = 2). Recordemos que la cola con mayor prioridad
es Q1 entonces ahora corro P3, que luego de un time slice baja a Q2 (P3 = 68 - 15 = 53). Luego P4 llega y termina
luego de 8 instantes de tiempo, se ejecuta P5 en Q1 y baja a Q2 luego de completar el time slice (P5 = 17 - 15 = 2).
Luego P6 corre en Q1 y baja luego de completar el time slice (P6 = 24 - 15 = 9).

Finalizaó P4

Ahora en Q2 corre P1, que luego de completar su time slice baja a Q3 (P1 = 54 - 25 = 29). Ahora corre P2, que
finaliza luego de 2 instantes de tiempo. Corre P3 y baja luego de un time slice (P3 = 53 - 25 = 28). Corre P5 y termina
luego de 2 instantes de tiempo. Corre P6 y finaliza luego de 9 instantes de tiempo.

Finalizaron P2, P5, P6

Finalmente los procesos que quedaron en la cola de menor prioridad van a correr de acuerdo a SJF. En este caso
el primer proceso en llegar fue P1, que termina luego de 29 instantes de tiempo. Luego lo hace P3 que finaliza luego de
28 instantes y terminan de correr todos los procesos.

Finalizaron P1 y P3

Todos arriban en 0.

Completion times:

P1: 15 + 15 + 15 + 8 + 15 + 15 (hasta acá Q1) + 25 + 2 + 25 + 2 + 9 (hasta acá Q2) + 29  = 175 -> finaliza
P2: 15 + 15 + 15 + 8 + 15 + 15 (hasta acá Q1) + 25 + 2 = 110 -> finaliza
P3: 15 + 15 + 15 + 8 + 15 + 15 (hasta acá Q1) + 25 + 2 + 25 + 2 + 9 (hasta acá Q2) + 29 + 28 = 203 -> finaliza
P4: 15 + 15 + 15 + 8 = 53 -> finaliza
P5: 15 + 15 + 15 + 8 + 15 + 15 (hasta acá Q1) + 25 + 2 + 25 + 2 = 137 -> finaliza
P6: 15 + 15 + 15 + 8 + 15 + 15 (hasta acá Q1 ) + 25 + 2 + 25 + 2 + 9  = 146 -> finaliza

Turnaround times = completion time - arrival time = completion time

Waiting time = turnaround time - duracion = completion time - duracion

P1: 175 - 69 = 106
P2: 110 - 17 = 93
P3: 203 - 68 = 135
P4: 53 - 8 = 45
P5: 137 - 17 = 120
P6: 146 - 24 = 122


Por las dudas, si la última fuera STCF si al llegar P3, el tiempo restante de P1 fuera mayor al de P3, se correria
P3 y la ultima en terminar seria P1.

b) SJF vs STCF

El problema que tiene SJF es que es non-preemptive, por lo que una vez iniciada la corrida
de un proceso, no va a cortarla. 

Tomando de ejemplo los procesos y duracion del punto 1.

En un SJF correrían en el siguiente orden: P4, P2, P5, P6, P3, P1.
Lo mismo ocurriría en STFC. La ventaja del segundo se ve si en vez de que arribaran al mismo tiempo, lo hicieran
con un instante de delay (P1 en 0, P2 en 1, P3 en 2, etc).
En SJF el orden se mantendría, mientras que en STFC, al llegar P2 este cortaria la corrida de P1 y lo reemplazaria.
P3 no reemplazaría a P2, pero P4 si lo haría, finalizando P4 y retomando luego P2 para luego ir por P5, y en cada instante
corriendo el que vaya a terminar antes.

STFC viene a mejorar el tiempo de respuesta promedio. Esto se ve facil en el caso de P2, que en SJF debe esperar a que
P1 finalice, mientras que en STCF al llegar correrá inmediatamente, de esta manera los tiempos de respuesta de P1 y P2
serán cero (estoy en el caso del delay de 1 instante).
STCF es mejor porque básicamente es SJF, pero incluye el caso en que hay un proceso largo corriendo y llega uno corto, 
mejorando el tiempo de respuesta.

### Concurrencia

a. Dado la siguiente estructura de datos, implemente la función insertar y ls funcion crear, de forma tal que sea thread safe:

```

int List_Insert(list_t *L, int key) {

 node_t *new = malloc(sizeof(node_t));
 if (new == NULL) {
     perror("malloc");
     return -1; // fail
 }
 new->key = key;
 new->next = L->head;
 L->head= new;
 return 0; // success
}
// nodo
typedef struct __node_t {
 int key;
 struct __node_t *next;
} node_t;
// lista
typedef struct __list_t {
 node_t *head;
} list_t;

```

b. Explique detalladamente que es un thread. Cuales son sus componentes. Que metadata se debe almacenar. Cual es su relacion con el paralelismo.

a) 

La insercion de un nodo en una lista es una region crítica para la concurrencia, porque las referencias
podrían quedar mal. Puedo hacer uso de un mutex para garantizar la atomicidad (que se ejecute como una sola
instruccion de bajo nivel algo que involucra más de una) de las acciones que se consideran de riesgo.

En primer lugar le agregaría un mutex a la lista:

```
#include <pthread.h>

typedef struct __list_t {
    node_t *head;
    pthread_mutex_t lock;
} list_t;

```

Entonces al crear/iniciar la lista voy a inicializar el mutex

```

void init(list_t* list) {
    
    pthread_mutex_init(&list->lock, NULL);
}

```

Cuando voy a realizar la inserción, hago lock (pongo el candado para que no entre otro hilo mientras estoy) y 
cuando termino hago unlock (lo dejo para que otro lo agarre)

```

int list_insert(list_t* list, int key) {
    node_t* new = malloc(sizeof(node_t));
    if (new == NULL) {
        perror("malloc");
        return -1;
    } 
    new->key = key;
    pthread_mutex_lock(&list->lock);
    new->next = list->head;
    list->head = new;
    pthread_mutex_unlock(&list->lock);
    return 0;
}


```

De esta manera, la sección crítica en la que se resignan las referencias no podrá ser interrumpida por otro 
hilo de ejecucion, y se ejecutará de manera atómica.


b) Del material de la cátedra es una secuencia de ejecución atómica que representa una tarea planificable de
ejecución, es decir unn thread es un conjunto de instrucciones ejecutandose donde el sistema operativo puede
decidir donde cortar y seguir cuando quiera. En un programa sin concurrencia, hay un hilo de ejecucion principal
que son las instrucciones del programa que están corriendo. La utilidad aparece cuando hay múltiples hilos que permiten
realizar varias tareas a la vez (o en paralelo en caso de haber múltiples cores, aunque acá entra también el
tema del manejo de la cache para que se pueda aprovechar al máximo y no se pierda si se cambia de core).
Cada vez que se crea un thread, el TCB almaacena el estado Per-thread del mismo. Para poder cortar y retomar
debe entonces poder tener un puntero al stack del thread y sus registros en el procesador (como un context switch en
un proceso, debo guardar el estado para poder volver y retomar como estaba). Por cada thread tengo que tener el 
identificador del mismo, la prioridad (esto le sirve al planificador) y el status. A su vez los threads comparten
un estado que debe ser guardado, que estará compuesto por el código en si, variables globales y el heap.

Lo que permiten los hilos es aprovechar mejor el rendimiento del procesador aprovechando el paralelismo. Aunque 
tenga muchos procesadores, si no lanzo hilos el programa se ejecutará secuencialmente. Si tuviera un solo thread,
durante acciones de I/O el procesador quedaría ocioso, si en cambio tuviera más threads, se podrían aprovechar
esos momentos. El thread en I/O liberaría el procesador, permitiendole a otro thread hacer uso del mismo mientras el
primero está en I/O. El intercambio es un context switch.


# Tercer Parcialito: FileSystem

### File System

Sea un disco que posee 128 bloques de 4kb y un sistema operativo cuyos i-nodos son de 128 bytes. Defina la estructura completa del sistema de archivos unix-like . Justificar cada elección.

 128 bloques de 4 KB
 Inodos de 128 Bytes
 
 Asumo se pide un VSFS
 
Entonces el sistema de archivos va a ser una partición de 128 bloques (entonces van del 0 al 127) con un tamaño
de 128 * 4 KB = 512 KB.

Si los inodos ocupan 128 bytes, entonces en un bloque de 4 KB = 4096 bytes entran 32 inodos.

La mayor parte del file system va a estar destinada a data del usuario (data region). Entonces de los 128 bloques voy
destinar 118 para la región de datos de usuario (algo así como el 90%).
Además el file system debe almacenar y llevar cuenta de cada archivo (metadata: tamaño, derechos, tiempos de acceso,
creacion, modificacion, propietario, etc) -> Para esto usamos los inodos, que ocupan espacio en el disco (tabla de inodos,
que no es más que un array de inodos).
Tenía 128, destiné 118 para data de usuario, me quedan 10. De esos, 7 serán para INODOS. De esta manera, si cada
bloque podia contener 32 inodos voy a poder tener 32 * 7 = 224 inodos o archivos en mi file system. Esto 
da un ratio de 1 inodo : 2.27kb (Depende el uso que de el usuario, si fueran todos archivos txt podría estar bien elegido
el tamaño).
Ahora el bitmap (uno por cada region: inodo y data). -> Si tengo bloques de 4KB puedo llevar cuenta de hasta 32000
archivos/inodos, pero tomo como minima unidad de medida al bloque para simplificar el file system.
Hasta ahora la region de datos iba del bloque [10-127]. La de inodos de [3-9]. bitmap de data el [2] y 
bitmap de inodos el [1]
Finalmente el superbloque irá en el bloque[0]. Aca va a ir la información correspondiente al file system
en sí: cantidad de inodos = 224, cantidad de bloques de data = 118, inicio de la tabla de inodos = [3].

#### Corrección: 

yo aclare que tenes que suponer que direccionas a 256 bloque por ende 256 inodos / X inodos por bloque = YY inodos. justamente comente que este era el error clasico de este ejercicio.

#### Ejercicio 2

File System
Se tiene un file system basado en i-nodos con la siguientes características:

Los bloques son de 1 kiB (1024 bytes) [2^10]
El tamaño de un i-nodo es de 64 bytes [2^6]
La distribución de los bloques es:
El bloque 0 es el boot_block
El bloque 1 es el superblock
El bloque 2 es el i-node bitmap
El bloque 3 es el block-bitmap
Hay 126 bloques dedicados a la i-node table
Hay 128 bloques dedicados a datos
Dada la siguiente información de la tabla de i-nodos y el contenido de los bloques de datos, indicar:

¿Qué se mostraría en pantalla o que equivale ejecutar ls /home/mariano , ls /home/juan y ls /mnt? Indicar la secuencia de operaciones (lecturas de bloque blkrd indicando la numeración relativa a la sección de i-nodos o datos; y la numeración dentro del sistema entero), que se realizan para acceder al archivo /home/dato/start.sh. Indicar para cada bloque leído qué información contiene y qué parte resulta relevante.
¿Hay algún archivo que tenga más de una referencia (hard link)? ¿Qué syscall o comando unix usaría para borrar este tipo de archivos?
I-node entry	Block ptr				data block #	content
0	1				0	mariano:10, dato:11, juan:12
1	home:5, mnt:4, bin:3
3	5				5	ls:40, cat:41
4	6				6	cdrom:33
5	0				7	apuntes.md:101, start.sh:32
9	jos.c.md:104, start.c.:34
10	9				11	
11	9				43	
12	11					
40	45					
41	57					
32	111					
33	43					
34	44					
101	52					
102	53					
104	54					
Nota: la primer columna de la tabla de i-nodos refiere al número de i-nodo, mientras que la primer columna de la tabla de bloques hace referencia a la numeración de bloques relativa al los bloques de datos. Ayuda: recordar que el i-nodo 0 es siempre el directorio raíz.

#### Resolución

Bloques de 1 KB
Inodos de 64 Bytes => 16 inodos por bloque

[0] boot block
[1] super block
[2] inode bitmap
[3] block bitmap

[4,129] : 126 bloques para la tabla de inodos => 16 inodos por bloque => 16 * 126 = 2016 inodos
Inode start = 5 kb
[130, 257] : 128 data region

blk ptr es el bloque de datos al que apunta el inodo (relativo al bloque de datos, asumo que blk ptr = n => #data 
block = n)
#data block es el bloque y en la columna de la derecha su contenido

En el superbloque voy a tener toda la info correspondiente al file system, entonces si me dan el nro
de inodo de el SB voy a poder saber donde empieza la tabla de inodos. Conociendo eso, el nro de inodo y 
el tamaño del mismo voy a poder calcular el offset dentro de la tabla. 

boot  S   IB   BB     IT         DR
[0]  [1]  [2]  [3]  [4,129]  [130,257]

ls /home/mariano



home : 5 => inode number = 5 => offset desde el arranque de inode region = 5 * 64, #blk de tabla 
de inodo = 0 (entran 16 por bloque)=> block ptr = 0 =>
=> offset dentro de data region = 0 * 1kb => # data block = 0
Alli tomo mariano : 10 => inode number = 10 => offset en tabla inodo = 10 * 64 bytes, #blk de tabla de inodo=0 =>
blk ptr = 9 => #data block = 9, offset en data region = 9 * 1kb=> contiene jos.c.md y start.c

Los bloques absolutos accedidos dentro del sistema para la secuencia entero fueron: 
4 (inode number 5),
130 (0 en data region)
4 (inode number = 10)
130 + 9 = 139 (9 data region)

ls /home/juan

home es igual a arriba

juan : 12 => inode number = 12 => offset dentro de tabla de inodo = 12 * 64, # blk de tabla de inodo = 0 => 
=> blk ptr = 11 => # data block es igual 11 => offset data region = 11 * 1kb => no contiene nada

Bloques absolutos accedidos dentro del sistema fueron:
4 (inode number 12),
130 (0 data region)
4 (12 inode number),
130 + 11 = 141 en data region (#data blk = 11)

ls /mnt 

mnt : 4 => inode number = 4 => off en inode tabble = 4 * 64, # blk de tabla de inodo = 0 => blk ptr = 9 => 
#data block = 9 => contiene jos.c.md y start.c

bloques absolutos accedidos dentro del sistema son:
4 (inode number 4),
130 + 9 = 139 (9 data region)

/home/dato/start.sh

home : 5 => inodo 5 => blk ptr = 0 => # data blk = 0 => mariano:10 dato:11 juan:12 (busco dato, otro directorio (al final es un archivo tambien))
dato : 11 => inodo 11 => blk ptr = 9 => # data blk = 9 => tiene jos.c.md y start.c (no va a encontrar start.sh ni ejecutarlo)


2. Hard link hace que dos file apunten al mismo inodo, si se modifica el uno tambien lo hace el otro. 
Para ver esto, dos archivos deberían tener el mismo inodo number. En la tabla, luego de ":" viene el inode number,
ninguno se repite. Por lo que no exite un hard link. En el soft link no tendrán el mismo inode number. En realidad se podria decir que un archivo comun es un hard link, asi que hasta que no elimine el último,
archivo se va a conservar (el contador debe llegar a cero para que no lo continue haciendo).
Podria usar rm, en man rm = unlink. También, siguiendo esa lógica podría usar unlink.




