#include "configuraciones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;

uint32_t calcular_tiempo_de_espera(int unidades_de_trabajo);
op_code atender_gen(int cod_op, t_list *parametros);
op_code atender_stdin(int cod_op, t_list *parametros);
op_code atender_stdout(int cod_op, t_list *parametros);
op_code atender_dialfs(int cod_op, t_list *parametros);
void setear_config(char *archivo_config_io);

int main(int argc, char *argv[])
{

    // if (argc < 3)
    // {
    logger_propio = crear_logger("entradasalida_propio");
    //     log_info(logger_propio, "Uso: %s [nombre de interfaz] [ruta de archivo de configuracion]\n", argv[0]);
    //     return EXIT_FAILURE;
    // }

    char *nombre = malloc(strlen(argv[1]) + 1);
    strcpy(nombre, argv[1]);
    logger_obligatorio = crear_logger("entradasalida_obligatorio");
    log_info(logger_propio, "Iniciando Interfaz de I/O %s", nombre);

    setear_config(argv[2]);

    // conectar_a
    // usleep(1000 * 100);
    // int conexion_memoria = conectar_a("MEMORIA", logger_propio, 5);

    // conectar al kernel y presentarse
    int conexion = crear_conexion(logger_propio, obtener_ip_kernel(), obtener_puerto_kernel());
    t_paquete *paquete = crear_paquete(PAQUETE);
    agregar_a_paquete_string(paquete, nombre);
    agregar_a_paquete_string(paquete, obtener_tipo_interfaz());
    enviar_paquete(paquete, conexion);
    eliminar_paquete(paquete);

    op_code (*atender)(int cod_op, t_list *parametros) = NULL;

    char *tipo_interfaz = obtener_tipo_interfaz();
    if (strcmp(tipo_interfaz, "GENERICA") == 0)
    {
        atender = atender_gen;
    }
    else if (strcmp(tipo_interfaz, "STDIN") == 0)
    {
        atender = atender_stdin;
    }
    else if (strcmp(tipo_interfaz, "STDOUT") == 0)
    {
        atender = atender_stdout;
    }
    else if (strcmp(tipo_interfaz, "DIALFS") == 0)
    {
        atender = atender_dialfs;
    }
    free(tipo_interfaz);

    uint8_t respuesta;
    int cod_op;
    t_list *parametros = list_create();

    while (1)
    {
        cod_op = recibir_operacion(conexion);
        parametros = recibir_paquete(conexion);

        respuesta = atender(cod_op, parametros);

        enviar_cod_op(respuesta, conexion);
    }

    terminar_programa(conexion, logger_propio, config);
    log_destroy(logger_obligatorio);

    return 0;
}

op_code atender_gen(int cod_op, t_list *parametros)
{
    op_code respuesta = OK;

    if (cod_op == IO_GEN_SLEEP)
    {
        loggear_operacion(*(int *)list_get(parametros, 0), comandos[cod_op]);

        int unidades_de_trabajo = atoi((char *)list_get(parametros, 1));
        uint32_t tiempo_de_espera_ms = calcular_tiempo_de_espera(unidades_de_trabajo) * 1000;
        usleep((__useconds_t)tiempo_de_espera_ms);
    }
    else
    {
        respuesta = OPERACION_INVALIDA;
    }

    return respuesta;
}

uint32_t calcular_tiempo_de_espera(int unidades_de_trabajo)
{
    return unidades_de_trabajo * obtener_tiempo_unidad_trabajo();
}

void setear_config(char *archivo_config_io)
{
    char *config_ruta = string_new();
    string_append(&config_ruta, "configuraciones/");
    string_append(&config_ruta, archivo_config_io);
    config = iniciar_config(logger_propio, config_ruta);
    free(config_ruta);
}