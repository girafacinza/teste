#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

typedef struct
{
	char pista[17];
	int qtd;
	char vetpalavras[3][17];
	char descpalavras[3][17];
	char descpalavrasFinal[3][17];
	float premios[12];
} Linhas;

Linhas ler_linhas(void);
void ler_premios(float[]);
float num_sorteio(float[]);
int prepara(Linhas*);
void placar (Linhas,float[], int);
int verificaLetra (Linhas*, char);
int verificaPalavra (Linhas);

int main(){
	float jogador[3]= {0,0,0}, rodar;
	int i=0, j, qtd_letras, acerto, qtd_acertos=0, contFinal=0, vencedor=-1,k=0;
	char letraescolhida;
	Linhas linhas;
	
	linhas = ler_linhas();
	qtd_letras = prepara(&linhas);

	FILE *arq;
	
	arq = fopen("premios.dat", "rb"); // arq é a variavel que retorna a abertura binária do premios.dat 
	if(arq == NULL) {
		printf("ERRO AO ABRIR O ARQUIVO!\n");
		exit(2);
		}
	while ( i<12 ){
		fscanf(arq, "%f\n", &linhas.premios[i]);
		i++;
	}
	
	fclose(arq); //fechar o arquivo
	i=0;
	
	while ( qtd_letras > qtd_acertos + 3 ) {
		placar(linhas,jogador,vencedor);	
		printf("\nJogador %d, Roda a roda!",i+1);
	  	fflush(stdin);
		getchar();
		rodar = num_sorteio(linhas.premios);

		if (rodar == 0){
			printf("\nPASSOU A VEZ!\n");
			i = (i+1)%3; //vai para próximo jogador
      		printf("A vez sera passada para o jogador %d.\nPressione qualquer tecla.", i+1);
      		fflush(stdin);
			getchar(); //espera o usuario clicar em qualquer tecla
		}
	
		else if(rodar == 0.01f){
			printf("\nPERDEU TUDO!\nPressione qualquer tecla.");
			jogador[i] = 0; //zerar placar do jogador ativo
			i = (i+1)%3; //vai para próximo jogador
			fflush(stdin);
			getchar(); 
		}
		else { 
			do{ //perguntar letra até o usuário digitar um caracter válido
				printf("Uma letra por R$%.2f: ", rodar);
				fflush(stdin);
				scanf("%c", &letraescolhida);
				
			} while ( letraescolhida < 'A' || ( letraescolhida > 'Z' && letraescolhida < 'a' ) || letraescolhida > 'z' ); 
			
			if ( letraescolhida >= 'a' && letraescolhida <= 'z') letraescolhida -= 32;
			
			acerto = verificaLetra (&linhas, letraescolhida); 
      
			if (acerto>0) {
				jogador[i] = jogador[i] + rodar*acerto;
				qtd_acertos += acerto; //armazena total de caracteres descobertos
				if ( qtd_letras - qtd_acertos == 0 ){ //jogador venceu antes da rodada final
							vencedor = i;
							system("cls");
							placar (linhas, jogador, vencedor );
							return 0;
				}
			}
			else if (acerto == -1){
        		i = (i+1)%3;//vai para próximo jogador
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
	placar(linhas,jogador,vencedor);	
	printf("\nJogador %d, Roda a roda!",i+1);
	fflush(stdin);
	getchar();
	do		rodar = num_sorteio(linhas.premios); //não aceita 'passa a vez' ou 'perde tudo' na última rodada
	while 	( rodar == 0.01f || rodar == 0 );
	printf("\nFalta(m) %d letra(s) para completar a palavra.\n", qtd_letras-qtd_acertos);
	printf("Voce tem 5 segundos para pensar e depois digitar a palavra...");
	for(j=5;j!=0; j--){ //contagem regressiva
		printf("%d...", j);
		Sleep(1000);
	} 
	printf("\n\nValendo R$ %.2f, a palavra eh: \n", rodar+jogador[i]);
	acerto = verificaPalavra(linhas);
	if (acerto == 1){ //jogador ganhou
		vencedor = i;
		jogador[i] += rodar;
      	system("cls");
		placar (linhas, jogador, vencedor );
		return 1;
	}
	else{
		i = (i+1)%3;
		contFinal++;
	}
    system("cls");
	}
	
	placar (linhas, jogador, vencedor );
	printf("\nNinguem acertou! Fim de jogo.\n");
			
return 2;
}

Linhas ler_linhas(void){	
	int qtd, i,j, sorteio;
	char pista[17], vetpalavras[3][17];
	Linhas linhas[5];
	FILE *arq;
	arq = fopen("palavras.dat", "rb");
	if (arq == NULL)
	{
		printf("\nERRO AO ABRIR O ARQUIVO\n");
 		exit(1);
	}
	for (j=0;j<5;j++)
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
			if (fscanf(arq, "%s", vetpalavras[i]) != 1)
			{
				printf("\nERRO AO ABRIR O ARQUIVO!\n");
 				exit(2);
      		}
    	}
		strcpy(linhas[j].pista, pista);
		linhas[j].qtd = qtd;
		for (i = 0; i < qtd; i++)
			strcpy(linhas[j].vetpalavras[i], vetpalavras[i]);
	}
  	srand(time(NULL));
	sorteio = rand()%5;
	fclose(arq);
	return linhas[sorteio];
}

float num_sorteio(float premios[]){ 
	float valor;
	srand(time(NULL));
	valor =  premios[rand()%12];
 	return valor;
}

int prepara(Linhas*linhas){ 
	int i, j, contagem=0, qtddLetras, qtddPalavras;
	qtddPalavras = (*linhas).qtd;
	for (i=0;i<qtddPalavras;i++) { //percorre todas as palavras
		qtddLetras = strlen((*linhas).vetpalavras[i]);
		for (j=0;j<qtddLetras;j++){ //percorre todas as letras até o tamanho de cada palavra
			(*linhas).descpalavras[i][j]='_';
			contagem++; //conta quantos caracteres existem no total das palavras
		}
	}
	return contagem;
}

void placar (Linhas linhas, float dinheiro_do_jogador[], int iVencedor ){
	int i, j;
	printf("\n\t\t\tA palavra esta associada com: %s\n\n\t\t\t", linhas.pista);
	for (i=0; i<linhas.qtd; i++) {
		for (j=0;j<strlen(linhas.vetpalavras[i]);j++){
			if ( iVencedor == -1 ) printf(" %c ", linhas.descpalavras[i][j]); 
			else printf(" %c ", linhas.vetpalavras[i][j]); //caso alguem vença, imprime o vetor original
		}
		printf("\n\t\t\t");
	}
	printf("\n\nJogador 1\t\tJogador 2\t\tJogador 3\n");
	printf("==========\t\t==========\t\t==========\n");
	printf("R$%8.2f\t\tR$%8.2f\t\tR$%8.2f\n",dinheiro_do_jogador[0],dinheiro_do_jogador[1],dinheiro_do_jogador[2]);
	switch (iVencedor){ //caso alguém vença o linhas
		case 0: printf(" VENCEDOR\n");
		break;
		case 1: printf("\t\t\t VENCEDOR\n");
		break;
		case 2: printf("\t\t\t\t\t\t VENCEDOR\n");
		break;
	}
}

int verificaLetra (Linhas *linhas, char letra){
	int i, j, acert=0;
	for (i=0; i<(*linhas).qtd ; i++){ //percorre todas as palavras
		for (j=0;j<strlen((*linhas).vetpalavras[i]);j++){ //percorre todas as letras até atingir o tamanho da palavra
    		if ((*linhas).descpalavras[i][j] == letra) return -1; //se a letra já foi descoberta, retorna -1
			else if ((*linhas).vetpalavras[i][j] == letra ) { //compara letra por letra da palavra original para ver se é igual
				(*linhas).descpalavras[i][j] = letra; //revela a letra na palavra descoberta
				acert++; //contagem para saber quantas letras foram descobertas
			} 
		}
	}
  return acert;
}

int verificaPalavra (Linhas linhas){
	int i, verif, acert=0;
	char palavrasFinal[3][17];
	for ( i=0 ; i<linhas.qtd ; i++ ){
		printf("A palavra %d eh: ", i+1);
		fflush(stdin);
		gets(palavrasFinal[i]);
		verif = strcmp ( strupr(palavrasFinal[i]),linhas.vetpalavras[i] );
		if ( verif == 0 ) acert++;
	}
	if ( acert == linhas.qtd ) return 1;
	else return 0;
}
