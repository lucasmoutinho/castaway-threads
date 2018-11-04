/*

ALUNO: LUCAS DA SILVA MOUTINHO - 15/0015747
PROFESSOR: ALCHIERI
ESTUDO DIRIGIDO 8 - PROGRAMAÇÃO CONCORRENTE - 2/2018

*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0

int main(){
  int continue_simulation = TRUE;
  char input;

  printf("-------------------------\n");
  printf("\nCASTAWAY PROBLEM\n\n");
  printf("-------------------------\n\n");

  
  do{

    printf("\nDeseja simular uma rodada? (s/n)\n");
    scanf("%c", &input);
    getchar();

    while(input != 's' && input != 'S' && input!= 'n' && input != 'N'){
      printf("Insira apenas os caracteres 'S' ou 'N'\n");
      printf("Deseja simular uma rodada? (s/n)\n");
      scanf("%c", &input);
      getchar();
    }

    if(input == 'n' || input == 'N'){
      continue_simulation = FALSE;
    }
    else{
    }
  } while (continue_simulation);
  

  return 0;
}