# castaway-threads
A castaway problem solved with threads

# Problematização
Este programa resolve um problema envolvendo threads e programação concorrente. A problematização desenvolvida envolve a situação fictícia de um acidente de cruzeiro no meio do oceano atlântico, este que acaba deixando um grupo de náufragos presos em uma ilha do atlântico. Estes náufragos, com o intuito de sobreviverem, disputam pelas porções de comida restantes. Entretanto, quando as porções acabam, matam algum dos náufragos restantes da ilha, gerando mais porções de comida.

Enquanto os náufragos tentam sobreviver e não morrerem de fome, um barco de resgate é acionado para buscar sobreviventes na ilha. Este sai do continente, vai até a ilha e espera os náufragos subirem para serem resgatados. Porém, o barco só consegue carregar 3 náufragos por vez. Dada a limitação do barco, existe uma preferência para quem será resgatado: crianças, depois mulheres e, por fim, homens. Enquanto carrega os sobreviventes de volta ao continente, o restante dos náufragos na ilha voltam a combaterem por comida.

Tantos os náufragos quanto o barco serão threads e a comida o rescurso compartilhado entre estas. São utilizados recursos como condicionais e semáforos para a correta cadência de ações e controle das threads.

# Para compilar e executar o programa
Execute o seguinte comando no terminal:

```
make
```

# Created by
Trabalho da disciplina de programação concorrente da UNB (Universidade de Brasília)

```
Lucas da Silva Moutinho - 15/0015747
```