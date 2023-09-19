#include <stdio.h>	 // print, fgets function
#include <stdlib.h>	 // exit, atoi function
#include <string.h>	 //strlen function
#include <math.h>	 // sqrt function
#include <algorithm> // std::min
#include <limits.h>	 // INT_MAX
#include <time.h>	 //time(NULL) usado apra inicializar a semente aleatoria

int calculateTourDistance(int *tour); // calculo da qualidade da solucao (fitness)
void tweak(int *tour, int i, int j);  // modifica uma dada solucao (encontra uma solucao vizinha)
void readFile(const char *inputFile); // le o arquivo que foi passado como parametro e armazena os valores em distanceMatrix
void greedySearch(int *tour);
void simulatedAnnealing(int *tour);

int **distanceMatrix, size = -1, pos = 0;

int main(const int argc, const char **inputFile)
{
	// if(argc != 2){ //verifica se foi passado o nome do arquivo (o primeiro argumento em C e o nome do executavel)
	// 	fprintf(stderr,"use: tspScratch <tsp file>\n\n");
	// 	exit(1);
	// }else{
	//     readFile(inputFile[1]);
	// }
	readFile("tsp/eil51.tsp"); // passa direto o nome do arquivo, ao inves de usar linha de comando

	//    Mostra a distanceMatrix
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
			printf("%d ", distanceMatrix[i][j]);
		printf("\n");
	}

	//    Cria um tour percorrendo as cidades em ordem e mostra o tamanho desse tour
	int tour[size];
	for (int i = 0; i < size; i++)
		tour[i] = i;
	printf("Tamanho %d\n", calculateTourDistance(tour));

    
	// Busca gulosa
	// Fazer uma busca gulosa partindo da cidade 0, depois da 1, depois da 2 ...
	// Salvar melhor resultado encontrado nessa etapa
	greedySearch(tour);
	printf("Tamanho depois da busca gulosa: %d\n", calculateTourDistance(tour));

	// 	Simulated Annealing
	// Usar a melhor solução encontrada no estagio anterior como solucao inicial do Simulated Annealing
	// Implementar o Simulated annealing, com atencao ao detalhe de que aqui estamos tentando minimizar a distancia total, no algoritmo mostrado, estamos tentando maximizar a qualidade

	simulatedAnnealing(tour);
	printf("Melhor caminho encontrado: %d\n", calculateTourDistance(tour));
}

int calculateTourDistance(int *tour)
{ // calculo da qualidade da solucao (fitness)
	int dist = 0;
	for (int i = 0; i < size - 1; i++)
	{
		dist += distanceMatrix[tour[i]][tour[i + 1]];
	}
	dist += distanceMatrix[tour[size - 1]][tour[0]];
	return dist;
}

void greedySearch(int *tour)
{   // Melhor solução encontrada até o momento.
    int smallest_sum = INT_MAX;

    int *inserted = (int*) malloc(size * sizeof(int));
    int initial_city = 0; // cidade inicial para cada partida
    int num = 0;

	
    for (int i = 0; i <= size; i++) {

        if (i == size)
            num = initial_city;
        else //iniciando os caminhos
            num = i;

        for (int i = 0; i < size; i++) { 
            inserted[i] = false;
        }

        tour[0] = num; 
        inserted[num] = true; //primeiro no recebido

        int sum = 0;

        for (int i = 0; i < size; i++) {
            int cost = INT_MAX;
            int selected_neighbor; // cidade mais proxima celecionada

            if (i == size - 1) {
                inserted[num] = false;
            }

            for (int j = 0; j < size; j++) {
                if (((!inserted[j]) && (cost > distanceMatrix[tour[i]][j]) && (i != j)) || (((i == 0 && j == 0) && (!inserted[j])) || ((i == size - 1 && j == size - 1) && (!inserted[j])))) {
					selected_neighbor = j;										
                    cost = distanceMatrix[tour[i]][j];										
                    
                }
            }

            if (cost != INT_MAX) {
                sum += cost;
            }

            tour[i + 1] = selected_neighbor; // add no caminho o caminho mais rapido
            inserted[selected_neighbor] = true; 
        }

        if (i == size) {
            printf("melhor solucao encontrada na busca gulosa: cidade %d\n", initial_city);
        }

        if (sum < smallest_sum) {  //verficacao para encontrar o menor custo
            smallest_sum = sum;
            initial_city = num; 
			
        }
    }
    free(inserted);
}

void teste(int n, int *tour){
    printf("\nSolucao gerada -> ");
    for(int i=0; i<n; i++){
        printf("%2d ", tour[i]); //impressao do caminho
    }
    printf("\n");
}


int path_length(int *tour){
    int length = 0;
    for(int i=0 ; i<size ; i++){
        length += distanceMatrix[tour[i]][tour[i+1]];
    }

    return length;
}

int tamanho_do_caminho(int *tour){
    int tamanho = 0;
    for(int i=0 ; i<size ; i++){
        tamanho += distanceMatrix[tour[i]][tour[i+1]]; //somando valores de distancia de cada cidade contando com o retorno para a cidade inicial
    }

    return tamanho;
}
void copia_caminho(int *tour, int *copia){ //copiando o caminho para outro espaco alocado, auxilio para outros metodos
    for(int i=0 ; i<size+1 ; i++){
        copia[i] = tour[i];
    }
}

void simulatedAnnealing(int *tour) {
    int *current_tour = (int *)malloc((size + 1) * sizeof(int));
    int *best_tour = (int *)malloc((size + 1) * sizeof(int));

    memcpy(current_tour, tour, (size + 1) * sizeof(int));
    memcpy(best_tour, tour, (size + 1) * sizeof(int));

    float initial_temperature = 10000.0; 
    float temperature = initial_temperature;
    float cooling_rate = 0.99999; 

    while (temperature > 1.0) {
        int city1 = rand() % size + 1;
        int city2 = rand() % size + 1;

        if (city1 == city2)
            continue;  

        int temp = current_tour[city1];
        current_tour[city1] = current_tour[city2];
        current_tour[city2] = temp;

        int current_distance = calculateTourDistance(current_tour);
        int best_distance = calculateTourDistance(best_tour);

        int delta_distance = current_distance - best_distance;

        if (delta_distance < 0 || (rand() / (float)RAND_MAX) < exp(-delta_distance / temperature)) {
            memcpy(best_tour, current_tour, (size + 1) * sizeof(int));
        } else {
            temp = current_tour[city1];
            current_tour[city1] = current_tour[city2];
            current_tour[city2] = temp;
        }

        temperature = temperature * cooling_rate;

        if (temperature < 1000.0)
            cooling_rate = 0.999995;
    }

   
    memcpy(tour, best_tour, (size + 1) * sizeof(int));

}

void readFile(const char *inputFile)
{ // le o arquivo que foi passado como parametro e armazena os valores em distanceMatrix

	double *x, *y;
	char type[10] = "";

	char s[500];
	FILE *stream = fopen(inputFile, "r"); // read file (read-only permission)
	if (stream == NULL)
	{ // if the file was not read, error
		fprintf(stderr, "\nFail to Open tsp File!!\n");
		exit(1);
	}
	while (fgets(s, 500, stream))
	{						   // read every line (not larger than 500 chars) of the input
		if (strlen(s) - 1 > 0) // remove the last break line of the line
			s[strlen(s) - 1] = '\0';
		if (('\r' == s[strlen(s) - 1])) // in some files there is a carriage return at the end, don't know why. This command removes it
			s[strlen(s) - 1] = 0;

		char *value1 = strtok(s, " "); // creating sub-strings separated by space
		char *value2 = strtok(NULL, " ");
		char *value3 = strtok(NULL, " ");

		if (!strcmp(value1, "EDGE_WEIGHT_TYPE:"))
		{ // verify if the instance is of type EUC_2D CEIL_2D or, ATT, only the calculation for these types were implemented
			if (strcmp(value2, "EUC_2D") && strcmp(value2, "ATT") && strcmp(value2, "CEIL_2D"))
			{
				fprintf(stderr, "\nERROR! tsp file is of type \"%s\" only EUC_2D, ATT or CEIL_2D are supported. Aborting!!\n", value2);
				exit(1);
			}
		}

		if (!strcmp(value1, "EDGE_WEIGHT_TYPE"))
		{ // verify if the instance is of type EUC_2D CEIL_2D or, ATT, only the calculation for these types were implemented
			if (strcmp(value3, "EUC_2D") && strcmp(value3, "ATT") && strcmp(value3, "CEIL_2D"))
			{
				fprintf(stderr, "\nERROR! tsp file is not of type EUC_2D, ATT or CEIL_2D aborting!!\n");
				exit(1);
			}
			else
				strcpy(type, value3);
		}

		if (!strcmp(value1, "TYPE") && (strcmp(value3, "TSP")))
		{ // verify if the instance is of type TSP, the other types will not be considered
			fprintf(stderr, "\nERROR! tsp file is not of type TSP, aborting!!\n");
			exit(1);
		}

		if (!strcmp(value1, "DIMENSION"))
		{ // read the dimension from the header and allocate memory for the cities
			size = atoi(value3);
			distanceMatrix = (int **)malloc(size * sizeof(int *));
			x = (double *)malloc(size * sizeof(double *));
			y = (double *)malloc(size * sizeof(double *));

			for (int i = 0; i < size; i++)
				distanceMatrix[i] = (int *)malloc(size * sizeof(int));
		}

		if (atoi(value1))
		{ // if the first substring is a number, the list of cities started
			if (size == -1)
			{ // if the size was not set, it was not in the header, error
				fprintf(stderr, "\nERROR! Dimension not set in file header!!\n");
				exit(1);
			}
			x[pos] = atof(value2); // storing the values from the file in the arrays
			y[pos] = atof(value3);
			pos++;
		}
		// 		free(x);
		//         free(y);
	}

	if (!strcmp(type, "EUC_2D"))
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				double xd = x[i] - x[j];
				double yd = y[i] - y[j];
				double dist = sqrt(xd * xd + yd * yd);
				distanceMatrix[i][j] = (int)(dist + 0.5); // calculating the euclidean distance, rounding to int and storing in the distance matrix
			}
		}
	}
	else if (!strcmp(type, "CEIL_2D"))
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				double xd = x[i] - x[j];
				double yd = y[i] - y[j];
				double dist = sqrt(xd * xd + yd * yd);
				distanceMatrix[i][j] = (int)(dist + 0.000000001); // calculating the euclidean distance, rounding to int and storing in the distance matrix
			}
		}
	}
	else if (!strcmp(type, "ATT"))
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				double xd = x[i] - x[j];
				double yd = y[i] - y[j];
				double rij = sqrt((xd * xd + yd * yd) / 10.0);
				double tij = (int)(rij + 0.5);
				if (tij < rij)
					distanceMatrix[i][j] = tij + 1;
				else
					distanceMatrix[i][j] = tij;
			}
		}
	}
}
