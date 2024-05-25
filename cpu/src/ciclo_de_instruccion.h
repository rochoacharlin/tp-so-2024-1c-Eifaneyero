#ifndef CICLO_DE_INSTRUCCION_H
#define CICLO_DE_INSTRUCCION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include "utils/contexto_ejecucion.h"
#include "utils/estructuras_compartidas.h"
#include "configuraciones.h"
#include "conexiones/conexiones.h" //Para usar conexiones globales

// ID instruccion
typedef enum
{
    SET,
    MOV_IN,
    MOV_OUT,
    SUM,
    SUB,
    JNZ,
    RESIZE,
    COPY_STRING,
    WAIT,
    SIGNAL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    EXIT
} t_id;

char *comandos[] = {
    [SET] = "SET",
    [MOV_IN] = "MOV_IN",
    [MOV_OUT] = "MOV_OUT",
    [SUM] = "SUM",
    [SUB] = "SUB",
    [JNZ] = "JNZ",
    [RESIZE] = "RESIZE",
    [COPY_STRING] = "COPY_STRING",
    [WAIT] = "WAIT",
    [SIGNAL] = "SIGNAL",
    [IO_GEN_SLEEP] = "IO_GEN_SLEEP",
    [IO_STDIN_READ] = "IO_STDIN_READ",
    [IO_STDOUT_WRITE] = "IO_STDOUT_WRITE",
    [IO_FS_CREATE] = "IO_FS_CREATE",
    [IO_FS_DELETE] = "IO_FS_DELETE",
    [IO_FS_TRUNCATE] = "IO_FS_TRUNCATE",
    [IO_FS_WRITE] = "IO_FS_WRITE",
    [IO_FS_READ] = "IO_FS_READ",
    [EXIT] = "EXIT"};

typedef struct
{
    t_id id;
    char *param1;
    char *param2;
    char *param3;
    char *param4;
    char *param5;
} t_instruccion;

// Version 2.0:
// typedef struct {
//     t_id id;
//     char* parametros[];
// }t_instruccion;

extern bool hay_interrupcion;
extern pthread_mutex_t mutex_interrupt;
extern char *motivo_interrupcion;

// -------------------- CICLO DE INSTRUCCION -------------------- //

void ciclo_de_instruccion(t_contexto contexto);
t_instruccion *fetch(t_contexto contexto);
void decode(t_instruccion *instruccion);
void execute(t_instruccion *instruccion);
void check_interrupt(t_contexto contexto);

// -------------------- CICLO DE INSTRUCCION: Secundarias -------------------- //

// ---------- FETCH ---------- //

t_instruccion *convertir_string_a_instruccion(char *instruccion_string);
t_instruccion *inicializar_instruccion(t_instruccion *instruccion);
t_id string_id_to_enum_id(char *id_string);

// ---------- OTRAS ---------- //

bool instruccion_bloqueante(t_id id_instruccion);

// -------------------- INSTRUCCIONES -------------------- //

void set(char *nombre_registro, void *valor);
void sum(char *nombre_destino, char *nombre_origen);
void sub(char *nombre_destino, char *nombre_origen);
void jnz(char *nombre_registro, void *nro_instruccion);
void io_gen_sleep(char *nombre, char *unidades);

// -------------------- MANEJO DE CONTEXTO -------------------- //

void devolver_contexto(char *motivo_desalojo, t_list *param);

#endif
