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
#define CASTAWAYS 15
#define CARNE_HUMANA 300
#define COMER 20

/*
Argumentos de cada Náufrago
status = estado atual do naufrago (0 - naufragado, 1 - resgatado , 2 - morto por outro naufrago, 3 - morto de fome)
sexo = sexo e idade do naufrago (0 - Homem Adulto, 1 - Mulher Adulta, 2 - Homem Criança, 3 - Mulher Criança)
nome = nome do náufrago 
*/
typedef struct{
  int id;
  int status;
  int sex;
  char name[15];
} castaway_arg, *ptr_castaway_arg;

castaway_arg cast_arg[CASTAWAYS];
pthread_t castaway[CASTAWAYS];
pthread_mutex_t l = PTHREAD_MUTEX_INITIALIZER; /* lock pros náufragos*/
pthread_cond_t cn = PTHREAD_COND_INITIALIZER; /* condicional pros náufragos*/
int comida = 0;
int number_adultmale, number_adultfemale, number_childrenmale, number_childrenfemale, id_kill;

char male_names[100][15] = {
    "Artur",        /*0*/
    "Alfredo",      /*1*/
    "Adelmar",      /*2*/
    "Álvaro",       /*3*/
    "Abraão",       /*4*/
    "Apolo",        /*5*/
    "Beto",         /*6*/
    "Baltasar",     /*7*/
    "Bartolomeu",   /*8*/
    "Bóris",        /*9*/
    "Caetano",      /*10*/
    "Cláudio",      /*11*/
    "César",        /*12*/
    "Clóvis",       /*13*/
    "Cabrinha",     /*14*/
    "Dante",        /*15*/
    "Dorival",      /*16*/
    "Daniel",       /*17*/
    "Diego",        /*18*/
    "Edu",          /*19*/
    "Ezequiel",     /*20*/
    "Fagner",       /*21*/
    "Frederico",    /*22*/
    "Gerson",       /*23*/
    "Gonzaga",      /*24*/
    "Galvão",       /*25*/
    "Haroldo",      /*26*/
    "Hermes",       /*27*/
    "ígor",         /*28*/
    "Irineu",       /*29*/
    "João",         /*30*/
    "Jeziel",       /*31*/
    "Joselino",     /*32*/
    "Jarbas",       /*33*/
    "Kevin",        /*34*/
    "Lucas",        /*35*/
    "Leôncio",      /*36*/
    "Lorenzo",      /*37*/
    "Moacir",       /*38*/
    "Matusalém",    /*39*/
    "Mara",         /*40*/
    "Nathan",       /*41*/
    "Nazaré",       /*42*/
    "Nivaldo",      /*43*/
    "Nicholas",     /*44*/
    "Otávio",       /*45*/
    "Perotto",      /*46*/
    "Plínio",       /*47*/
    "Quirilo",      /*48*/
    "Rafael",       /*49*/
    "Sanderson",    /*50*/
    "Selmo",        /*51*/
    "Silva",        /*52*/
    "Tatiano",      /*53*/
    "Thales",       /*54*/
    "Thomas",       /*55*/
    "Uriel",        /*56*/
    "Valdemar",     /*57*/
    "Virgínio",     /*58*/
    "Xavier",       /*59*/
    "Wilson",       /*60*/
    "Soneca",       /*61*/
    "Jesus",        /*62*/
    "Yan",          /*63*/
    "Jamez",        /*64*/
    "Xonas",        /*65*/
    "Yuri",         /*66*/
    "Zé",           /*67*/
    "Zacarias",     /*68*/
    "Moutinho",     /*69*/
    "Ximenes",      /*70*/
    "João Gabriel", /*71*/
    "Zanina",       /*72*/
    "Yago",         /*73*/
    "Alchieri",     /*74*/
    "Vânder",       /*75*/
    "Wladimir",     /*76*/
    "Vitor",        /*77*/
    "Rebores",      /*78*/
    "Mafra",        /*79*/
    "Tiago",        /*80*/
    "Ryu",          /*81*/
    "Kaio",         /*82*/
    "Otto",         /*83*/
    "Batista",      /*84*/
    "Zohan",        /*85*/
    "Léo",          /*86*/
    "Khalil",       /*87*/
    "Buda",         /*88*/
    "Kratos",       /*89*/
    "Antônio",      /*90*/
    "Xerxes",       /*91*/
    "Farpas",       /*92*/
    "Heitor",       /*93*/
    "Oswaldo",      /*94*/
    "Maranhão",     /*95*/
    "Jonathan",     /*96*/
    "Wesley",       /*97*/
    "Pedro",        /*98*/
    "Juju"          /*99*/
};

char female_names[100][15] = {
    "Azarzuela",   /*0*/
    "Afrodite",    /*1*/
    "Amanda",      /*2*/
    "Amélia",      /*3*/
    "Ana",         /*4*/
    "Beatriz",     /*5*/
    "Bela",        /*6*/
    "Bernadete",   /*7*/
    "Cássia",      /*8*/
    "Caterina",    /*9*/
    "Clodete",     /*10*/
    "Conceição",   /*11*/
    "Consolação",  /*12*/
    "Cilda",       /*13*/
    "Cristina",    /*14*/
    "Dafne",       /*15*/
    "Dalva",       /*16*/
    "Denise",      /*17*/
    "Donzélia",    /*18*/
    "Donizete",    /*19*/
    "Dolores",     /*20*/
    "Edna",        /*21*/
    "Elizabeth",   /*22*/
    "Érica",       /*23*/
    "Fabiana",     /*24*/
    "Fátima",      /*25*/
    "Flor",        /*26*/
    "Francisca",   /*27*/
    "Gabi",        /*28*/
    "Gertrudes",   /*29*/
    "Helga",       /*30*/
    "Iolanda",     /*31*/
    "Irina",       /*32*/
    "Isolda",      /*33*/
    "Ivânia",      /*34*/
    "Jaqueline",   /*35*/
    "Joelma",      /*36*/
    "Janice",      /*37*/
    "Josefina",    /*38*/
    "Jussara",     /*39*/
    "Luísa",       /*40*/
    "Luiza",       /*41*/
    "Lana",        /*42*/
    "Karen",       /*43*/
    "Kátia",       /*44*/
    "Kelly",       /*45*/
    "Katarina",    /*46*/
    "Kyara",       /*47*/
    "Lisa",        /*48*/
    "Madalena",    /*49*/
    "Maria",       /*50*/
    "Manuela",     /*51*/
    "Morgana",     /*52*/
    "Nádia",       /*53*/
    "Carolina",    /*54*/
    "Norma",       /*55*/
    "Nina",        /*56*/
    "Odete",       /*57*/
    "Olívia",      /*58*/
    "Oriana",      /*59*/
    "Paloma",      /*60*/
    "Patrícia",    /*61*/
    "Penélope",    /*62*/
    "Priscila",    /*63*/
    "Quirina",     /*64*/
    "Renata",      /*65*/
    "Rosa",        /*66*/
    "Rosália",     /*67*/
    "Ruth",        /*68*/
    "Sofia",       /*69*/
    "Sabrina",     /*70*/
    "Sássia",      /*71*/
    "Samara",      /*72*/
    "Socorro",     /*73*/
    "Solange",     /*74*/
    "Susana",      /*75*/
    "Susete",      /*76*/
    "Tabita",      /*77*/
    "Tamara",      /*78*/
    "Terezinha",   /*79*/
    "Úrsula",      /*80*/
    "Ursulina",    /*81*/
    "Valentina",   /*82*/
    "Vitória",     /*83*/
    "Viviana",     /*84*/
    "Violeta",     /*85*/
    "Welwitschia", /*86*/
    "Xênia",       /*87*/
    "Yara",        /*88*/
    "Yasmin",      /*89*/
    "Zahra",       /*90*/
    "Zulmira",     /*91*/
    "Zuleica",     /*92*/
    "Zoraida",     /*93*/
    "Dilma",       /*94*/
    "Zeferina",    /*95*/
    "Zoraya",      /*96*/
    "Eliana",      /*97*/
    "Creusa",      /*98*/
    "Cremilda"     /*99*/
};

void clrscr(){
  system("@cls||clear");
}

void print_status(int status){
  
  switch (status){
    case 0:
      printf("NAUFRAGADO");
      break;

    case 1:
      printf("RESGATADO");
      break;

    case 2:
      printf("MORTO - Virou comida!");
      break;

    case 3:
      printf("MORTO - Morreu de fome!");
      break;

    default:
      break;
  }
}

/*
Seguindo os indices:
i = estado atual do naufrago (0 - naufragado, 1 - resgatado , 2 - morto por outro naufrago, 3 - morto de fome)
i + 1 = sexo e idade do naufrago (0 - Homem Adulto, 1 - Mulher Adulta, 2 - Homem Criança, 3 - Mulher Criança)
i + 2 = numero entre 0 e 99 equivalente ao seu nome 
*/
void initialize_castaways(){
  int length = CASTAWAYS;
  int i, name, j;

  number_adultfemale = 0;
  number_adultmale = 0;
  number_childrenfemale = 0;
  number_childrenmale = 0;

  srand(time(NULL));

  for(i = 0; i < length; i++){
    cast_arg[i].id = i;
    cast_arg[i].sex = (rand() % 4);
    switch (cast_arg[i].sex){
      case 0:
        number_adultmale++;
        break;

      case 1:
        number_adultfemale++;
        break;

      case 2:
        number_childrenmale++;
        break;

      case 3:
        number_childrenfemale++;
        break;

      default:
        break;
    }
    name = (rand() % 100);
    j = 0;
    if(cast_arg[i].sex == 0 || cast_arg[i].sex == 2){
      while(male_names[name][j] != '\0'){
        cast_arg[i].name[j] = male_names[name][j];
        j++;
      }
    }
    else{
      while(female_names[name][j] != '\0'){
        cast_arg[i].name[j] = female_names[name][j];
        j++;
      }
    }
    cast_arg[i].name[j] = '\0';
  }
}

void print_castaways(){
  int length = CASTAWAYS;
  int i;

  printf("-------------------------\n");
  printf("LISTA DOS NÁUFRAGOS\n\n");
  printf("Homens Adultos: %d -- Mulheres Adultas: %d -- Crianças: %d\n\n", number_adultmale, number_adultfemale, number_childrenfemale + number_childrenmale);

  for (i = 0; i < length; i++){
    printf("Náufrago %d: ", cast_arg[i].id);

    switch (cast_arg[i].sex){
      case 0:
        printf("%s (Homem) ", &cast_arg[i].name[0]);
        break;

      case 1:
        printf("%s (Mulher) ", &cast_arg[i].name[0]);
        break;

      case 2:
        printf("%s (Criança H) ", &cast_arg[i].name[0]);
        break;

      case 3:
        printf("%s (Criança M) ", &cast_arg[i].name[0]);
        break;

      default:
        break;
    }

    printf("- ");
    print_status(cast_arg[i].status);
    printf("\n");
  }
  printf("-------------------------\n\n");

  printf("\n\nPRESSIONE QUALQUER TECLA PARA CONTINUAR\n");
  getchar();
}

int kill_someone(int id){
  int someone = rand() % CASTAWAYS;
  int i;
  int found = 0;
  int found_id = id;
  for(i = someone; i < CASTAWAYS; i++){
    if(cast_arg[i].status == 0){
      if(cast_arg[i].id != id){
        found = 1;
        cast_arg[i].status = 2;
        found_id = cast_arg[i].id;
        break;
      }
    }
  }
  if(!found){
    for(i = 0; i < someone; i++){
      if(cast_arg[i].status == 0){
        if(cast_arg[i].id != id){
          cast_arg[i].status = 2;
          found_id = cast_arg[i].id;
          break;
        }
      }
    }
  }
  return found_id;
}

void *surviving(void *arg) {
  ptr_castaway_arg castaway_arg = (ptr_castaway_arg)arg;

  while(castaway_arg->status == 0){
    pthread_mutex_lock(&l);
    while(comida == 0){
      pthread_cond_wait(&cn, &l);
    }
    if(castaway_arg->status == 0){
      printf("Náufrago %d (%s): Vou comer... ainda existem %d porções de comida\n", castaway_arg->id, castaway_arg->name, comida);
      sleep(1);
      comida-=COMER;
      if(comida == 0){
        printf("Náufrago %d (%s): EITA... existem %d porções de comida... alguém precisa ser sacrificado!\n", castaway_arg->id, castaway_arg->name, comida);
        id_kill = kill_someone(castaway_arg->id);
        if(id_kill == castaway_arg->id){
          printf("Infelizmente, não tem mais ninguém...\n");
          castaway_arg->status = 3;
        }
        else{
          printf("Náufrago %d (%s) foi morto por Náufrago %d (%s)... conseguiu-se %d porções de comida a mais\n", cast_arg[id_kill].id, cast_arg[id_kill].name, castaway_arg->id, castaway_arg->name, CARNE_HUMANA);
          comida += CARNE_HUMANA;
        }
      }
    }
    pthread_mutex_unlock(&l);
    sleep(3);
  }
  pthread_exit(0);
}

int shipwreck(){
  int i;

  printf("-------------------------\n");
  printf("\nACIDENTE!!!!!\n\n");
  printf("Um cruzeiro naufragou no oceano atlântico...\n");
  printf("Os %d sobreviventes nadaram até uma ilha próxima e aguardam resgate...\n", CASTAWAYS);
  srand(time(NULL));
  comida = (rand() % 5) * 100;
  printf("Um total equivalente à %d porções de comida foi levado dos restos do navio até a ilha...\n", comida);
  printf("Quantos sobreviverão???\n\n");

  initialize_castaways();
  print_castaways();

  for(i = 0; i < CASTAWAYS; i++){
    pthread_create(&castaway[i], NULL, surviving, (void *)(&(cast_arg[i])));
  }

  for (i = 0; i < CASTAWAYS; i++){
    pthread_join(castaway[i], NULL);
  }

  print_castaways();

  return 0;
}

int main(){
  int continue_simulation = TRUE;
  char input;

  printf("-------------------------\n");
  printf("\nCASTAWAY PROBLEM\n\n");
  printf("-------------------------\n\n");

  
  do{

    printf("\nDeseja iniciar a simulação de naufrágio? (s/n)\n");
    scanf("%c", &input);
    getchar();

    while(input != 's' && input != 'S' && input!= 'n' && input != 'N'){
      printf("\nInsira apenas os caracteres 'S' ou 'N'\n");
      printf("Deseja iniciar a simulação de naufrágio? (s/n)\n");
      scanf("%c", &input);
      getchar();
    }

    if(input == 'n' || input == 'N'){
      continue_simulation = FALSE;
    }
    else{
      clrscr();
      shipwreck();
    }
  } while (continue_simulation);
  

  return 0;
}