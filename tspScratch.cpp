#include <stdio.h> // print, fgets function
#include <stdlib.h> // exit, atoi function
#include <string.h> //strlen function
#include <math.h> // sqrt function
#include <algorithm>    // std::min
#include <limits.h> // INT_MAX
#include <time.h> //time(NULL) usado apra inicializar a semente aleatoria

int calculateTourDistance(int *tour); //calculo da qualidade da solucao (fitness)
void tweak(int* tour, int i, int j); //modifica uma dada solucao (encontra uma solucao vizinha)
void readFile(const char *inputFile); //le o arquivo que foi passado como parametro e armazena os valores em distanceMatrix

int **distanceMatrix, size=-1, pos=0;

void greedySearch(int*tour) {
	// Inicializa o tour com a primeira cidade.
	tour[0] = 0;

	// Para cada cidade que ainda não foi visitada:
	for (int i = 1; i < size; i++) {
		// Encontra a cidade mais próxima da cidade atual.
		int closestCity = -1;
		int minDistance = INT_MAX;
		for (int j = 0; j < size; j++) {
			if (tour[j] == -1 && distanceMatrix[tour[i - 1]][j] < minDistance) {
				closestCity = j;
				minDistance = distanceMatrix[tour[i - 1]][j];
			}
		}

		// Adiciona a cidade mais próxima ao tour.
		tour[i] = closestCity;
	}
	printf("Busca Gulosa: Tamanho %d\n", size);
	int bestResult = calculateTourDistance(tour);
	printf("Busca Gulosa: Melhor Resultado %d\n", bestResult);
};

void simulatedAnnealing(int*tour) {
	// Inicializa a temperatura.
	int temperature = 1000;
	// Realiza iterações até que a temperatura chegue a zero
	while (temperature > 0) {
		// Gera uma solução vizinha.
		int neighborTour[size];
		memcpy(neighborTour, tour, size * sizeof(int));
		int i = rand() % size;
		int j = rand() % size;
		tweak(neighborTour, i, j);
		// Calcula a diferença de custo entre a solução atual e a solução vizinha.
		int deltaCost = calculateTourDistance(neighborTour) - calculateTourDistance(tour);
		// Aceita a solução vizinha com uma probabilidade de acordo com a temperatura.
		if (deltaCost < 0) {
			memcpy(tour, neighborTour, size * sizeof(int));
		} else {
			double probability = exp(-deltaCost / temperature);
			if (rand() / (RAND_MAX + 1.0) < probability) {
				memcpy(tour, neighborTour, size * sizeof(int));
			}
		}
		// Reduz a temperatura.
		temperature *= 0.9;
	}
};

int main(const int argc, const char **inputFile){
    // if(argc != 2){ //verifica se foi passado o nome do arquivo (o primeiro argumento em C e o nome do executavel)
	// 	fprintf(stderr,"use: tspScratch <tsp file>\n\n");
	// 	exit(1);
	// }else{
    //     readFile(inputFile[1]);
	// }
	readFile("tsp/eil51.tsp");//passa direto o nome do arquivo, ao inves de usar linha de comando

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


//Busca gulosa
    //Fazer uma busca gulosa partindo da cidade 0, depois da 1, depois da 2 ...
    //Salvar melhor resultado encontrado nessa etapa
	greedySearch(tour);


// 	Simulated Annealing
    //Usar a melhor solução encontrada no estagio anterior como solucao inicial do Simulated Annealing
    //Implementar o Simulated annealing, com atencao ao detalhe de que aqui estamos tentando minimizar a distancia total, no algoritmo mostrado, estamos tentando maximizar a qualidade
	simulatedAnnealing(tour);

}

void tweak(int *tour, int i, int j){
    //Implementar
    //Gerar dois numeros aleatorios entre 0 e o numero de cidades, garantir que esses dois numeros sejam diferentes, eles serao indices digamos x e y
    //Trocar os valores que estao na posicao x e y do vetor tour
	// Copia o tour atual para a solução vizinha.
	int neighborTour[size];
	memcpy(neighborTour, tour, size * sizeof(int));
	// Troca as cidades nas posições i e j.
	int temp = neighborTour[i];
	neighborTour[i] = neighborTour[j];
	neighborTour[j] = temp;
}



int calculateTourDistance(int *tour){ //calculo da qualidade da solucao (fitness)
	int dist=0;
	for(int i=0; i<size-1; i++){
		dist+=distanceMatrix[tour[i]][tour[i+1]];
	}
	dist+=distanceMatrix[tour[size-1]][tour[0]];
	return dist;
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
