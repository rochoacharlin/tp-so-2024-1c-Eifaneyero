#include "planificacion.h"

sem_t hay_pcbs_NEW;
t_list *pcbs_en_NEW;
sem_t sem_grado_multiprogramacion;
t_list *pcbs_en_memoria;
pthread_mutex_t mutex_lista_NEW;
pthread_mutex_t mutex_lista_READY;
sem_t planificacion_liberada;
sem_t planificacion_pausada;
t_list *pcbs_en_READY;
sem_t hay_pcbs_READY;
int32_t procesos_creados = 0;

void planificar_a_largo_plazo(void)
{
    while (1)
    {
        sem_wait(&hay_pcbs_NEW);
        sem_wait(&sem_grado_multiprogramacion);
        sem_wait(&planificacion_liberada);

        t_pcb *pcb = obtener_siguiente_pcb_READY();

        // recibir_estructuras_iniciales_memoria(pcb);

        estado anterior = pcb->estado;
        pcb->estado = READY;
        list_add(pcbs_en_memoria, pcb);

        // log minimo y obligatorio
        loggear_cambio_de_estado(pcb->PID, anterior, pcb->estado);

        sem_post(&planificacion_liberada);
        ingresar_pcb_a_READY(pcb);
    }
}

t_pcb *obtener_siguiente_pcb_READY(void)
{
    pthread_mutex_lock(&mutex_lista_NEW);
    t_pcb *pcb = desencolar_pcb(pcbs_en_NEW);
    pthread_mutex_unlock(&mutex_lista_NEW);
    return pcb;
}

void ingresar_pcb_a_READY(t_pcb *pcb)
{
    pthread_mutex_lock(&mutex_lista_READY);
    encolar_pcb(pcbs_en_READY, pcb);
    pthread_mutex_unlock(&mutex_lista_READY);

    sem_post(&hay_pcbs_READY);

    // log minimo y obligatorio
    lista_PIDS = string_new();
    mostrar_PIDS(pcbs_en_READY);
    loggear_ingreso_a_READY(lista_PIDS);
    free(lista_PIDS);
}

void ingresar_pcb_a_NEW(t_pcb *pcb)
{
    pthread_mutex_lock(&mutex_lista_NEW);
    encolar_pcb(pcbs_en_NEW, pcb);
    pthread_mutex_unlock(&mutex_lista_NEW);

    // log minimo y obligatorio
    loggear_creacion_proceso(pcb->PID);
    sem_post(&hay_pcbs_NEW);
}

void planificar_a_corto_plazo_segun_algoritmo(void)
{
    char *algoritmo = obtener_algoritmo_planificacion();

    if (strcmp(algoritmo, "FIFO") == 0)
    {
        planificar_a_corto_plazo(proximo_a_ejecutar_segun_FIFO);
    }
    else if (strcmp(algoritmo, "RR") == 0)
    {
        planificar_a_corto_plazo(proximo_a_ejecutar_segun_FIFO);
    }
    else if (strcmp(algoritmo, "VRR") == 0)
    {
        // TODO: Solucion VRR
        // si encuntra que hay pcbs en la cola_ready+, se ejecutara ese
    }
    else
    {
        log_error(logger_propio, "Algoritmo invalido. Debe ingresar FIFO, RR o VRR");
        abort();
    }
}

void planificar_a_corto_plazo(t_pcb *(*proximo_a_ejecutar)())
{
    crear_colas_de_bloqueo();
    while (1)
    {
        sem_wait(&hay_pcbs_READY);
        t_pcb *pcb_proximo = proximo_a_ejecutar();

        estado anterior = pcb_proximo->estado;
        pcb_proximo->estado = EXEC;

        // log minimo y obligatorio
        loggear_cambio_de_estado(pcb_proximo->PID, anterior, pcb_proximo->estado);

        // contexto_ejecucion = procesar_pcb_segun_algoritmo(pcb_proximo,algoritmo);
        // int rafaga_CPU = contexto_ejecucion->rafaga_CPU_ejecutada;
        // retorno_contexto(pcb_proximo, contexto_ejecucion);
    }
}

t_pcb *proximo_a_ejecutar_segun_FIFO(void)
{
    return desencolar_pcb(pcbs_en_READY);
}

void inicializar_listas_planificacion(void)
{
    pcbs_en_NEW = list_create();
    pcbs_en_READY = list_create();
    pcbs_en_memoria = list_create();
}

void destruir_listas_planificacion(void)
{
    list_destroy_and_destroy_elements(pcbs_en_NEW, (void *)destruir_pcb);
    list_destroy_and_destroy_elements(pcbs_en_READY, (void *)destruir_pcb);
}

void inicializar_semaforos_planificacion(void)
{
    pthread_mutex_init(&mutex_lista_NEW, NULL);
    pthread_mutex_init(&mutex_lista_READY, NULL);
    sem_init(&hay_pcbs_NEW, 0, 0);
    sem_init(&hay_pcbs_READY, 0, 0);
    sem_init(&sem_grado_multiprogramacion, 0, obtener_grado_multiprogramacion());
    sem_init(&planificacion_liberada, 0, 1);
}
void destruir_semaforos_planificacion(void)
{
    pthread_mutex_destroy(&mutex_lista_NEW);
    pthread_mutex_destroy(&mutex_lista_READY);
    sem_close(&hay_pcbs_NEW);
    sem_close(&hay_pcbs_READY);
    sem_close(&sem_grado_multiprogramacion);
}

/*
t_contexto_ejecucion *procesar_pbc_segun_algoritmo(t_contexto_ejecucion *contexto,char * algoritmo){



}
*/

t_contexto_ejecucion *ejecutar_segun_RR(t_pcb *pcb)
{

    enviar_contexto_actualizado(pcb->contexto, conexion_dispatch);
    int quantum = obtener_quantum();
    pthread_t hilo_Q;
    pthread_create(&hilo_Q, NULL, ejecutar_quamtum, (*void)&pcb);
    pthread_detach(hilo_Q);
    t_contexto_ejecucion *contexto;
    contexto = esperar_contexto();
    pthread_cancel(hilo_Q);

    return contexto;
}

t_contexto_ejecucion *esperar_contexto()
{
    // TODO
    // recv(conexion_dispatch,*buffer_contexto,tamanio,0 )
}

void *ejecutar_quamtum(void *pcb)
{
    t_pcb *pcb_q = (t_pcb *)pcb;

    usleep(pcb_q->quamtum);
    enviar_interrupcion_FIN_Q(pcb->PID, conexion_interrupt);
}

void enviar_interrupcion_FIN_Q(int PID, int fd)
{

    t_interrupcion *interrupcion = malloc(sizeof(interrupcion));
    interupcion->tipo_interrupcion = FIN_DE_QUANTUM;
    t_interrupcion->PID = PID;
    t_paquete *paquete = crear_paquete_interrupcion(interrupcion);
    enviar_paquete(paquete, fd);

    liberar_interrupcion(interrupcion);
    liberar_paquete(paquete);
}

t_paquete *crear_paquete_interrupcion(t_interrupcion *interr)
{

    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = INTERRUPCION;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = sizeof(desalojo) + sizeof(int);
    paquete->buffer->stream = malloc(buffer->size);

    int offset = 0;

    memcpy(paquete->buffer->stream + offset, &(interrupcion->motivo_interrupcion), sizeof(desalojo));
    offset += sizeof(desalojo);

    memcpy(paquete->buffer->stream + offset, &(interrupcion->pid), sizeof(int));

    return paquete;
}

/*

// No se a donde

struct
{

    desalojo tipo_interrupcion;
    int proceso_PID;
} t_interrupcion;

enum
{
    EXIT // este  para cuando haya que eliminar un proceso
    FIN_DE_QUANTUM
    ERROR
} desalojo;
//
enum
{

    INTERRUPCION
    DISPATCH

} t_conexion;

en cpu
*/