#ifndef MEMORIA_CONEXIONES_H
#define MEMORIA_CONEXIONES_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include <utils/estructuras_compartidas/instrucciones.h>
#include <configuraciones.h>
#include <lectura/lectura.h>

extern t_log *logger_propio;
extern int server_fd;
extern int sockets[3];
extern t_dictionary *script_segun_PID;

void iniciar_semaforo_para_peticiones(void);

void esperar_a(char *tipo, int *socket, int server);

void iniciar_servidor_memoria(void);
void iniciar_conexiones();

void atender_kernel(int socket_cliente);
void atender_cpu(int socket_cliente);
void atender_io(int socket_cliente);

#endif
