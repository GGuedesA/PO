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
	readFile("/home/felix/Documents/GitHub/PO/tsp/eil51.tsp"); // passa direto o nome do arquivo, ao inves de usar linha de comando

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

	printf("Tamanho antes da busca gulosa: %d\n", calculateTourDistance(tour));

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

void tweak(int *tour, int i, int j)
{
	// Implementar
	// Gerar dois numeros aleatorios entre 0 e o numero de cidades, garantir que esses dois numeros sejam diferentes, eles serao indices digamos x e y
	// Trocar os valores que estao na posicao x e y do vetor tour
	int temp = tour[i];
	tour[i] = tour[j];
	tour[j] = temp;
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

void teste(int n, int *caminho){
    printf("\nSolucao gerada -> ");
    for(int i=0; i<n; i++){
        printf("%2d ", caminho[i]); //impressao do caminho
    }
    printf("\n");
}

void simulatedAnnealingg(int *tour)
{
	// Temperatura inicial.
	int temperature = 10;

	// Taxa de resfriamento.
	double coolingRate = 0.9;

	// Melhor solução encontrada até o momento.
	int bestDistance = INT_MAX;

	// Loop principal do algoritmo.
	while (temperature > 0)
	{
		// Gera uma vizinhança.
		tweak(tour, rand() % size, rand() % size);

		// Calcula a diferença de energia.
		int deltaEnergy = calculateTourDistance(tour) - calculateTourDistance(tour);

		// Aceita a mudança com probabilidade.
		if (deltaEnergy < 0)
		{
			// A mudança é melhor, então sempre aceitamos.
		}
		else
		{
			// A mudança é pior, então aceitamos com probabilidade de e^(-deltaEnergy/temperature).
			double probability = exp(-deltaEnergy / temperature);
			if (rand() % 1000 < probability * 1000)
			{
				// Aceitamos a mudança.
			}
			else
			{
				// Rejeitamos a mudança e restauramos o estado anterior.
				memcpy(tour, tour, size * sizeof(int));
			}
		}

		// Diminuir a temperatura.
		temperature *= coolingRate;

		// Atualizar a melhor solução encontrada
		int distance = calculateTourDistance(tour);
		if (distance < bestDistance)
		{
			bestDistance = distance;
			memcpy(tour, tour, size * sizeof(int));
		}
	}

	// Imprimir a melhor solução encontrada
	printf("Melhor solucao encontrada pelo algoritmo simulated annealing: %d\n", bestDistance);
}
int path_length(int *caminho){
    int length = 0;
    for(int i=0 ; i<size ; i++){
        length += distanceMatrix[caminho[i]][caminho[i+1]];
    }

    return length;
}

int tamanho_do_caminho(int *caminho){
    int tamanho = 0;
    for(int i=0 ; i<size ; i++){
        tamanho += distanceMatrix[caminho[i]][caminho[i+1]]; //somando valores de distancia de cada cidade contando com o retorno para a cidade inicial
    }

    return tamanho;
}
void copia_caminho(int *caminho, int *copia){ //copiando o caminho para outro espaco alocado, auxilio para outros metodos
    for(int i=0 ; i<size+1 ; i++){
        copia[i] = caminho[i];
    }
}

void simulatedAnnealing(int *tour) {
	int *copy = (int*)malloc((size+1)* sizeof(int));
	int *auxiliary = (int*)malloc((size+1)* sizeof(int));

    float e = 2.7182; 
    float t = 111;
	copia_caminho(tour, copy);
	copia_caminho(tour, auxiliary);

    for (int i = 0; i <= 100000000; i++) { 
        t = ((float)i * 100.00 / 100000000.00); 
        int city_one = rand() % size + 1; 
        int city_two = rand() % size + 1;

        if (city_one == city_two || ((city_one == 0 && city_two == size) || (city_one == size && city_two == 0))) {
            // If any of these situations occur, it means that two equal positions were drawn.
            while (city_one == city_two || ((city_one == 0 && city_two == size) || (city_one == size && city_two == 0))) {
                city_one = rand() % size + 1; // the draw is repeated until both are different, as specified in the while loop structure
                city_two = rand() % size + 1;
            }
        }

        if (city_one == 0) {
            copy[size] = copy[city_two]; 
        }
        if (city_two == size) { 
            copy[0] = copy[city_one]; 
        }
        if (city_one == size) {
            copy[0] = copy[city_two];
        }
        if (city_two == 0) {
            copy[size] = copy[city_one];
        }

        int aux = copy[city_one];
        copy[city_one] = copy[city_two];
        copy[city_two] = aux; 

        float valueR = (float)path_length(copy);
        float valueS = (float)path_length(auxiliary);;
        float exponent = (valueS - valueR) / t;
        float equation = pow(e, exponent);
        float random_value = rand() % 2;

        if ((valueR < valueS) || (random_value < equation)) {
            memcpy(auxiliary, copy,  size * sizeof(int));
        }
        else {
            memcpy(copy, auxiliary,   size * sizeof(int)); 
        }
        if (path_length(auxiliary) < path_length(tour)) {
            memcpy(tour, auxiliary,  size * sizeof(int));
        }
    }
	printf("\n\nTAMANHO TOTAL = %d\n", path_length(tour));
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
