/**
 * @file process.c
 * @author Arthur
 * @brief Implémentation de la gestion des processus.
 * @version 0.2
 * @date 2026-04-21
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <malloc.h>
#include <n7OS/cpu.h>
#include <n7OS/mem.h>
#include <n7OS/paging.h>
#include <n7OS/process.h>
#include <stdio.h> // Pour printf dans display_processes (debug)
#include <string.h>

// Déclaration de la fonction d'assemblage pour le changement de contexte
extern void ctx_sw(uint32_t *old, uint32_t *new);
extern void processus1(); // Prototype du processus de test
extern void minishell();  // Prototype du processus de test

// La table des processus

process_t process_table[NB_PROC];

// Pointeurs et compteurs d'état du système
process_t *current_active_process = NULL;
pid_t next_pid_counter = 0;

/**
 * @brief Processus inactif (idle) tournant en fond.
 */
static void idle() {
  while (1) {
    printf("Idle (PID %u)\n", get_current_pid());
    hlt();
  }
}

/**
 * @brief Trouve une case vide dans la table des processus.
 * @return L'index de la case, ou -1 si la table est pleine.
 */
static int find_free_process_slot() {
  for (int i = 0; i < NB_PROC; i++) {
    if (process_table[i].stack ==
        NULL) { // Si stack est NULL, la case est libre
      return i;
    }
  }
  return -1;
}

/**
 * @brief Retourne l'index d'un processus dans la table, ou -1.
 */
static int index_of(const process_t *p) {
  for (int i = 0; i < NB_PROC; i++) {
    if (&process_table[i] == p)
      return i;
  }
  return -1;
}

void init_processes() {
  // Initialisation de la table
  for (int i = 0; i < NB_PROC; i++) {
    process_table[i].stack = NULL;
    process_table[i].pid = 0;
  }

  // Création du processus idle
  create_process("idle", idle, PRIORITY_LOW);
  create_process("processus1", processus1, PRIORITY_MEDIUM);
  create_process("minishell", minishell, PRIORITY_MEDIUM);
  // On démarre sur le processus idle
  current_active_process = &process_table[0];
  current_active_process->state = EXE;
  idle();

  // idle(); // Lancement du processus idle
}

pid_t create_process(const char *name, void (*entry_point)(void),
                     process_priority_t priority) {
  int pos = find_free_process_slot();
  if (pos == -1) {
    return 0; // Erreur : plus de place (0 utilisé car pid_t est unsigned)
  }

  process_t *p = &process_table[pos];

  p->name = (char *)name;
  p->pid = next_pid_counter++;
  p->ppid = get_current_pid(); // Le parent est celui qui appelle create_process
  p->state = PRET;
  p->priority = priority;
  p->exit_code = 0;

  // Allocation de la pile
  p->stack = (uint32_t *)malloc(STACK_SIZE);

  p->stack[STACK_SIZE - 1] = (uint32_t)entry_point; // Adresse de retour fictive

  // Initialisation du contexte
  p->context[0] = 0;                                     // ebx
  p->context[1] = (uint32_t)(p->stack + STACK_SIZE - 1); // esp
  p->context[2] = 0;                                     // ebp
  p->context[3] = 0;                                     // esi
  p->context[4] = 0;                                     // edi
  return p->pid;
}

void scheduler() {
  process_t *old = current_active_process;

  if (old->state == EXE) {
    old->state = PRET;
  }

  /* Recherche du prochain processus PRET (Round-Robin) */
  int cur_idx = index_of(old);
  int next_idx = (cur_idx + 1) % NB_PROC;

  while (process_table[next_idx].state != PRET ||
         process_table[next_idx].stack == NULL) {
    next_idx = (next_idx + 1) % NB_PROC;
  }

  process_t *next = &process_table[next_idx];

  next->state = EXE;
  current_active_process = next;

  ctx_sw(old->context, next->context);
}

process_t *get_process_by_pid(pid_t pid) {
  for (int i = 0; i < NB_PROC; i++) {
    if (process_table[i].stack != NULL && process_table[i].pid == pid) {
      return &process_table[i];
    }
  }
  return NULL;
}

void block_current_process() {
  current_active_process->state = BLO;
  scheduler(0); // Force le changement de contexte immédiatement
}

void unblock_process(pid_t pid) {
  process_t *p = get_process_by_pid(pid);
  if (p != NULL && p->state == BLO) {
    p->state = PRET;
  }
}

void exit_process(int status) {
  current_active_process->state = ZOM;
  current_active_process->exit_code = status;
  current_active_process->stack = NULL; // Marque la case comme libre

  // Réveiller le parent s'il était en train d'attendre (waitpid)
  process_t *parent = get_process_by_pid(current_active_process->ppid);
  if (parent != NULL && parent->state == BLO) {
    unblock_process(parent->pid);
  }

  // On passe la main, ce processus ne s'exécutera plus jamais
  scheduler(0);
}

void kill_current_process() { exit_process(-1); }

int waitpid(pid_t pid) {
  process_t *child = get_process_by_pid(pid);

  // Si l'enfant n'existe pas ou s'il n'est pas le nôtre
  if (child == NULL || child->ppid != get_current_pid()) {
    return -1;
  }

  // Tant que l'enfant n'est pas mort, on bloque le parent
  while (child->state != ZOM) {
    block_current_process();
  }

  // Récupération du code de retour
  int status = child->exit_code;

  // Nettoyage complet
  free(child->stack);
  child->stack = NULL;
  child->pid = 0;

  return status;
}

pid_t get_current_pid() { return current_active_process->pid; }

static const char *state_to_str(process_state_t s) {
  switch (s) {
  case PRET:
    return "PRET";
  case EXE:
    return "EXE ";
  case BLO:
    return "BLO ";
  case ZOM:
    return "ZOM ";
  default:
    return "????";
  }
}

void display_processes(void) {
  printf("PID  PPID ETAT NOM\n");
  printf("------------------------------------\n");
  for (int i = 0; i < NB_PROC; i++) {
    if (process_table[i].stack != NULL) {
      printf("%-4u %-4u %s %s\n", process_table[i].pid, process_table[i].ppid,
             state_to_str(process_table[i].state), process_table[i].name);
    }
  }
}