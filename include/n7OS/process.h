/**
 * @file process.h
 * @author Arthur
 * @brief Le fichier d'en-tête pour la gestion des processus.
 * @version 0.1
 * @date 2026-04-21
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <inttypes.h>

#define NB_PROC 15      // Nombre maximum de processus
#define STACK_SIZE 1024 // Taille de la pile de chaque processus
#define QUANTUM 50      // Quantum de temps pour l'ordonnancement

typedef uint32_t pid_t; // Type pour les PID

// Etats possibles d'un processus
typedef enum { // Problème avec la structure ici
  // TODO: ajouter un bitmap
  PRET, // Prêt à être exécuté
  EXE,  // En cours d'exécution
  BLO,  // Bloqué
  ZOM   // Zombie
} process_state_t;

// Structure représentant le contexte CPU d'un processus
typedef struct {
  uint32_t ebx;
  uint32_t esp; // Le pointeur de pile actuel du processus
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
} cpu_context_t;

// Priorités possibles pour les processus
typedef enum {
  PRIORITY_LOW,
  PRIORITY_MEDIUM,
  PRIORITY_HIGH
} process_priority_t;

// Structure représentant un processus
typedef struct {
  char *name;                  // Nom du processus
  pid_t pid;                   // PID du processus
  pid_t ppid;                  // PID du processus parent
  process_state_t state;       // Etat du processu
  uint32_t *stack;             // Pile du processus
  process_priority_t priority; // Priorité du processus
  // cpu_context_t context;       // Contexte CPU du processus
  uint32_t context[5]; // Contexte CPU du processus (ebx, esp, ebp, esi, edi)
  int exit_code;       // Code de sortie du processus
} process_t;

/**
 * @brief Initialise le système de gestion des processus.
 *
 */
void init_processes();

/**
 * @brief Crée un nouveau processus.
 * @param name Nom du processus.
 * @param entry_point Pointeur vers la fonction à exécuter.
 * @return Le PID du nouveau processus, ou -1 en cas d'erreur.
 */
pid_t create_process(const char *name, void (*entry_point)(void),
                     process_priority_t priority);

/**
 * @brief Mets à jour l'état du quantum du processus en cours et effectue
 * l'ordonnancement.
 *
 */
void scheduler();

/**
 * @brief Récupérer un processus par son PID.
 * @param pid Le PID du processus à récupérer.
 * @return Un pointeur vers le processus correspondant, ou NULL si aucun
 * processus avec ce PID n'existe.
 */
process_t *get_process_by_pid(pid_t pid);

/**
 * @brief Bloque le processus en cours d'exécution.
 *
 */
void block_current_process();

/**
 * @brief Débloque un processus bloqué.
 * @param pid Le PID du processus à débloquer.
 */
void unblock_process(pid_t pid);

/**
 * @brief Tue le processus en cours d'exécution.
 *
 */
void kill_current_process();

/**
 * @brief Affiche la liste des processus et leurs états.
 *
 */
void display_processes();

/**
 * @brief Fonction appelée par un processus pour se terminer de lui-même.
 * @param status Le code de retour du processus.
 */
void exit_process(int status);

/**
 * @brief Attend la fin d'un processus enfant et nettoie son état zombie.
 * @param pid Le PID de l'enfant à attendre (ou -1 pour n'importe lequel).
 * @return Le code de retour de l'enfant.
 */
int waitpid(pid_t pid);

/**
 * @brief Renvoie le PID du processus en cours d'exécution.
 */
pid_t get_current_pid();

#endif // __PROCESS_H__