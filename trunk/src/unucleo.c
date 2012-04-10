/*
 * src/unucleo.c
 *
 * unife source.
 * 
 * TODO - vamos dividir em mais de um modulo?
 *
 * Germano Andersson <germanoa@gmail.com>
 * Felipe Lahti <felipe.lahti@gmail.com>
 *
 * 2012-XX-XX
 */

#include <ucontext.h>
#include <stdint.h>
#include <include/unucleo.h>
#include <stdio.h>

proc_state *procs_ready;
proc_state *procs_blocked;

proc_struct *proc_running;

map_join *joins;

int libsisop_init()
{
    return 99;
}


void scheduler(void)
{
    printf("sched\n");
    /* 
    1. se proc_running != NULL
        se existe algum proc blocked joined ao proc_running
            enviar proc blocked para ready & proc_running = NULL. (fazer para todos processos em blocked, pois pode haver mais de um joined ao proc q terminou).
    2. enquanto houver nivel de prioridade
            se tem um processo na proc_state_ready
                remover do estado state, salva-lo em proc_running chavear contexto & break (break = fazer com que no retorno do swapcontext retorne ao inicio da funcao)
            senao descer um nivel de prioridade e voltar a 2.
    3. se chegou aqui, entao nao ha mais processos. exit.       
    */
}
