#ifndef KERNEL_CONSOLA_H_
#define KERNEL_CONSOLA_H_

// #include <string.h>
#include <readline/readline.h>
#include <stdlib.h>
#include "../planificacion/pcb.h"
#include "../planificacion/planificacion.h"
#include <unistd.h>

extern t_list *pcbs_en_READY;
extern sem_t planificacion_liberada;
extern sem_t planificacion_pausada;

void consola_interactiva(void);
void listar_procesos_por_cada_estado(void);
void listar_procesos_por_estado(char *estado, t_list *lista);

#endif