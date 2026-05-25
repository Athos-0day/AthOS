/**
 * @file minishell.c
 * @author Arthur
 * @brief shell minimaliste pour tester la gestion des processus et des appels
 * système.
 * @version 0.1
 * @date 2026-05-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <n7OS/console.h>
#include <n7OS/cpu.h>
#include <n7OS/keyboard.h>
#include <n7OS/process.h>
#include <stdio.h>
#include <string.h>

void print_help() {
  printf("Commandes disponibles :\n");
  printf(" - help : Affiche ce message d'aide\n");
  printf(" - ps : Affiche la liste des processus\n");
  printf(" - block <pid> : Bloque le processus avec le PID spécifié\n");
  printf(" - unblock <pid> : Débloque le processus avec le PID spécifié\n");
  printf(" - kill <pid> : Tue le processus avec le PID spécifié\n");
  printf(" - processus1 : Lance le processus de test processus1\n");
  printf(" - waitdemo : Lance un processus enfant puis attend sa fin\n");
  printf(" - clear : Efface l'écran\n");
  printf(" - exit : Quitte le shell\n");
  printf("Tapez une commande pour l'exécuter.\n");
}

extern void processus1();
extern void waitdemo();

void minishell() {
  printf("Bienvenue dans le mini-shell de n7OS !\n");
  printf("Tapez 'help' pour voir les commandes disponibles.\n");

  char command[256];
  char c = 0;
  int idx = 0;
  printf("arthur@n7OS> ");
  while (1) {
    idx = 0;
    memset(command, 0, sizeof(command));

    while (1) {
      c = kgetch();
      if (c == '\0') {
        continue;
      }

      if (c == '\r' || c == '\n') {
        printf("\n");
        break;
      }

      if (c == '\b' || c == 127) {
        if (idx > 0) {
          idx--;
          command[idx] = '\0';
          // printf("\b \b");
        }
        continue;
      }

      if (idx < (int)sizeof(command) - 1) {
        command[idx++] = c;
        // printf("%c", c);
      }
    }

    if (idx == 0) {
      printf("arthur@n7OS> ");
      continue;
    }

    if (strcmp(command, "help") == 0) {
      print_help();
    } else if (strcmp(command, "ps") == 0) {
      display_processes();
    } else if (strcmp(command, "exit") == 0) {
      printf("Au revoir !\n");
      kill_current_process();
      break;
    } else if (strcmp(command, "clear") == 0) {
      printf("\f");
    } else if (strncmp(command, "block ", 6) == 0) {
      pid_t pid = atoi(command + 6);
      int result = block_process(pid);
      if (result == -1) {
        printf("Processus non trouvé ou déjà bloqué : %d\n", pid);
      }
    } else if (strncmp(command, "unblock ", 8) == 0) {
      pid_t pid = atoi(command + 8);
      int result = unblock_process(pid);
      if (result == -1) {
        printf("Processus non trouvé ou déjà débloqué : %d\n", pid);
      }
    } else if (strncmp(command, "kill ", 5) == 0) {
      pid_t pid = atoi(command + 5);
      int result = kill_process(pid);
      if (result == -1) {
        printf("Processus non trouvé : %d\n", pid);
      }
    } else if (strcmp(command, "processus1") == 0) {
      create_process("processus1", processus1, PRIORITY_MEDIUM);
    } else if (strcmp(command, "waitdemo") == 0) {
      pid_t child_pid = create_process("waitdemo", waitdemo, PRIORITY_MEDIUM);
      if (child_pid == 0) {
        printf("Impossible de créer le processus de démonstration.\n");
      } else {
        printf("Terminal : lancement du processus enfant PID %u\n", child_pid);
        int status = waitpid(child_pid);
        printf("Terminal : retour de waitpid, statut=%d\n", status);
      }
    } else {
      printf("Commande inconnue : %s\n", command);
    }

    printf("arthur@n7OS> ");
  }
}