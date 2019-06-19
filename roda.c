#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	char pista[17];
	int qtd;
	char vetpalavras[3][17];
	char descpalavras[3][17];
	char descpalavrasFinal[3][17];
	float premios[12];
} Roda;

//define uma struct com o nome Roda para armazenar a pista, quantidade de palavras para cada pista, palavras, palavras descobertas, a ultima palavra e o valor dos premios.

Roda ler_linhas(void); // declara funcao ler linhas
int prepara(Roda*); // declara a funcao prepara
int verificaLetra (Roda*, char);
int comparaPalavraFinal (Roda);
float num_sorteio(float[]); //declara a funcao que retorna um numero aleatorio entre 0 e 12
void placar (Roda,float[], int);

int main(){
	int i=0, j, qtd_letras, acerto, qtd_acertos=0, contFinal=0, vencedor=-1,k=0;
	float jogador[3]= {0,0,0}, play; //declara um vetor com 3 jogadores, cada um começa com 0
	char letraescolhida; 
	Roda roda;
	
	roda = ler_linhas(); // roda recebe o retorno da função ler linhas que retorna uma pista
	qtd_letras = prepara(&roda); // retorna quantos caracteres existem no total das palavras

	FILE *arq;
	
	arq = fopen("premios.dat", "rb"); // arq é a variavel que retorna a abertura binária do premios.dat 
	if(arq == NULL) {
		printf("ERRO AO ABRIR O ARQUIVO!\n");
		exit(2);
		} // teste de erro
	while ( i<12 ){
		fscanf(arq, "%f\n", &roda.premios[i]);
		i++;
	} //armazena todos os premios contidos no premios.dat em um vetor com 12 valores. cada i é uma linha diferente no arq (fscanf pega o que ta no arquivo, lê como float, e adiciona no roda.premios[i]).
	
	fclose(arq); //fechar o arquivo
	i=0; 
	
	while ( qtd_letras > qtd_acertos + 3 ) { 
		placar(roda,jogador,vencedor);	
		printf("\nJogador %d, Roda a roda! ",i+1);
	  	fflush(stdin);
		getchar();
		play = num_sorteio(roda.premios); // play recebe o valor sorteado no premio.dat

		if (play == 0){
			printf("\nPASSOU A VEZ!\n");
			i = (i+1)%3; //vai para próximo jogador
      		printf("A vez sera passada para o jogador %d.\nPressione qualquer tecla.", i+1);
      		fflush(stdin);
			getchar(); //espera o usuario clicar em qualquer tecla
		}
	
		else if(play == 0.01f){
			printf("\nPERDEU TUDO!\nPressione qualquer tecla.");
			jogador[i] = 0; //zerar placar do jogador ativo
			i = (i+1)%3; //vai para próximo jogador
			fflush(stdin);
			getchar(); 
		}
		else { 
			do{ //perguntar letra até o usuário digitar um caracter válido
				printf("Uma letra por R$%.2f: ", play);
				fflush(stdin);
				scanf("%c", &letraescolhida);
				
			} while ( letraescolhida < 'A' || ( letraescolhida > 'Z' && letraescolhida < 'a' ) || letraescolhida > 'z' ); 
			
			if ( letraescolhida >= 'a' && letraescolhida <= 'z') letraescolhida -= 32;
			
			acerto = verificaLetra (&roda, letraescolhida); 
      
			if (acerto>0) {
				jogador[i] = jogador[i] + play*acerto;
				qtd_acertos += acerto; //armazena total de caracteres descobertos
				if ( qtd_letras - qtd_acertos == 0 ){ //jogador venceu antes da rodada final
							vencedor = i;
							system("cls");
							placar (roda, jogador, vencedor );
							return 0;
				}
			}
			else if (acerto == -1){
        		i = (i+1)%3; //vai para próximo jogador, e só permite 3
        		printf("\nA letra %c ja foi descoberta.\nA vez sera passada para o jogador %d.\n\nPressione qualquer tecla.", letraescolhida, i+1);
        		fflush(stdin);
				getchar(); //espera o usuario clicar em qualquer tecla
     		}
			else{
        		i = (i+1)%3;//vai para próximo jogador
        		printf("\nNao existe letra %c na palavra.\nA vez sera passada para o jogador %d.\n\nPressione qualquer tecla.", letraescolhida, i+1);
        		fflush(stdin);
				getchar(); //espera o usuario clicar em qualquer tecla
			}
		}
      	system("cls"); //limpa a tela
	}

	while ( vencedor==-1 && contFinal<3 ){ //rodada final
	placar(roda,jogador,vencedor);	
	printf("\nJogador %d, Roda a roda!",i+1);
	fflush(stdin);
	getchar();
	do		play = num_sorteio(roda.premios); //não aceita 'passa a vez' ou 'perde tudo' na última rodada
	while 	( play == 0.01f || play == 0 );
	printf("\nFalta(m) %d letra(s) para completar a palavra.\n", qtd_letras-qtd_acertos);
	printf("Voce tem 5 segundos para pensar e depois digitar a palavra...");
	fflush(stdin);
	for(j=5;j!=0; j--){ //contagem regressiva
		printf("%d...", j);
		Sleep(1000);
	} 
	printf("\n\nValendo R$ %.2f, a palavra eh: \n", play+jogador[i]);
	acerto = comparaPalavraFinal(roda);
	if (acerto == 1){ //jogador ganhou
		vencedor = i;
		jogador[i] += play;
      	system("cls");
		placar (roda, jogador, vencedor );
		return 1;
	}
	else{
		i = (i+1)%3;
		contFinal++;
	}
    system("cls");
	}
	
	placar (roda, jogador, vencedor );
	printf("\nNinguem acertou! Fim de jogo.\n");
			
return 2;
}

//fim do main

//começa as funções:

Roda ler_linhas(void){	
	int qtd, i,j, sorteio;
	char pista[17], vetpalavras[3][17];
	Roda roda[5]; // define a struct roda como um vetor. Isso serve para poder armazenar em cada linha palavras diferentes, ou seja o vetor armazena 5 valores que representam as 5 linhas.
	FILE *arq;
	arq = fopen("palavras.dat", "rb");
	if (arq == NULL)
	{
		printf("\nERRO AO ABRIR O ARQUIVO\n"); //teste de erro
 		exit(1);
	}
	for (j=0;j<5;j++) // para cada linha
	{
		if (fscanf(arq,"%s %d", pista, &qtd) != 2) //primeiro faz a leitura do nome e da quantidade de palavras
		{ //se nao conseguiu ler os 2 campos
			if (feof(arq)) //é fim do arquivo ?
				printf("* ok, fim de arquivo\n");
			else
				printf("\nERRO AO ABRIR O ARQUIVO!\n");
			exit(0);
    	}
		for (i = 0; i < qtd; i++)
		{
			if (fscanf(arq, "%s", vetpalavras[i]) != 1)  // agora faz a leitura de cada palavra relacionada a pista
			{
				printf("\nERRO AO ABRIR O ARQUIVO!\n");
 				exit(2);
      		}
    	}
		strcpy(roda[j].pista, pista); // copia a pista gerada pelo fscanf para a struct
		roda[j].qtd = qtd; // copia a quantidade gerada pelo fscanf para a struct
		for (i = 0; i < qtd; i++)
			strcpy(roda[j].vetpalavras[i], vetpalavras[i]); // copia a as palavras (no máximo 3) geradas pelo fscanf
	}
  	srand(time(NULL));
	sorteio = rand()%5; //sorteia 1 entre 5 pistas
	fclose(arq);
	return roda[sorteio];
}

float num_sorteio(float premios[]){ //essa função simplesmente retorna um valor aleatorio entre 0 e 12. É usado para sortear os premios
	float valor;
	srand(time(NULL));
	valor =  premios[rand()%12];
 	return valor;
}

int prepara(Roda*roda){ 
	int i, j, contagem=0, qtddLetras, qtddPalavras;
	qtddPalavras = (*roda).qtd;
	for (i=0;i<qtddPalavras;i++) { //percorre todas as palavras
		qtddLetras = strlen((*roda).vetpalavras[i]);
		for (j=0;j<qtddLetras;j++){ //percorre todas as letras até o tamanho de cada palavra
			(*roda).descpalavras[i][j]='_';
			contagem++; //conta quantos caracteres existem no total das palavras
		}
	}
	return contagem;
}

void placar (Roda roda, float dinheiro_do_jogador[], int iVencedor ){
	int i, j;
	printf("\n\t\t\tA palavra esta associada com: %s\n\n\t\t\t\t", roda.pista);
	for (i=0; i<roda.qtd; i++) {
		for (j=0;j<strlen(roda.vetpalavras[i]);j++){
			if ( iVencedor == -1 ) printf(" %c ", roda.descpalavras[i][j]); 
			else printf(" %c ", roda.vetpalavras[i][j]); //caso alguem vença, imprime o vetor original
		}
		printf("\n\t\t\t\t");
	}
	printf("\n\nJogador 1\t\tJogador 2\t\tJogador 3\n");
	printf("==========\t\t==========\t\t==========\n");
	printf("R$ %.2f\t\t\tR$ %.2f\t\t\tR$ %.2f\n",dinheiro_do_jogador[0],dinheiro_do_jogador[1],dinheiro_do_jogador[2]);
	switch (iVencedor){ //caso alguém vença o roda
		case 0: printf(" VENCEDOR\n");
		break;
		case 1: printf("\t\t\t VENCEDOR\n");
		break;
		case 2: printf("\t\t\t\t\t\t VENCEDOR\n");
		break;
	}
}

int verificaLetra (Roda *roda, char letra){
	int i, j, acertos=0;
	for (i=0; i<(*roda).qtd ; i++){ //percorre todas as palavras
		for (j=0;j<strlen((*roda).vetpalavras[i]);j++){ //percorre todas as letras até atingir o tamanho da palavra
    		if ((*roda).descpalavras[i][j] == letra) return -1; //se a letra já foi descoberta, retorna -1
			else if ((*roda).vetpalavras[i][j] == letra ) { //compara letra por letra da palavra original para ver se é igual
				(*roda).descpalavras[i][j] = letra; //revela a letra na palavra descoberta
				acertos++; //contagem para saber quantas letras foram descobertas
			} 
		}
	}
  return acertos;
}

int comparaPalavraFinal (Roda roda){
	int i, verifica, acert=0;
	char palavrasFinal[3][17];
	for ( i=0 ; i<roda.qtd ; i++ ){
		printf("A palavra %d eh: ", i+1);
		fflush(stdin);
		gets(palavrasFinal[i]);
		verifica = strcmp ( strupr(palavrasFinal[i]),roda.vetpalavras[i] );
		if ( verifica == 0 ) acert++;
	}
	if ( acert == roda.qtd ) return 1;
	else return 0;
}
