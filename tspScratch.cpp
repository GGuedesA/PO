#include <stdio.h> // print, fgets function
#include <stdlib.h> // exit, atoi function
#include <string.h> //strlen function
#include <math.h> // sqrt function
#include <algorithm>    // std::min
#include <limits.h> // INT_MAX
#include <time.h> //time(NULL) usado apra inicializar a semente aleatoria

int calculateTourDistance(int *tour); //calculo da qualidade da solucao (fitness)
void tweak(int *tour, int i, int j); //modifica uma dada solucao (encontra uma solucao vizinha)
void readFile(const char *inputFile); //le o arquivo que foi passado como parametro e armazena os valores em distanceMatrix
void greedySearch(int* tour );
void simulatedAnnealing(int* tour );

int **distanceMatrix, size=-1, pos=0;



int main(const int argc, const char **inputFile){
    // if(argc != 2){ //verifica se foi passado o nome do arquivo (o primeiro argumento em C e o nome do executavel)
	// 	fprintf(stderr,"use: tspScratch <tsp file>\n\n");
	// 	exit(1);
	// }else{
    //     readFile(inputFile[1]);
	// }
	readFile("eil51.tsp");//passa direto o nome do arquivo, ao inves de usar linha de comando
    
//    Mostra a distanceMatrix
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++)
            printf("%d ", distanceMatrix[i][j]);
        printf("\n");
    }

//    Cria um tour percorrendo as cidades em ordem e mostra o tamanho desse tour    
	int tour[size];
	for(int i=0; i<size; i++)
		tour[i]=i;
    printf("Tamanho %d\n", calculateTourDistance(tour));

	
    
    printf("Tamanho antes da busca gulosa: %d\n", calculateTourDistance(tour));
    
    
//Busca gulosa
    //Fazer uma busca gulosa partindo da cidade 0, depois da 1, depois da 2 ...
    //Salvar melhor resultado encontrado nessa etapa
	greedySearch(tour);
	printf("Tamanho depois da busca gulosa: %d\n", calculateTourDistance(tour));
	


// 	Simulated Annealing
    //Usar a melhor solução encontrada no estagio anterior como solucao inicial do Simulated Annealing
    //Implementar o Simulated annealing, com atencao ao detalhe de que aqui estamos tentando minimizar a distancia total, no algoritmo mostrado, estamos tentando maximizar a qualidade

    simulatedAnnealing(tour);
    printf("Melhor caminho encontrado: %d\n", calculateTourDistance(tour));


}

void tweak(int *tour, int i, int j){
    //Implementar
    //Gerar dois numeros aleatorios entre 0 e o numero de cidades, garantir que esses dois numeros sejam diferentes, eles serao indices digamos x e y
    //Trocar os valores que estao na posicao x e y do vetor tour
    int temp = tour[i];
    tour[i] = tour[j];
    tour[j] = temp;
}



int calculateTourDistance(int *tour){ //calculo da qualidade da solucao (fitness)
	int dist=0;
	for(int i=0; i<size-1; i++){
		dist+=distanceMatrix[tour[i]][tour[i+1]];
	}
	dist+=distanceMatrix[tour[size-1]][tour[0]];
	return dist;
}

void greedySearch(int *tour) {
  // Melhor solução encontrada até o momento.
  int bestDistance = INT_MAX;

  // Para cada cidade como ponto de partida
  for (int i = 0; i < size; i++) {
    // Inicializar o tour com a cidade atual
    for (int j = 0; j < size; j++) {
      tour[j] = j;
    }
    tour[0] = i;

    // Para cada cidade restante
    for (int j = 1; j < size; j++) {
      // Encontrar a cidade mais próxima da cidade atual
      int closestCity = -1;
      int minDistance = INT_MAX;
      for (int k = 0; k < size; k++) {
        if (tour[j - 1] != k) {
          int distance = distanceMatrix[tour[j - 1]][k];
          if (distance < minDistance) {
            closestCity = k;
            minDistance = distance;
          }
        }
      }

      // Adicionar a cidade mais próxima ao tour
      tour[j] = closestCity;
    }

    // Atualizar a melhor solução encontrada
    int distance = calculateTourDistance(tour);
    if (distance < bestDistance) {
      bestDistance = distance;
      memcpy(tour, tour, size * sizeof(int));
    }
  }

  // Imprimir a melhor solução encontrada
  printf("Melhor solucao encontrada pelo algoritmo guloso: %d\n", bestDistance);
}

void simulatedAnnealing(int *tour) {
  // Temperatura inicial.
  int temperature = 10;

  // Taxa de resfriamento.
  double coolingRate = 0.9;

  // Melhor solução encontrada até o momento.
  int bestDistance = INT_MAX;

  // Inicializar o tour com a melhor solução encontrada pelo algoritmo guloso
  memcpy(tour, tour, size * sizeof(int));

  // Loop principal do algoritmo.
  while (temperature > 0) {
    // Gera uma vizinhança.
    tweak(tour, rand() % size, rand() % size);

    // Calcula a diferença de energia.
    int deltaEnergy = calculateTourDistance(tour) - calculateTourDistance(tour);

    // Aceita a mudança com probabilidade.
    if (deltaEnergy < 0) {
      // A mudança é melhor, então sempre aceitamos.
    } else {
      // A mudança é pior, então aceitamos com probabilidade de e^(-deltaEnergy/temperature).
      double probability = exp(-deltaEnergy / temperature);
      if (rand() % 1000 < probability * 1000) {
        // Aceitamos a mudança.
      } else {
        // Rejeitamos a mudança e restauramos o estado anterior.
        memcpy(tour, tour, size * sizeof(int));
      }
    }

    // Diminuir a temperatura.
    temperature *= coolingRate;

    // Atualizar a melhor solução encontrada
    int distance = calculateTourDistance(tour);
    if (distance < bestDistance) {
      bestDistance = distance;
      memcpy(tour, tour, size * sizeof(int));
    }
  }

  // Imprimir a melhor solução encontrada
  printf("Melhor solucao encontrada pelo algoritmo simulated annealing: %d\n", bestDistance);
}
void readFile(const char *inputFile){ //le o arquivo que foi passado como parametro e armazena os valores em distanceMatrix

    double *x, *y;
	char type[10]="";

	char s[500];
	FILE *stream = fopen(inputFile, "r"); //read file (read-only permission)
	if( stream == NULL ){ // if the file was not read, error
		fprintf(stderr,"\nFail to Open tsp File!!\n");
		exit(1);
	}
	while(fgets(s,500,stream)){ //read every line (not larger than 500 chars) of the input
		if(strlen(s)-1>0) //remove the last break line of the line
			s[strlen(s)-1]='\0';
		if(('\r'==s[strlen(s)-1]))//in some files there is a carriage return at the end, don't know why. This command removes it
			s[strlen(s)-1]=0;

		char* value1 = strtok(s," "); //creating sub-strings separated by space
		char* value2 = strtok(NULL," ");
		char* value3 = strtok(NULL," ");

		if(!strcmp(value1, "EDGE_WEIGHT_TYPE:")){ //verify if the instance is of type EUC_2D CEIL_2D or, ATT, only the calculation for these types were implemented
			if(strcmp(value2, "EUC_2D") && strcmp(value2, "ATT") && strcmp(value2, "CEIL_2D")){
				fprintf(stderr,"\nERROR! tsp file is of type \"%s\" only EUC_2D, ATT or CEIL_2D are supported. Aborting!!\n", value2);
				exit(1);
			}
		}

		if(!strcmp(value1, "EDGE_WEIGHT_TYPE")){ //verify if the instance is of type EUC_2D CEIL_2D or, ATT, only the calculation for these types were implemented
			if(strcmp(value3, "EUC_2D") && strcmp(value3, "ATT") && strcmp(value3, "CEIL_2D")){
				fprintf(stderr,"\nERROR! tsp file is not of type EUC_2D, ATT or CEIL_2D aborting!!\n");
				exit(1);
			}else
				strcpy(type,value3);
		}

		if(!strcmp(value1, "TYPE") && (strcmp(value3, "TSP")) ){ //verify if the instance is of type TSP, the other types will not be considered
			fprintf(stderr,"\nERROR! tsp file is not of type TSP, aborting!!\n");
			exit(1);
		}

		if(!strcmp(value1, "DIMENSION")){//read the dimension from the header and allocate memory for the cities
			size = atoi(value3);
			distanceMatrix=(int**)malloc(size * sizeof(int*));
			x=(double*)malloc(size * sizeof(double*));
			y=(double*)malloc(size * sizeof(double*));

			for (int i=0;i<size;i++)
				distanceMatrix[i] = (int*)malloc(size * sizeof(int));
		}

		if(atoi(value1)){//if the first substring is a number, the list of cities started
			if(size==-1){//if the size was not set, it was not in the header, error
				fprintf(stderr,"\nERROR! Dimension not set in file header!!\n");
				exit(1);
			}
			x[pos]=atof(value2);//storing the values from the file in the arrays
			y[pos]=atof(value3);
			pos++;
		}
// 		free(x);
//         free(y);
	}

	if(!strcmp(type, "EUC_2D")){
		for(int i=0; i<size; i++){
			for(int j=0; j<size; j++){
				double xd = x[i]-x[j];
				double yd = y[i]-y[j];
				double dist=sqrt(xd*xd+yd*yd);
				distanceMatrix[i][j]=(int)(dist+0.5);//calculating the euclidean distance, rounding to int and storing in the distance matrix
			}
		}
	}else if(!strcmp(type, "CEIL_2D")){
		for(int i=0; i<size; i++){
			for(int j=0; j<size; j++){
				double xd = x[i]-x[j];
				double yd = y[i]-y[j];
				double dist=sqrt(xd*xd+yd*yd);
				distanceMatrix[i][j]=(int)(dist+0.000000001);//calculating the euclidean distance, rounding to int and storing in the distance matrix
			}
		}
	}else if(!strcmp(type, "ATT")){
		for(int i=0; i<size; i++){
			for(int j=0; j<size; j++){
				double xd = x[i]-x[j];
				double yd = y[i]-y[j];
				double rij=sqrt((xd*xd+yd*yd)/10.0);
				double tij=(int)(rij+0.5);
				if(tij<rij)
					distanceMatrix[i][j]=tij+1;
				else
					distanceMatrix[i][j]=tij;
			}
		}
	}
}
