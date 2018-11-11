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
#define CASTAWAYS 17
#define HUMAN_FOOD 200
#define EAT 20
#define BOAT_CAPACITY 3

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

/*
Argumentos de cada Barco
status = estado atual do barco (0 - resgatando, 1 - esperando na ilha , 2 - terminou resgate)
*/
typedef struct{
  int id;
  int status;
} rescue_boat_arg, *ptr_rescue_boat_arg;

castaway_arg cast_arg[CASTAWAYS]; /* Argumento dos Náufragos */
rescue_boat_arg boat_arg; /* Argumento do barco */
sem_t wait_boat; /* Semáforos para o barco */ 
pthread_t castaway[CASTAWAYS]; /* Thread dos Náufragos */
pthread_t boat; /* Thread do barco */
pthread_mutex_t l = PTHREAD_MUTEX_INITIALIZER; /* lock pros náufragos*/
pthread_cond_t cm = PTHREAD_COND_INITIALIZER;      /* condicional pros náufragos homens adultos - man*/
pthread_cond_t cw = PTHREAD_COND_INITIALIZER;      /* condicional pros náufragos mulheres adultos - woman*/
int comida = 0; /* Comida */
int number_adultmale, number_adultfemale, number_childrenmale, number_childrenfemale; /* Número de náufragos de cada sexo e idade */
int children_waiting, female_waiting, boat_waiting; /* Número de Náufragos esperando */
int number_alive = CASTAWAYS; 
int capacity = BOAT_CAPACITY;
int id_kill;

char male_names[100][15] = {
    "Artur",        /*0*/
    "Alfredo",      /*1*/
    "Almeida",      /*2*/
    "Giamps",       /*3*/
    "Abraão",       /*4*/
    "Apolo",        /*5*/
    "Daniels",         /*6*/
    "Dé",     /*7*/
    "Diogo",   /*8*/
    "Batman",        /*9*/
    "Carvalho",      /*10*/
    "Cláudio",      /*11*/
    "César",        /*12*/
    "Jadiel",       /*13*/
    "Cabrinha",     /*14*/
    "Dante",        /*15*/
    "Rodrigo Chaves",      /*16*/
    "Daniel",       /*17*/
    "Diego",        /*18*/
    "Edu",          /*19*/
    "Allison",     /*20*/
    "Fagner",       /*21*/
    "Fred",    /*22*/
    "Geraldo",       /*23*/
    "Gonzaga",      /*24*/
    "Galvão",       /*25*/
    "Capoeira",      /*26*/
    "Kipman",       /*27*/
    "Igor Batata",         /*28*/
    "Ícaro",       /*29*/
    "João",         /*30*/
    "Jeziel",       /*31*/
    "Rodolfo",     /*32*/
    "Jarbas",       /*33*/
    "Kevin",        /*34*/
    "Lucas",        /*35*/
    "Jureg",      /*36*/
    "Mesquita",      /*37*/
    "Marco Véi",       /*38*/
    "Marcola",    /*39*/
    "Kilmer",         /*40*/
    "Nathan",       /*41*/
    "Billy",       /*42*/
    "Ribas",      /*43*/
    "Nicholas",     /*44*/
    "Otávio",       /*45*/
    "Perotto",      /*46*/
    "Sodré",       /*47*/
    "Renato",      /*48*/
    "Rafael",       /*49*/
    "Halbe",    /*50*/
    "Rebores",        /*51*/
    "Struct",        /*52*/
    "Tuts",      /*53*/
    "Thaleco",       /*54*/
    "Thomas",       /*55*/
    "Uriel",        /*56*/
    "Naves",     /*57*/
    "Vitor Pontes",     /*58*/
    "Xavier",       /*59*/
    "Santaguida",       /*60*/
    "Soneca",       /*61*/
    "Jesus",        /*62*/
    "Yan",          /*63*/
    "Yamin",        /*64*/
    "Xonas",        /*65*/
    "Vinícius",         /*66*/
    "Vasconcelos",           /*67*/
    "Zacarias",     /*68*/
    "Moutinho",     /*69*/
    "Ximenes",      /*70*/
    "João Gabriel", /*71*/
    "Zanina",       /*72*/
    "Bruno Helder",         /*73*/
    "Alchieri",     /*74*/
    "Davi",       /*75*/
    "Wladimir",     /*76*/
    "Vitinho",        /*77*/
    "Rebores",      /*78*/
    "Mafra",        /*79*/
    "Tiago Cabral",        /*80*/
    "Alex Souza",   /*81*/
    "Caio",         /*82*/
    "Otto",         /*83*/
    "Batista",      /*84*/
    "Zohan",        /*85*/
    "Leozinho",          /*86*/
    "Khalil",       /*87*/
    "Buda",         /*88*/
    "Kratos",       /*89*/
    "Antônio",      /*90*/
    "Xerxes",       /*91*/
    "Thiaguinho",       /*92*/
    "Andrey",       /*93*/
    "Oswaldo",      /*94*/
    "Maranhão",     /*95*/
    "Fillype",     /*96*/
    "Babilônico",       /*97*/
    "Pedro",        /*98*/
    "Juju"          /*99*/
};

char female_names[100][15] = {
    "Anitta",   /*0*/
    "Afrodite",    /*1*/
    "Amanda",      /*2*/
    "Madonna",      /*3*/
    "Ana",         /*4*/
    "Betty",     /*5*/
    "Bella",        /*6*/
    "Bernadete",   /*7*/
    "Carol",      /*8*/
    "Caterina",    /*9*/
    "Alice",     /*10*/
    "Léia",   /*11*/
    "Patrícia",  /*12*/
    "Vanessa",       /*13*/
    "Cris",    /*14*/
    "Dafne",       /*15*/
    "Nayara",       /*16*/
    "Denise",      /*17*/
    "Genaína",    /*18*/
    "Carla",    /*19*/
    "Dolores",     /*20*/
    "Edna",        /*21*/
    "Elizabeth",   /*22*/
    "Érica",       /*23*/
    "Clara",     /*24*/
    "Fátima",      /*25*/
    "Flores",        /*26*/
    "Velma",   /*27*/
    "Gabi",        /*28*/
    "Gertrudes",   /*29*/
    "Padmé",       /*30*/
    "Rey",     /*31*/
    "Irina",       /*32*/
    "Emma Stone",      /*33*/
    "Jennifer",      /*34*/
    "Jaqueline",   /*35*/
    "Joelma",      /*36*/
    "Scarllet",      /*37*/
    "Alyson",    /*38*/
    "Jussara",     /*39*/
    "Luísa",       /*40*/
    "Arya",       /*41*/
    "Lana",        /*42*/
    "Karen",       /*43*/
    "Kátia",       /*44*/
    "Kelly",       /*45*/
    "Katarina",    /*46*/
    "Kyara",       /*47*/
    "Lisa",        /*48*/
    "Sarah Connor",    /*49*/
    "Maria",       /*50*/
    "Mulan",     /*51*/
    "Morgana",     /*52*/
    "Nádia",       /*53*/
    "Carolina",    /*54*/
    "Norma",       /*55*/
    "Nina",        /*56*/
    "Angelina Joeli",       /*57*/
    "Anne Hathaway",      /*58*/
    "Rihanna",      /*59*/
    "Ygritte",      /*60*/
    "Paty",    /*61*/
    "Penélope",    /*62*/
    "Priscila",    /*63*/
    "Uma Thurman",     /*64*/
    "Renata",      /*65*/
    "Rosa",        /*66*/
    "Mia Wallace",     /*67*/
    "Ellie",        /*68*/
    "Sofia",       /*69*/
    "Sabrina",     /*70*/
    "Xuxa",        /*71*/
    "Samara",      /*72*/
    "Socorro",     /*73*/
    "Solange",     /*74*/
    "Susana",      /*75*/
    "Daenerys",      /*76*/
    "Lagertha",      /*77*/
    "Bianca",      /*78*/
    "Terezinha",   /*79*/
    "Padmé",      /*80*/
    "Rapunzel",    /*81*/
    "Vitória",   /*82*/
    "Mariana",     /*83*/
    "Viviana",     /*84*/
    "Violeta",     /*85*/
    "Welwitschia", /*86*/
    "Jill Valentine",       /*87*/
    "Fiona",        /*88*/
    "Yasmin",      /*89*/
    "Zahra",       /*90*/
    "Zulmira",     /*91*/
    "Cersei",     /*92*/
    "Zoraida",     /*93*/
    "Dilma",       /*94*/
    "Hermione",    /*95*/
    "Elizabeth Swan",      /*96*/
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
    cast_arg[i].status = 0;
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

  children_waiting = number_childrenfemale + number_childrenmale;
  female_waiting = number_adultfemale;
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

/*
Argumentos de cada Barco
status = estado atual do barco (0 - resgatando, 1 - esperando na ilha , 2 - terminou resgate)
*/
void *boat_rescuing(void *arg){
  ptr_rescue_boat_arg rescue_boat_arg = (ptr_rescue_boat_arg)arg;

  while(rescue_boat_arg->status!=2 && number_alive != 0){
    printf("\nBarco de Resgate %d: Saindo do continente em direção a ilha\n\n", rescue_boat_arg->id);
    sleep(15);
    boat_waiting = TRUE;
    rescue_boat_arg->status = 1;
    if(number_alive == 0){
      printf("\nBarco de Resgate %d: Cheguei na ilha, mas infelizmente não existe mais ninguém aqui\n\n", rescue_boat_arg->id);
      sleep(2);
    }
    else{
      printf("\nBarco de Resgate %d: Cheguei na ilha, esperando náufragos subirem no barco...\n\n", rescue_boat_arg->id);
    }
    sem_wait(&wait_boat);
    printf("\nBarco de Resgate %d: Voltando ao continente!\n\n", rescue_boat_arg->id);
    if(number_alive == 0){
      rescue_boat_arg->status = 2;
    }
    else{
      rescue_boat_arg->status = 0;
      sleep(15);
      printf("\nBarco de Resgate %d: Cheguei no continente, descarregando os náufragos em segurança!\n\n", rescue_boat_arg->id);
      sleep(2);
    }
  }
  pthread_exit(0);
}

void remove_waiting(int sex){
  if(sex == 1){
    female_waiting--;
  }
  else if(sex == 2 || sex == 3){
    children_waiting--;
  }
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
        number_alive--;
        cast_arg[i].status = 2;
        remove_waiting(cast_arg[i].sex);
        found_id = cast_arg[i].id;
        break;
      }
    }
  }
  if(!found){
    for(i = 0; i < someone; i++){
      if(cast_arg[i].status == 0){
        if(cast_arg[i].id != id){
          number_alive--;
          cast_arg[i].status = 2;
          remove_waiting(cast_arg[i].sex);
          found_id = cast_arg[i].id;
          break;
        }
      }
    }
  }
  return found_id;
}

void children_castaway(ptr_castaway_arg castaway_arg){

  pthread_mutex_lock(&l);
  if(castaway_arg->status == 0){
    if(boat_waiting){
      printf("Náufrago %d (%s - CRIANÇA): Estou na fila do barco!\n", castaway_arg->id, castaway_arg->name);
      sleep(1);
      capacity--;
      number_alive--;
      castaway_arg->status = 1;
      remove_waiting(castaway_arg->sex);
      printf("Náufrago %d (%s - CRIANÇA): Entrei no barco, estou salvo! -- restam %d vagas no barco\n", castaway_arg->id, castaway_arg->name, capacity);
      if(capacity == 0){
        boat_waiting = FALSE;
        capacity = BOAT_CAPACITY;
        printf("Náufrago %d (%s - CRIANÇA): Galera... o barco lotou\n", castaway_arg->id, castaway_arg->name);
        sem_post(&wait_boat);
      }
      else if(number_alive == 0){
        boat_waiting = FALSE;
        printf("Não restam náufragos na ilha\n");
        sem_post(&wait_boat);
      }
      pthread_cond_broadcast(&cm);
      pthread_cond_broadcast(&cw);
    }
  }
  pthread_mutex_unlock(&l);
}

void woman_castaway(ptr_castaway_arg castaway_arg){

  pthread_mutex_lock(&l);
  if(castaway_arg->status == 0){
    if(boat_waiting){
      printf("Náufrago %d (%s - MULHER): Estou na fila do barco!\n", castaway_arg->id, castaway_arg->name);
      sleep(1);
    }
    while (children_waiting > 0 && boat_waiting){
      pthread_cond_wait(&cw, &l);
    }

    if(boat_waiting){
      capacity--;
      number_alive--;
      castaway_arg->status = 1;
      remove_waiting(castaway_arg->sex);
      printf("Náufrago %d (%s - MULHER): Entrei no barco, estou salvo! -- restam %d vagas no barco\n", castaway_arg->id, castaway_arg->name, capacity);
      if(capacity == 0){
        boat_waiting = FALSE;
        capacity = BOAT_CAPACITY;
        printf("Náufrago %d (%s - MULHER): Galera... o barco lotou\n", castaway_arg->id, castaway_arg->name);
        sem_post(&wait_boat);
      }
      else if(number_alive == 0){
        boat_waiting = FALSE;
        printf("Não restam náufragos na ilha\n");
        sem_post(&wait_boat);
      }
      pthread_cond_broadcast(&cm);
      pthread_cond_broadcast(&cw);
    }
  }
  pthread_mutex_unlock(&l);
}

void man_castaway(ptr_castaway_arg castaway_arg){

  pthread_mutex_lock(&l);
  if(castaway_arg->status == 0){
    if(boat_waiting){
      printf("Náufrago %d (%s - HOMEM): Estou na fila do barco!\n", castaway_arg->id, castaway_arg->name);
      sleep(1);
    }
    while ((children_waiting > 0 || female_waiting > 0) && boat_waiting){
      pthread_cond_wait(&cm, &l);
    }

    if(boat_waiting){
      capacity--;
      number_alive--;
      castaway_arg->status = 1;
      remove_waiting(castaway_arg->sex);
      printf("Náufrago %d (%s - HOMEM): Entrei no barco, estou salvo! -- restam %d vagas no barco\n", castaway_arg->id, castaway_arg->name, capacity);
      if(capacity == 0){
        boat_waiting = FALSE;
        capacity = BOAT_CAPACITY;
        printf("Náufrago %d (%s - HOMEM): Galera... o barco lotou\n" , castaway_arg->id, castaway_arg->name);
        sem_post(&wait_boat);
      }
      else if(number_alive == 0){
        boat_waiting = FALSE;
        printf("Não restam náufragos na ilha\n");
        sem_post(&wait_boat);
      }
      pthread_cond_broadcast(&cm);
      pthread_cond_broadcast(&cw);
    }
  }
  pthread_mutex_unlock(&l);
}

/*
Seguindo os indices:
i = estado atual do naufrago (0 - naufragado, 1 - resgatado , 2 - morto por outro naufrago, 3 - morto de fome)
i + 1 = sexo e idade do naufrago (0 - Homem Adulto, 1 - Mulher Adulta, 2 - Homem Criança, 3 - Mulher Criança)
i + 2 = numero entre 0 e 99 equivalente ao seu nome 
*/
void *surviving(void *arg) {
  ptr_castaway_arg castaway_arg = (ptr_castaway_arg)arg;

  while(castaway_arg->status == 0){

    /* Náufragos esperando para subir no barco */
    if(boat_waiting){
      switch (castaway_arg->sex){
        case 0:
          man_castaway(castaway_arg);
          break;

        case 1:
          woman_castaway(castaway_arg);
          break;

        case 2:
          children_castaway(castaway_arg);
          break;

        case 3:
          children_castaway(castaway_arg);
          break;

        default:
          break;
      }
    }

    /* Náufragos comendo e se matando */
    pthread_mutex_lock(&l);
    if(castaway_arg->status == 0 && !boat_waiting){
      printf("Náufrago %d (%s): Vou comer... ainda existem %d porções de comida\n", castaway_arg->id, castaway_arg->name, comida);
      sleep(1);
      comida-=EAT;
      if(comida == 0){
        printf("Náufrago %d (%s): EITA... existem %d porções de comida... alguém precisa ser sacrificado!\n", castaway_arg->id, castaway_arg->name, comida);
        id_kill = kill_someone(castaway_arg->id);
        if(id_kill == castaway_arg->id){
          printf("Infelizmente, não tem mais ninguém...\n");
          number_alive--;
          castaway_arg->status = 3;
          remove_waiting(castaway_arg->sex);
          sem_post(&wait_boat);
          boat_waiting = FALSE;
          sleep(2);
          printf("Náufrago %d (%s) morreu de fome!\n", castaway_arg->id, castaway_arg->name);
        }
        else{
          printf("Náufrago %d (%s) foi morto por Náufrago %d (%s)... conseguiu-se %d porções de comida a mais!\n", cast_arg[id_kill].id, cast_arg[id_kill].name, castaway_arg->id, castaway_arg->name, HUMAN_FOOD);
          comida += HUMAN_FOOD;
        }
      }
    }

    pthread_mutex_unlock(&l);
    sleep(1);
  }
  pthread_exit(0);
}

void initialize_food(){
  srand(time(NULL));
  comida = (rand() % 6) * 100;
  if (comida == 0){
    comida = 20;
  }
}

int shipwreck(){
  int i;

  number_alive = CASTAWAYS;
  capacity = BOAT_CAPACITY;
  boat_waiting = FALSE;

  printf("-------------------------\n");
  printf("\nACIDENTE!!!!!\n\n");
  printf("Um cruzeiro naufragou no oceano atlântico...\n");
  printf("Os %d sobreviventes nadaram até uma ilha próxima e aguardam resgate...\n", CASTAWAYS);
  initialize_food();
  printf("Um total equivalente à %d porções de comida foi levado dos restos do navio até a ilha...\n", comida);
  printf("Quantos sobreviverão???\n\n");

  initialize_castaways();
  print_castaways();

  for(i = 0; i < CASTAWAYS; i++){
    pthread_create(&castaway[i], NULL, surviving, (void *)(&(cast_arg[i])));
  }

  boat_arg.id = 0;
  boat_arg.status = 0;
  pthread_create(&boat, NULL, boat_rescuing, (void *)(&(boat_arg)));

  for (i = 0; i < CASTAWAYS; i++){
    pthread_join(castaway[i], NULL);
  }
  
  pthread_join(boat, NULL);

  print_castaways();

  return 0;
}

int main(){
  int continue_simulation = TRUE;
  char input;

  sem_init(&wait_boat, 0, 0);

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
