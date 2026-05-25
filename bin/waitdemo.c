#include <n7OS/process.h>
#include <n7OS/time.h>
#include <stdio.h>

// Ce processus est utilisé pour démontrer le fonctionnement de waitpid
// Il dure 3 secondes puis se termine avec un code de sortie de 42
void waitdemo() {
  // On récupère le timer au départ pour permettre de comparer
  uint32_t time_count = get_timer();
  printf("Enfant waitdemo : démarrage du test waitpid\n");
  display_processes(); // Affiche la liste des processus pour montrer le parent
                       // et l'enfant
  // Boucle de 3 secondes
  while (get_timer() - time_count < 3000u) {
  }
  printf("Enfant waitdemo : fin du travail, sortie immédiate\n");
  exit_process(42);
}
