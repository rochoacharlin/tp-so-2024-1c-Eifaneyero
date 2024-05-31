#include "conexiones.h"

int servidor(void)
{
    int servidor_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha());
    log_info(logger_propio, "Kernel listo para recibir clientes");
    int cliente_fd = esperar_cliente(logger_propio, servidor_fd);
    log_info(logger_propio, "Se conectó un cliente!");

    int32_t handshake_esperado = 4;
    int handshake_respuesta = handshake_servidor(logger_propio, cliente_fd, handshake_esperado);

    return servidor_fd;
}

int conexion_interrupt_con_CPU(void)
{
    conexion_kernel_cpu_interrupt = crear_conexion(logger_propio, obtener_ip_cpu(), obtener_puerto_cpu_interrupt());
    int32_t handshake = 5;
    int handshake_respuesta = handshake_cliente(logger_propio, conexion_kernel_cpu_interrupt, handshake);
    return conexion_kernel_cpu_dispatch;
}

int conexion_dispatch_con_CPU(void)
{
    conexion_kernel_cpu_dispatch = crear_conexion(logger_propio, obtener_ip_cpu(), obtener_puerto_cpu_dispatch());
    int32_t handshake = 5;
    int handshake_respuesta = handshake_cliente(logger_propio, conexion_kernel_cpu_dispatch, handshake);
    return conexion_kernel_cpu_interrupt;
}

int conexion_memoria(void)
{
    int conexion_memoria = crear_conexion(logger_propio, obtener_ip_memoria(), obtener_puerto_memoria());
    int32_t handshake = 1;
    int handshake_respuesta = handshake_cliente(logger_propio, conexion_memoria, handshake);

    return conexion_memoria;
}

/* FUERA DE USO
t_contexto *gestionar_ejecucion_proceso(t_pcb *proceso_en_ejecucion, t_contexto *contexto_ejecucion) // CPU ejecuta luego de enviar contexto actualizado
{
    if (contexto_ejecucion != NULL)
        destruir_contexto(contexto_ejecucion);
    iniciar_contexto();
    asignar_valores_pcb_a_contexto(proceso_en_ejecucion);
    enviar_contexto_a_cpu(conexion_kernel_cpu_dispatch); // Envió para realizar ciclo de instrucción
    // Aguardo envio de contexto por parte de CPU
    if (recibir_operacion(conexion_kernel_cpu_dispatch) < 0) // Solo verifico recepción de respuesta, indiferente al opCode
        error("Se desconectó el CPU.");
    recibir_contexto_y_actualizar_global(conexion_kernel_cpu_dispatch); // Da por sentado que opCode recibido es CONTEXTO_EJECUCION
    actualizar_pcb(proceso_en_ejecucion);
    return contexto_ejecucion;
}
*/