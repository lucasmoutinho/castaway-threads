/*
ALUNO: LUCAS DA SILVA MOUTINHO - 15/0015747
PROFESSOR: ALCHIERI
TRABALHO DE PC - CASTAWAY PROBLEM - PROGRAMAÇÃO CONCORRENTE - 2/2018
*/


/* 
Bibliotecas
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define TRUE 1          /* verdadeiro */
#define FALSE 0         /* falso */
#define CASTAWAYS 17    /* Número inicial de Náufragos na ilha */
#define HUMAN_FOOD 200  /* Quantidade de porções que um Náufrago morto adulto gera */
#define KID_FOOD 100    /* Quantidade de porções que um Náufrago morto criança gera */
#define EAT 20          /* Quantidade de porções comidas por vez por cada náufrago */
#define BOAT_CAPACITY 3 /* Número de vagas do barco */

/*
Cores
*/
#define COLOR_BLACK "\x1b[30m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_BRIGHT_RED "\x1b[91m"
#define COLOR_BRIGHT_GREEN "\x1b[92m"
#define COLOR_BRIGHT_YELLOW "\x1b[93m"
#define COLOR_BRIGHT_BLUE "\x1b[94m"
#define COLOR_BRIGHT_MAGENTA "\x1b[95m"
#define COLOR_BRIGHT_CYAN "\x1b[96m"
#define COLOR_RESET "\x1b[0m"

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
  
  int xuxa;
  int kills;
  int eaten;
  int line;
  int mvp;
} castaway_arg, *ptr_castaway_arg;

/*
Argumentos de cada Barco
status = estado atual do barco (0 - resgatando, 1 - esperando na ilha , 2 - terminou resgate)
*/
typedef struct{
  int id;
  int status;
} rescue_boat_arg, *ptr_rescue_boat_arg;

castaway_arg cast_arg[CASTAWAYS];                                                     /* Argumento dos Náufragos */
rescue_boat_arg boat_arg;                                                             /* Argumento do barco */
sem_t wait_boat;                                                                      /* Semáforos para o barco */
pthread_t castaway[CASTAWAYS];                                                        /* Thread dos Náufragos */
pthread_t boat;                                                                       /* Thread do barco */
pthread_mutex_t l = PTHREAD_MUTEX_INITIALIZER;                                        /* lock pros náufragos*/
pthread_mutex_t line_lock = PTHREAD_MUTEX_INITIALIZER;                                /* lock pra fila do barco*/
pthread_cond_t cm = PTHREAD_COND_INITIALIZER;                                         /* condicional pros náufragos homens adultos - man*/
pthread_cond_t cw = PTHREAD_COND_INITIALIZER;                                         /* condicional pros náufragos mulheres adultos - woman*/
int comida = 0;                                                                       /* Comida */
int number_adultmale, number_adultfemale, number_childrenmale, number_childrenfemale; /* Número de náufragos de cada sexo e idade */
int children_island, female_island;                                                   /* Número de Náufragos ainda na ilha */
int female_on_line, man_on_line, boat_waiting;                                        /* Homens, mulheres e barco esperando */
int number_alive = CASTAWAYS;                                                         /* Número de Náufragos vivos na ilha */
int capacity = BOAT_CAPACITY;                                                         /* Número de vagas restantes no barco */
int id_kill;                                                                          /* ID do Náufrago que morreu */
int used_male_names[100], used_female_names[100];                                     /* Vetor de nomes usados */
int oac, pc, first_left, last_standing, first_blood, first_killer, almost, peace;     /* ID para as Variáveis de conquista */
int bolso, pt, joke, mvp;                                                             /* ID para as Variáveis de conquista */

/*
Lista de nomes masculinos
*/
char male_names[100][15] = {
    "Artur",          /*0*/
    "Alfredo",        /*1*/
    "Almeida",        /*2*/
    "Giamps",         /*3*/
    "Senhor Picles",  /*4*/
    "Ladeira",        /*5*/
    "Daniels",        /*6*/
    "Dé",             /*7*/
    "Diogo",          /*8*/
    "Batman",         /*9*/
    "Samurouts",      /*10*/
    "Cláudio",        /*11*/
    "Vidal",          /*12*/
    "Jadiel",         /*13*/
    "Cabrinha",       /*14*/
    "Bolsonaro",      /*15*/
    "Rodrigo Chaves", /*16*/
    "Moutres",        /*17*/
    "Diegod",         /*18*/
    "Luís",           /*19*/
    "Allison",        /*20*/
    "Fagner",         /*21*/
    "Fred",           /*22*/
    "Geraldo",        /*23*/
    "Silvio Santos",  /*24*/
    "Galvão",         /*25*/
    "Capoeira",       /*26*/
    "Kipman",         /*27*/
    "Igor Batata",    /*28*/
    "Ícaro",          /*29*/
    "João",           /*30*/
    "Jeziel",         /*31*/
    "Rodolfo",        /*32*/
    "Jarbas",         /*33*/
    "Kevin",          /*34*/
    "Lucão",          /*35*/
    "Jureg",          /*36*/
    "Mesquita",       /*37*/
    "Marco Véi",      /*38*/
    "Marcola",        /*39*/
    "Kilmer",         /*40*/
    "Nathan",         /*41*/
    "Billy",          /*42*/
    "Ribas",          /*43*/
    "Nicholas",       /*44*/
    "Nichobolas",     /*45*/
    "Perotto",        /*46*/
    "Sodré",          /*47*/
    "Renato",         /*48*/
    "Rafael",         /*49*/
    "Halbe",          /*50*/
    "Rebores",        /*51*/
    "Struct",         /*52*/
    "Tuts",           /*53*/
    "Thaleco",        /*54*/
    "Wilson",         /*55*/
    "Uriel",          /*56*/
    "Navão",          /*57*/
    "Vitor Pontes",   /*58*/
    "Xotex",          /*59*/
    "Guidinha",       /*60*/
    "Soneca",         /*61*/
    "Jesus",          /*62*/
    "Yan",            /*63*/
    "Yamin",          /*64*/
    "Rofl",           /*65*/
    "Vinícius",       /*66*/
    "Vasconcelos",    /*67*/
    "Tecmec",         /*68*/
    "Engenet",        /*69*/
    "Ximenes",        /*70*/
    "João Gabriel",   /*71*/
    "Porto",          /*72*/
    "Bruno Helder",   /*73*/
    "Alchieri",       /*74*/
    "Davi",           /*75*/
    "Wladimir",       /*76*/
    "Vitinho",        /*77*/
    "Rebores",        /*78*/
    "Mafra",          /*79*/
    "Tiago Cabral",   /*80*/
    "Alex Souza",     /*81*/
    "Caieras",        /*82*/
    "Otto",           /*83*/
    "Batista",        /*84*/
    "Zohan",          /*85*/
    "Leozinho",       /*86*/
    "Khalil",         /*87*/
    "Buda",           /*88*/
    "Kratos",         /*89*/
    "Antônio",        /*90*/
    "Shikusu",        /*91*/
    "Thiaguinho",     /*92*/
    "Andrey",         /*93*/
    "Oswaldo",        /*94*/
    "Maranhão",       /*95*/
    "Phelps",         /*96*/
    "Babilônico",     /*97*/
    "Pedro",          /*98*/
    "Juju"            /*99*/
};

/*
Lista de nomes femininos
*/
char female_names[100][15] = {
    "Anitta",      /*0*/
    "Afrodite",    /*1*/
    "Amanda",      /*2*/
    "Madonna",     /*3*/
    "Ana",         /*4*/
    "Rebbeca",     /*5*/
    "Bella",       /*6*/
    "Bernadete",   /*7*/
    "Carolzinha",  /*8*/
    "Cremosa",     /*9*/
    "Alice",       /*10*/
    "Léia",        /*11*/
    "Patrícia",    /*12*/
    "Débora",      /*13*/
    "Cris",        /*14*/
    "Dafne",       /*15*/
    "Nayara",      /*16*/
    "Denise",      /*17*/
    "Genaína",     /*18*/
    "Carla",       /*19*/
    "Dolores",     /*20*/
    "Edna",        /*21*/
    "Elizabeth",   /*22*/
    "Érica",       /*23*/
    "Clara",       /*24*/
    "Fátima",      /*25*/
    "Flores",      /*26*/
    "Velma",       /*27*/
    "Gabi",        /*28*/
    "Gertrudes",   /*29*/
    "Padmé",       /*30*/
    "Rey",         /*31*/
    "Irina",       /*32*/
    "Emma",        /*33*/
    "Jennifer",    /*34*/
    "Jaqueline",   /*35*/
    "Joelma",      /*36*/
    "Scarllet",    /*37*/
    "Jandáia",     /*38*/
    "Jussara",     /*39*/
    "Luísa",       /*40*/
    "Arya",        /*41*/
    "Lana",        /*42*/
    "Karen",       /*43*/
    "Kátia",       /*44*/
    "Kelly",       /*45*/
    "Katarina",    /*46*/
    "Kyara",       /*47*/
    "Focátia",     /*48*/
    "Sarah",       /*49*/
    "Maria",       /*50*/
    "Mulan",       /*51*/
    "Morgana",     /*52*/
    "Nádia",       /*53*/
    "Carolina",    /*54*/
    "Norma",       /*55*/
    "Rapunzel",    /*56*/
    "Angelina",    /*57*/
    "Madalena",    /*58*/
    "Rihanna",     /*59*/
    "Ygritte",     /*60*/
    "Paty",        /*61*/
    "Penélope",    /*62*/
    "Priscila",    /*63*/
    "Andrômeda",   /*64*/
    "Renata",      /*65*/
    "Rosa",        /*66*/
    "Mia Wallace", /*67*/
    "Ellie",       /*68*/
    "Sofia",       /*69*/
    "Sabrina",     /*70*/
    "Xuxa",        /*71*/
    "Samara",      /*72*/
    "Socorro",     /*73*/
    "Solange",     /*74*/
    "Susana",      /*75*/
    "Daenerys",    /*76*/
    "Lagertha",    /*77*/
    "Bianca",      /*78*/
    "Terezinha",   /*79*/
    "Padmé",       /*80*/
    "Rapunzel",    /*81*/
    "Vitória",     /*82*/
    "Mariana",     /*83*/
    "Viviana",     /*84*/
    "Violeta",     /*85*/
    "Welwitschia", /*86*/
    "Jill",        /*87*/
    "Fiona",       /*88*/
    "Yasmin",      /*89*/
    "Zahra",       /*90*/
    "Zulmira",     /*91*/
    "Cersei",      /*92*/
    "Zoraida",     /*93*/
    "Dilma",       /*94*/
    "Hermione",    /*95*/
    "Alcione",     /*96*/
    "Eliana",      /*97*/
    "Creusa",      /*98*/
    "Cremilda"     /*99*/
};

/*
Função para limpar a tela do terminal
*/
void clrscr(){
  system("clear");
}

/*
Função para imprimir na tela do terminal o status de cada náufrago
*/
void print_status(int status){
  
  switch (status){
    case 0:
      printf(COLOR_BRIGHT_YELLOW "NAUFRAGADO" COLOR_RESET);
      break;

    case 1:
      printf(COLOR_BRIGHT_GREEN "RESGATADO" COLOR_RESET);
      break;

    case 2:
      printf(COLOR_BRIGHT_RED "MORTO - Virou comida!" COLOR_RESET);
      break;

    case 3:
      printf(COLOR_BRIGHT_MAGENTA "MORTO - Morreu de fome!" COLOR_RESET);
      break;

    default:
      break;
  }
}

void initialize_used_names(){
  int i;

  for(i = 0; i < 100; i++){
    used_female_names[i] = 0;
    used_male_names[i] = 0;
  }
}

/*
Seguindo os indices:
i = estado atual do naufrago (0 - naufragado, 1 - resgatado , 2 - morto por outro naufrago, 3 - morto de fome)
i + 1 = sexo e idade do naufrago (0 - Homem Adulto, 1 - Mulher Adulta, 2 - Homem Criança, 3 - Mulher Criança)
i + 2 = numero entre 0 e 99 equivalente ao seu nome 

Esta função inicializa os náufragos com seus respectivos argumentos
*/
void initialize_castaways(){
  int length = CASTAWAYS;
  int i, name, j;

  number_adultfemale = 0;
  number_adultmale = 0;
  number_childrenfemale = 0;
  number_childrenmale = 0;
  initialize_used_names();

  srand(time(NULL));

  for(i = 0; i < length; i++){
    cast_arg[i].id = i;
    cast_arg[i].status = 0;
    cast_arg[i].sex = (rand() % 3);
    if(cast_arg[i].sex == 2){
      cast_arg[i].sex = (rand() % 2) + 2;
    }
    cast_arg[i].kills = 0;
    cast_arg[i].xuxa = 0;
    cast_arg[i].eaten = 0;
    cast_arg[i].line = 0;
    cast_arg[i].mvp = 0;
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
      while(used_male_names[name] == 1){
        name = (rand() % 100);
      }
      while(male_names[name][j] != '\0'){
        cast_arg[i].name[j] = male_names[name][j];
        j++;
      }
      used_male_names[name] = 1;
    }
    else{
      while(used_female_names[name] == 1){
        name = (rand() % 100);
      }
      while(female_names[name][j] != '\0'){
        cast_arg[i].name[j] = female_names[name][j];
        j++;
      }
      used_female_names[name] = 1;
    }
    cast_arg[i].name[j] = '\0';
  }

  children_island = number_childrenfemale + number_childrenmale;
  female_island = number_adultfemale;
}

/*
Retorna o número máximo de xuxas
*/
int top_xuxa(){
  int top = -1;
  int i;
  int length = CASTAWAYS;

  for(i = 0; i < length; i++){
    if(cast_arg[i].xuxa > top){
      top = cast_arg[i].xuxa;
    }
  }
  return top;
}

/*
Retorna o número máximo de mortes
*/
int top_kills(){
  int top = -1;
  int i;
  int length = CASTAWAYS;

  for(i = 0; i < length; i++){
    if(cast_arg[i].kills > top){
      top = cast_arg[i].kills;
    }
  }
  return top;
}

/*
Retorna o número máximo de mortes feitos por uma criança
*/
int top_kid_kills(){
  int top = -1;
  int i;
  int length = CASTAWAYS;

  for(i = 0; i < length; i++){
    if(cast_arg[i].kills > top && (cast_arg[i].sex == 2 || cast_arg[i].sex == 3)){
      top = cast_arg[i].kills;
    }
  }
  return top;
}

/*
Retorna o número máximo de porções comidas
*/
int top_eaten(){
  int top = -1;
  int i;
  int length = CASTAWAYS;

  for(i = 0; i < length; i++){
    if(cast_arg[i].eaten > top){
      top = cast_arg[i].eaten;
    }
  }
  return top;
}

/*
Retorna o número máximo de vezes entrado na fila de espera do barco
*/
int top_line(){
  int top = -1;
  int i;
  int length = CASTAWAYS;

  for(i = 0; i < length; i++){
    if(cast_arg[i].line > top){
      top = cast_arg[i].line;
    }
  }
  return top;
}

/*
Retorna o que mais ganhou conquistas
*/
int top_mvp(){
  int top = -1;
  int i;
  int length = CASTAWAYS;

  for(i = 0; i < length; i++){
    if(cast_arg[i].mvp > top){
      top = cast_arg[i].mvp;
    }
  }
  return top;
}

/*
Função para imprimir na tela a lista de náufragos
Caso o modo seja 1, imprime uma lista de conquistas
*/
void print_castaways(int mode){
  int length = CASTAWAYS;
  int i, top, found = FALSE;

  printf(COLOR_BRIGHT_CYAN "-------------------------\n");
  printf("LISTA DOS NÁUFRAGOS\n\n" COLOR_RESET);
  printf("Homens Adultos: %d -- Mulheres Adultas: %d -- Crianças: %d\n\n", number_adultmale, number_adultfemale, number_childrenfemale + number_childrenmale);

  for (i = 0; i < length; i++){
    printf(COLOR_BRIGHT_YELLOW "Náufrago %d: %s " COLOR_RESET, cast_arg[i].id, &cast_arg[i].name[0]);

    switch (cast_arg[i].sex){
      case 0:
        printf("(Homem) ");
        break;

      case 1:
        printf("(Mulher) ");
        break;

      case 2:
        printf("(Criança H) ");
        break;

      case 3:
        printf("(Criança M) ");
        break;

      default:
        break;
    }

    printf("- ");
    print_status(cast_arg[i].status);
    printf("\n");
  }
  printf(COLOR_BRIGHT_CYAN "-------------------------\n\n" COLOR_RESET);

  printf("\n\nPRESSIONE ENTER PARA CONTINUAR\n");
  getchar();

  if(mode){
    printf("\n\n");
    printf(COLOR_BRIGHT_YELLOW "***********" COLOR_RESET);
    printf(" ACHIEVEMENTS (CONQUISTAS) ");
    printf(COLOR_BRIGHT_YELLOW "***********\n\n\n" COLOR_RESET);

    /* O primeiro a entrar no barco */
    if(first_left != -1){
      printf("Primeiro a entrar no barco - ");
      printf(COLOR_BRIGHT_CYAN "O Precoce: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[first_left].id, cast_arg[first_left].name);
      cast_arg[first_left].mvp++;
    }

    /* O primeiro a matar */
    if(first_killer != -1){
      printf("Primeiro a matar - ");
      printf(COLOR_BRIGHT_RED "Quero ver o circo pegar fogo: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[first_killer].id, cast_arg[first_killer].name);
      cast_arg[first_killer].mvp++;
    }

    /* O primeiro a morrer */
    if(first_blood != -1){
      printf("Primeiro a morrer - ");
      printf(COLOR_BRIGHT_CYAN "Pô...achei vacilo: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[first_blood].id, cast_arg[first_blood].name);
      cast_arg[first_blood].mvp++;
    }
    
    /* O que mais matou */
    top = top_kills();
    found = FALSE;
    if(top > 0){
      printf("O náufrago que mais matou - ");
      printf(COLOR_BRIGHT_RED "Só na facadinha! (%d Mortes): ", top);
      for(i = 0; i < length; i++){
        if(cast_arg[i].kills == top){
          printf("-- Náufrago %d (%s) -- ", cast_arg[i].id, cast_arg[i].name);
          cast_arg[i].mvp++;
        }
        if(cast_arg[i].kills > 0 && cast_arg[i].eaten == 0){
          found = TRUE;
        }
      }
      printf("\n" COLOR_RESET);
    }

    /* Matou, mas não comeu ninguém */
    if(found){
      printf("Matou uma galera, mas não comeu ninguém - ");
      printf(COLOR_BRIGHT_MAGENTA "O Serial Killer Vegano: ");
      for(i = 0; i < length; i++){
        if(cast_arg[i].kills > 0 && cast_arg[i].eaten == 0){
          printf("-- Náufrago %d (%s - %d mortes) -- ", cast_arg[i].id, cast_arg[i].name, cast_arg[i].kills);
          cast_arg[i].mvp++;
        }
      }
      printf("\n" COLOR_RESET);
    }

    /* Criança que mais matou */
    top = top_kid_kills();
    if(top > 0){
      printf("A criança que mais matou - ");
      printf(COLOR_BRIGHT_YELLOW "Aquele sobrinho bizarro! (%d Mortes): ", top);
      for(i = 0; i < length; i++){
        if(cast_arg[i].kills == top && (cast_arg[i].sex == 2 || cast_arg[i].sex == 3)){
          printf("-- Náufrago %d (%s) -- ", cast_arg[i].id, cast_arg[i].name);
          cast_arg[i].mvp++;
        }
      }
      printf("\n" COLOR_RESET);
    }

    /* O que mais matou crianças */
    top = top_xuxa();
    if(top > 0){
      printf("O que mais matou crianças - ");
      printf(COLOR_BRIGHT_MAGENTA "Rainha dos baixinhos! (%d Mortes): ", top);
      for(i = 0; i < length; i++){
        if(cast_arg[i].xuxa == top){
          printf("-- Náufrago %d (%s) -- ", cast_arg[i].id, cast_arg[i].name);
          cast_arg[i].mvp++;
        }
      }
      printf("\n" COLOR_RESET);
    }

    /* O que mais comeu */
    found = FALSE;
    top = top_eaten();
    if(top > 0){
      printf("O que mais comeu - ");
      printf(COLOR_BRIGHT_YELLOW "O canibal enrustido (%d Porções): ", top);
      for(i = 0; i < length; i++){
        if(cast_arg[i].eaten == top){
          printf("-- Náufrago %d (%s) -- ", cast_arg[i].id, cast_arg[i].name);
          cast_arg[i].mvp++;
          if(cast_arg[i].kills == 0){
            found = TRUE;
          }
        }
      }
      printf("\n" COLOR_RESET);
    }

    /* O que mais comeu mas não matou ninguém */
    if(found){
      printf("Foi o que mais comeu, mas não matou ninguém - ");
      printf(COLOR_BRIGHT_YELLOW "Eu tenho mãos delicadas!: ");
      for(i = 0; i < length; i++){
        if(cast_arg[i].eaten == top && cast_arg[i].kills == 0){
          printf("-- Náufrago %d (%s) -- ", cast_arg[i].id, cast_arg[i].name);
          cast_arg[i].mvp++;
        }
      }
      printf("\n" COLOR_RESET);
    }

    /* O que mais entrou na fila antes de ser resgatado ou morto*/
    top = top_line();
    if(top > 0){
      printf("O que entrou na fila mais vezes antes de ser resgatado ou morto - ");
      printf(COLOR_BRIGHT_MAGENTA "O mais rejeitado do bonde (%d Tentativas): ", top);
      for(i = 0; i < length; i++){
        if(cast_arg[i].line == top){
          printf("-- Náufrago %d (%s) -- ", cast_arg[i].id, cast_arg[i].name);
          cast_arg[i].mvp++;
        }
      }
      printf("\n" COLOR_RESET);
    }

    /* Quem matou o Vidal */
    if(oac != -1){
      printf("Matou o Vidal - ");
      printf(COLOR_BRIGHT_CYAN "Odeio OAC: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[oac].id, cast_arg[oac].name);
      cast_arg[oac].mvp++;
    }

    /* Quem matou o Alchieri */
    if(pc != -1){
      printf("Matou o Alchieri - ");
      printf(COLOR_BRIGHT_YELLOW "O moleque que reprovou PC: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[pc].id, cast_arg[pc].name);
      cast_arg[pc].mvp++;
    }

    /* Quem matou a Ladeira */
    if(joke != -1){
      printf("Matou o Ladeira - ");
      printf(COLOR_BRIGHT_YELLOW "Não gosto de piada: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[joke].id, cast_arg[joke].name);
      cast_arg[joke].mvp++;
    }

    /* Quem matou o Bolsonaro */
    if(bolso != -1){
      printf("Matou o Bolsonaro - ");
      printf(COLOR_BRIGHT_RED "A verdadeira facadinha: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[bolso].id, cast_arg[bolso].name);
      cast_arg[bolso].mvp++;
    }

    /* Quem matou a Dilma */
    if(pt != -1){
      printf("Matou a Dilma - ");
      printf(COLOR_BRIGHT_RED "Impeachment: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[pt].id, cast_arg[pt].name);
      cast_arg[pt].mvp++;
    }

    /* Foi resgatado logo antes de morrer de fome */
    if(almost){
      printf("Foi resgatado logo antes de morrer de fome - ");
      printf(COLOR_BRIGHT_GREEN "Aquela Cagada Matinal!: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[last_standing].id, cast_arg[last_standing].name);
      cast_arg[last_standing].mvp++;
    }

    /* Foi resgatado logo antes de morrer de fome */
    if(peace){
      printf("Foi o último a ser resgatado, não matou e não comeu ninguém - ");
      printf(COLOR_BRIGHT_CYAN "O Maior de todos os Santos: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[last_standing].id, cast_arg[last_standing].name);
      cast_arg[last_standing].mvp++;
    }

    /* O último a sair da ilha (vivo ou morto) */
    if(last_standing != -1){
      printf("O Último a sair da ilha (vivo ou morto) - ");
      printf(COLOR_BRIGHT_GREEN "Last Man Standing: -- Náufrago %d (%s) --\n" COLOR_RESET, cast_arg[last_standing].id, cast_arg[last_standing].name);
      cast_arg[last_standing].mvp++;
    }

    /* O que mais ganhou conquistas */
    top = top_mvp();
    if(top > 0){
      printf("\n\n\n");
      printf(COLOR_BRIGHT_GREEN "Most Valuable Player - MVP (%d Conquistas): ", top);
      for(i = 0; i < length; i++){
        if(cast_arg[i].mvp == top){
          printf("-- Náufrago %d (%s) -- ", cast_arg[i].id, cast_arg[i].name);
        }
      }
      printf("\n" COLOR_RESET);
    }

    printf("\n\nPRESSIONE ENTER PARA CONTINUAR\n");
    getchar();
  }
}

/*
Argumentos de cada Barco
status = estado atual do barco (0 - resgatando, 1 - esperando na ilha , 2 - terminou resgate)

Esta função representa o comportamento da thread barco
*/
void *boat_rescuing(void *arg){
  ptr_rescue_boat_arg rescue_boat_arg = (ptr_rescue_boat_arg)arg;

  while(rescue_boat_arg->status!=2 && number_alive > 0){
    printf(COLOR_BRIGHT_CYAN "\nBarco de Resgate %d: Saindo do continente em direção a ilha\n\n" COLOR_RESET, rescue_boat_arg->id);
    sleep(15);
    boat_waiting = TRUE;
    rescue_boat_arg->status = 1;
    if(number_alive == 0){
      printf(COLOR_BRIGHT_CYAN "\nBarco de Resgate %d: Cheguei na ilha, mas infelizmente não existe mais ninguém aqui...\n\n" COLOR_RESET, rescue_boat_arg->id);
      sleep(2);
    }
    else{
      printf(COLOR_BRIGHT_CYAN "\nBarco de Resgate %d: Cheguei na ilha, esperando náufragos subirem no barco...\n\n" COLOR_RESET, rescue_boat_arg->id);
    }
    sem_wait(&wait_boat);
    printf(COLOR_BRIGHT_CYAN "\nBarco de Resgate %d: Voltando ao continente!\n\n" COLOR_RESET, rescue_boat_arg->id);
    if(number_alive == 0){
      rescue_boat_arg->status = 2;
    }
    else{
      rescue_boat_arg->status = 0;
      sleep(15);
      printf(COLOR_BRIGHT_CYAN "\nBarco de Resgate %d: Cheguei no continente, descarregando os náufragos em segurança!\n\n" COLOR_RESET, rescue_boat_arg->id);
      sleep(2);
    }
  }
  pthread_exit(0);
}

/*
Função para diminuir o número de mulheres e crianças totais na ilha
*/
void remove_waiting(int sex){
  if(sex == 1){
    female_island--;
  }
  else if(sex == 2 || sex == 3){
    children_island--;
  }
}

/*
Função para printar mensagens de easter egg
*/
void easter_egg_printer(int dead_id, int killer_id){
  if(first_blood == -1){
    first_blood = dead_id;
  }
  if(first_killer == -1){
    first_killer = killer_id;
  }
  if(dead_id != killer_id){
    if(strcmp(cast_arg[dead_id].name, "Vidal") == 0){
      oac = killer_id;
    }
    else if(strcmp(cast_arg[dead_id].name, "Alchieri") == 0){
      pc = killer_id;
    }
    else if(strcmp(cast_arg[dead_id].name, "Dilma") == 0){
      pt = killer_id;
    }
    else if(strcmp(cast_arg[dead_id].name, "Bolsonaro") == 0){
      bolso = killer_id;
    }
    else if(strcmp(cast_arg[dead_id].name, "Ladeira") == 0){
      joke = killer_id;
    }
  }
  if(strcmp(cast_arg[dead_id].name, "Alcione") == 0){
    printf(COLOR_BRIGHT_YELLOW "Mas tem que me prender! TEEEEEMMM que seduziirrr!\n" COLOR_RESET);
  }
  else if(strcmp(cast_arg[dead_id].name, "Focátia") == 0){
    printf(COLOR_BRIGHT_YELLOW "Ai meu deus roubaram a Focátia!\n" COLOR_RESET);
  }
  else if(strcmp(cast_arg[dead_id].name, "Cabrinha") == 0){
    printf(COLOR_BRIGHT_YELLOW "Béééé!\n" COLOR_RESET);
  }
  else if(strcmp(cast_arg[dead_id].name, "Guidinha") == 0){
    printf(COLOR_BRIGHT_YELLOW "Sempre te admireo!\n" COLOR_RESET);
  }
  else if(strcmp(cast_arg[dead_id].name, "Silvio Santos") == 0){
    printf(COLOR_BRIGHT_YELLOW "Não vai ganhar aviãozinho\n" COLOR_RESET);
  }
  else if(strcmp(cast_arg[dead_id].name, "Galvão") == 0){
    printf(COLOR_BRIGHT_YELLOW "Hoje Não!....Hoje Sim!\n" COLOR_RESET);
  }
  else if(strcmp(cast_arg[dead_id].name, "Bolsonaro") == 0){
    printf(COLOR_BRIGHT_YELLOW "TCHÁÁÁÁÁÁÁÁÁÁ!\n" COLOR_RESET);
  }
  else if(strcmp(cast_arg[dead_id].name, "Wilson") == 0){
    printf(COLOR_BRIGHT_YELLOW "WILLLSOOOONNNN!\n" COLOR_RESET);
  }
  else if(strcmp(cast_arg[dead_id].name, "Kratos") == 0){
    printf(COLOR_BRIGHT_YELLOW "BOYYYYYY!\n" COLOR_RESET);
  }
}

/*
Função para escolher um náufrago restante na ilha randomicamente e matá-lo 
*/
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
        cast_arg[id].kills++;
        if(cast_arg[i].sex == 2 || cast_arg[i].sex == 3){
          cast_arg[id].xuxa++;
        }
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
          cast_arg[id].kills++;
          if(cast_arg[i].sex == 2 || cast_arg[i].sex == 3){
            cast_arg[id].xuxa++;
          }
          break;
        }
      }
    }
  }
  return found_id;
}

/*
Função de comportamento das threads de crianças na fila de espera do barco
*/
void children_castaway(ptr_castaway_arg castaway_arg){
  pthread_mutex_lock(&line_lock);
  if(castaway_arg->status == 0){
    if(boat_waiting){
      printf(COLOR_BRIGHT_YELLOW "Náufrago %d (%s - CRIANÇA): Estou na fila do barco!\n" COLOR_RESET, castaway_arg->id, castaway_arg->name);
      sleep(1);
      castaway_arg->line++;
      capacity--;
      number_alive--;
      castaway_arg->status = 1;
      remove_waiting(castaway_arg->sex);
      if(first_left == -1){
        first_left = castaway_arg->id;
      }
      printf(COLOR_BRIGHT_GREEN "Náufrago %d (%s - CRIANÇA): Entrei no barco, estou salvo! -- restam %d vagas no barco\n" COLOR_RESET, castaway_arg->id, castaway_arg->name, capacity);
      if(capacity == 0){
        boat_waiting = FALSE;
        capacity = BOAT_CAPACITY;
        printf(COLOR_BRIGHT_MAGENTA "Náufrago %d (%s - CRIANÇA): Galera... o barco lotou\n" COLOR_RESET, castaway_arg->id, castaway_arg->name);
        if(number_alive == 0){
          last_standing = castaway_arg->id;
          if(castaway_arg->kills == 0 && castaway_arg->eaten == 0){
            peace = TRUE;
          }
        }
        sem_post(&wait_boat);
      }
      else if(number_alive == 0){
        boat_waiting = FALSE;
        printf(COLOR_BRIGHT_MAGENTA "Não restam náufragos na ilha\n" COLOR_RESET);
        sleep(2);
        last_standing = castaway_arg->id;
        if(castaway_arg->kills == 0 && castaway_arg->eaten == 0){
          peace = TRUE;
        }
        if(capacity == BOAT_CAPACITY-1){
          almost = TRUE;
        }
        sem_post(&wait_boat);
      }
      pthread_cond_broadcast(&cm);
      pthread_cond_broadcast(&cw);
    }
  }
  pthread_mutex_unlock(&line_lock);
}

/*
Função de comportamento das threads de mulheres adultas na fila de espera do barco
*/
void woman_castaway(ptr_castaway_arg castaway_arg){
  pthread_mutex_lock(&line_lock);
  if(castaway_arg->status == 0){
    if(boat_waiting){
      printf(COLOR_BRIGHT_YELLOW "Náufrago %d (%s - MULHER): Estou na fila do barco!\n" COLOR_RESET, castaway_arg->id, castaway_arg->name);
      sleep(1);
      castaway_arg->line++;
    }
    while (children_island > 0 && boat_waiting){
      pthread_cond_wait(&cw, &line_lock);
    }

    if(boat_waiting && castaway_arg->status == 0){
      capacity--;
      number_alive--;
      castaway_arg->status = 1;
      remove_waiting(castaway_arg->sex);
      if(first_left == -1){
        first_left = castaway_arg->id;
      }
      printf(COLOR_BRIGHT_GREEN "Náufrago %d (%s - MULHER): Entrei no barco, estou salvo! -- restam %d vagas no barco\n" COLOR_RESET, castaway_arg->id, castaway_arg->name, capacity);
      if(capacity == 0){
        boat_waiting = FALSE;
        capacity = BOAT_CAPACITY;
        printf(COLOR_BRIGHT_MAGENTA "Náufrago %d (%s - MULHER): Galera... o barco lotou\n" COLOR_RESET, castaway_arg->id, castaway_arg->name);
        if(number_alive == 0){
          last_standing = castaway_arg->id;
          if(castaway_arg->kills == 0 && castaway_arg->eaten == 0){
            peace = TRUE;
          }
        }
        sem_post(&wait_boat);
      }
      else if(number_alive == 0){
        boat_waiting = FALSE;
        printf(COLOR_BRIGHT_MAGENTA "Não restam náufragos na ilha\n" COLOR_RESET);
        sleep(2);
        last_standing = castaway_arg->id;
        if(castaway_arg->kills == 0 && castaway_arg->eaten == 0){
          peace = TRUE;
        }
        if(capacity == BOAT_CAPACITY-1){
          almost = TRUE;
        }
        sem_post(&wait_boat);
      }
      pthread_cond_broadcast(&cm);
      pthread_cond_broadcast(&cw);
    }
  }
  pthread_mutex_unlock(&line_lock);
}

/*
Função de comportamento das threads de homens adultos na fila de espera do barco
*/
void man_castaway(ptr_castaway_arg castaway_arg){
  pthread_mutex_lock(&line_lock);
  if(castaway_arg->status == 0){
    if(boat_waiting){
      printf(COLOR_BRIGHT_YELLOW "Náufrago %d (%s - HOMEM): Estou na fila do barco!\n" COLOR_RESET, castaway_arg->id, castaway_arg->name);
      sleep(1);
      castaway_arg->line++;
    }
    while ((children_island > 0 || female_island > 0) && boat_waiting){
      pthread_cond_wait(&cm, &line_lock);
    }

    if(boat_waiting && castaway_arg->status == 0){
      capacity--;
      number_alive--;
      castaway_arg->status = 1;
      remove_waiting(castaway_arg->sex);
      if(first_left == -1){
        first_left = castaway_arg->id;
      }
      printf(COLOR_BRIGHT_GREEN "Náufrago %d (%s - HOMEM): Entrei no barco, estou salvo! -- restam %d vagas no barco\n" COLOR_RESET, castaway_arg->id, castaway_arg->name, capacity);
      if(capacity == 0){
        boat_waiting = FALSE;
        capacity = BOAT_CAPACITY;
        printf(COLOR_BRIGHT_MAGENTA "Náufrago %d (%s - HOMEM): Galera... o barco lotou\n" COLOR_RESET, castaway_arg->id, castaway_arg->name);
        if(number_alive == 0){
          last_standing = castaway_arg->id;
          if(castaway_arg->kills == 0 && castaway_arg->eaten == 0){
            peace = TRUE;
          }
        }
        sem_post(&wait_boat);
      }
      else if(number_alive == 0){
        boat_waiting = FALSE;
        printf(COLOR_BRIGHT_MAGENTA "Não restam náufragos na ilha\n" COLOR_RESET);
        sleep(2);
        last_standing = castaway_arg->id;
        if(castaway_arg->kills == 0 && castaway_arg->eaten == 0){
          peace = TRUE;
        }
        if(capacity == BOAT_CAPACITY-1){
          almost = TRUE;
        }
        sem_post(&wait_boat);
      }
      pthread_cond_broadcast(&cm);
      pthread_cond_broadcast(&cw);
    }
  }
  pthread_mutex_unlock(&line_lock);
}

/*
Seguindo os indices:
i = estado atual do naufrago (0 - naufragado, 1 - resgatado , 2 - morto por outro naufrago, 3 - morto de fome)
i + 1 = sexo e idade do naufrago (0 - Homem Adulto, 1 - Mulher Adulta, 2 - Homem Criança, 3 - Mulher Criança)
i + 2 = numero entre 0 e 99 equivalente ao seu nome 

Esta função representa o comprotamento das threads de náufragos na ilha, seja esperando o barco
ou dividindo o recurso das comidas e matando outros náufragos para gerar mais comida
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
      if(comida > 0){
        printf("Náufrago %d (%s): Vou comer... ainda existem %d porções de comida\n", castaway_arg->id, castaway_arg->name, comida);
        sleep(1);
        comida-=EAT;
        castaway_arg->eaten+=EAT;
      }
      else{
        printf(COLOR_BRIGHT_MAGENTA "Náufrago %d (%s): EITA... existem %d porções de comida... alguém precisa ser sacrificado!\n" COLOR_RESET, castaway_arg->id, castaway_arg->name, comida);
        id_kill = kill_someone(castaway_arg->id);
        if(id_kill == castaway_arg->id){
          printf(COLOR_BRIGHT_MAGENTA "Infelizmente, não tem mais ninguém...\n" COLOR_RESET);
          sleep(2);
          number_alive--;
          castaway_arg->status = 3;
          remove_waiting(castaway_arg->sex);
          sem_post(&wait_boat);
          boat_waiting = FALSE;
          printf(COLOR_BRIGHT_RED "Náufrago %d (%s) morreu de fome!\n" COLOR_RESET, castaway_arg->id, castaway_arg->name);
          last_standing = castaway_arg->id;
        }
        else{
          if(cast_arg[id_kill].sex == 2 || cast_arg[id_kill].sex == 3){
            printf(COLOR_BRIGHT_RED "Náufrago %d (%s) foi morto por Náufrago %d (%s)... Conseguiu-se %d porções de comida a mais! -- restam: %d\n" COLOR_RESET, cast_arg[id_kill].id, cast_arg[id_kill].name, castaway_arg->id, castaway_arg->name, KID_FOOD, number_alive);
            comida += KID_FOOD;
          }
          else{
            printf(COLOR_BRIGHT_RED "Náufrago %d (%s) foi morto por Náufrago %d (%s)... Conseguiu-se %d porções de comida a mais! -- restam: %d\n" COLOR_RESET, cast_arg[id_kill].id, cast_arg[id_kill].name, castaway_arg->id, castaway_arg->name, HUMAN_FOOD, number_alive);
            comida += HUMAN_FOOD;
          }
        }
        easter_egg_printer(id_kill, castaway_arg->id);
        sleep(1);
      }
    }

    pthread_mutex_unlock(&l);
    sleep(1);
  }
  pthread_exit(0);
}

/*
Função para gerar a porção de comida inicial na ilha
*/
void initialize_food(){
  srand(time(NULL));
  comida = (rand() % 6) * 100;
  if (comida == 0){
    comida = 20;
  }
}

void initialize_globals(){
  number_alive = CASTAWAYS;
  capacity = BOAT_CAPACITY;
  boat_waiting = FALSE;
  oac = -1;
  pc = -1;
  bolso = -1;
  pt = -1;
  joke = -1;
  first_left = -1;
  last_standing = -1;
  first_killer = -1;
  first_blood = -1;
  almost = FALSE;
  peace = FALSE;
  man_on_line = 0;
  female_on_line = 0;
}

/*
Função que inicializa as threads barco e náufragos e inicia a simulação de naufrágio
*/
int shipwreck(){
  int i;

  initialize_globals();
  sem_init(&wait_boat, 0, 0);

  printf(COLOR_BRIGHT_CYAN "-------------------------\n");
  printf("\nACIDENTE!!!!!\n\n" COLOR_RESET);
  printf("Um cruzeiro naufragou no oceano atlântico...\n");
  printf("Os %d sobreviventes nadaram até uma ilha próxima e aguardam resgate...\n", CASTAWAYS);
  initialize_food();
  printf("Um total equivalente à %d porções de comida foi levado dos restos do navio até a ilha...\n", comida);
  printf(COLOR_BRIGHT_RED "Quantos sobreviverão???\n\n" COLOR_RESET);

  initialize_castaways();
  print_castaways(0);

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

  print_castaways(1);

  return 0;
}

/*
Função main. Permanece em um loop de acordo com a resposta escrita pelo usuário no terminal
*/
int main(){
  int continue_simulation = TRUE;
  char input;


  printf(COLOR_BRIGHT_CYAN "-------------------------\n");
  printf("\nCASTAWAY PROBLEM\n\n");
  printf("-------------------------\n\n" COLOR_RESET);

  
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
