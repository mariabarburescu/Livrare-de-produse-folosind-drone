#include <stdio.h>
#include <stdlib.h>

#define INF 9999

//Structura pentru retinerea comenzilor
typedef struct Command
{
	int type; //Retine tipul fermei
	int node; //Retine clientul
}command;

//Structura pentru a retine informatii legate de fiecare nod
typedef struct Node
{
	int viz; //Retine daca nodul a fost vizitat
	int length; //Retine lungimea drumului pana la nod
	int prev; //Retine nodul anterior celui actual
	int farm_type; //Retine daca nodul este ferma sau potential client
}node;

//Functie pentru eliberarea unei matrici
void eliberare(int **a, int size)
{
	int i;
	for (i = 0; i < size; i++)
		free(a[i]);
	free(a);
}

//Functie care returneaza nodul minim de la sursa
int minim(node *info, int N)
{
	int i, min = INF, min_index;
	for (i = 0; i < N; i++)
		/*Daca lungimea drumului pana la nodul i este minima si
		acesta nu a fost vizitat, retinem lungimea si nodul*/
		if (info[i].length < min && info[i].viz == 0)
		{
			min = info[i].length;
			min_index = i;
		}
	//Returnam nodul cu lungimea cea mai mica si care nu a fost vizitat
	return min_index;
}

//Functie care reseteaza campurile nodurilor
void reset(node *info, int N)
{
	int i;
	for (i = 0; i < N; i++)
	{
		info[i].viz = 0;
		info[i].length = INF;
	}
}

//Functie care calculeaza drumul de la nodul current pana la toate celelalte noduri
void path(int current, int **mat, node *info, int N)
{
	int i, u, j;
	//Resetam campurile nodurilor
	reset(info, N);
	//Lungimea drumului nodului current este 0, deoarece este inceputul drumului
	info[current].length = 0;
	for (i = 0; i < N; i++)
	{
		//Alegem nodul cu lungimea minima de la current
		u = minim(info, N);
		//Il marcam ca vizitat
		info[u].viz = 1;
		for (j = 0; j < N; j++)
			//Verificam daca nodurile sunt adiacente
			if (mat[u][j] != 0)
				if (info[j].viz == 0 && info[u].length != INF)
					if (info[u].length + mat[u][j] < info[j].length)
					{
						info[j].length = info[u].length + mat[u][j];
						info[j].prev = u;
					}
	}
}

//Functie pentru afisarea drumului
void show (int first, int current, node *info, FILE *out)
{
	if (first == current)
		return;
	show(first, info[current].prev, info, out);
	fprintf(out, " %d", current);
}

//Functie pentru primul task
void task1(command *commands, int **mat, int C, int farm, node *info, int N, FILE *out)
{
	int i, cost = 0;
	fprintf(out, "%d", farm);
	for (i = 0; i < C; i++)
	{
		//Calculam drumul de la ferma la comanda
		path(farm, mat, info, N);
		cost += info[commands[i].node].length;
		show(farm, commands[i].node, info, out);
		//Calculam drumul de la comanda inapoi la ferma
		path(commands[i].node, mat, info, N);
		cost += info[farm].length;
		show(commands[i].node, farm, info, out);
	}
	fprintf(out, "\n%d\n", cost);
}

//Functie pentru alegerea celei mai bune ferme
int best_farm(node *info, int N, int current, int target, int **mat, int type)
{
	int i, min = INF, min_index, cost = 0;
	for (i = 0; i < N; i++)
		if (info[i].farm_type == type)
		{
			//Calculam drumul de la nodul current
			path(current, mat, info, N);
			cost = info[i].length;
			//Calculam drumul de la ferma i
			path(i, mat, info, N);
			cost += info[target].length;
			if (cost < min)
			{
				min = cost;
				min_index = i;
			}
		}
	return min_index;
}

//Functie pentru alegerea celei mai apropiate functii
int closest_farm(node *info, int N)
{
	int i, min = INF, min_index;
	for (i = 0; i < N; i++)
		//Verificam daca i este ferma si daca drumul pana la ea 
		//este minim
		if (info[i].farm_type != 0 && info[i].length < min)
		{
			min = info[i].length;
			min_index = i;
		}
	return min_index;
}

//Functie pentru al doilea task
void task2(command *commands, int **mat, int C, int farm, node *info, int N, FILE *out, int *cost)
{
	int i;
	for (i = 0; i < C; i++)
	{
		//Calculam drumul de la ferma
		path(farm, mat, info, N);
		(*cost) += info[commands[i].node].length;
		show(farm, commands[i].node, info, out);
		//Calculam drumul de la comanda
		path(commands[i].node, mat, info, N);
		//Alegem cea mai buna ferma la care sa mergem
		if (i == C - 1)
			farm = closest_farm(info, N);
		else
			farm = best_farm(info, N, commands[i].node, commands[i+1].node, mat, commands[i+1].type);
		//Calculam drumul de la comanda
		path(commands[i].node, mat, info, N);
		(*cost) += info[farm].length;
		show(commands[i].node, farm, info, out);
	}
	fprintf(out, "\n%d\n", *cost);
}

//Functie pentru al treilea task
int task3(command *commands, int **mat, int C, int farm, node *info, int N)
{
	int i, cost = 0;
	for (i = 0; i < C; i++)
	{
		//Calculam drumul de la ferma
		path(farm, mat, info, N);
		cost += info[commands[i].node].length;
		//Calculam drumul de la comanda
		path(commands[i].node, mat, info, N);
		if (i == C - 1)
			farm = closest_farm(info, N);
		else
			farm = best_farm(info, N, commands[i].node, commands[i+1].node, mat, commands[i+1].type);
		//Calculam drumul de la comanda
		path(commands[i].node, mat, info, N);
		cost += info[farm].length;
	}
	return cost;
}

//Functie pentru interschimbarea a doua valori
void swap(int *a, int *b)
{
	int aux = (*a);
	(*a) = (*b);
	(*b) = aux;
}

//Functie pentru generarea tuturor permutarilor comenzilor si pentru a alege
//cea mai optima varianta
void Permutation(command *commands, int size, int C, int *cost_min, command *best, int **mat, int farm, node *info, int N, int *start_farm)
{
    int i, j, cost = 0;
    if (size == 1)
    {
    	//Daca ferma de inceput nu corespune cu ferma necesara 
    	//pentru efectuarea primei comenzi cautam ferma buna
    	if (info[farm].farm_type != commands[0].type)
			for (i = 0; i < N; i++)
			{
				//Daca ferma corespunde realizarii primei comenzi
				if (info[i].farm_type == commands[0].type)
				{
					//Calculam drumul de la ferma la celelalte noduri
					path(farm, mat, info, N);
					//Adunam costul pana la noua ferma gasita
					cost = info[i].length;
					//Adunam costul de la ferma gasita pana la prima comanda
    				cost += task3(commands, mat, C, i, info, N);
    				//Daca costul este minim salvam costul, ferma de start 
    				//si ordinea de realizare a comenzilor
    				if (cost < (*cost_min))
    				{
    					(*cost_min) = cost;
    					(*start_farm) = i;
    					for (j = 0; j < C; j++)
    					{
   				 			best[j].type = commands[j].type;
    						best[j].node = commands[j].node;
   				 		}
    				}
    			}
   			}
   		//Daca ferma de start corespunde primei comenzi
   		else
   		{
   			//Adunam costul de la ferma la 
   			cost += task3(commands, mat, C, farm, info, N);
   			if (cost < (*cost_min))
   			{
   				(*cost_min) = cost;
   				for (j = 0; j < C; j++)
   				{
		 			best[j].type = commands[j].type;
   					best[j].node = commands[j].node;
		 		}
   			}
  		}
        return;
    }
 
    for (i = 0; i < size; i++)
    {
        Permutation(commands,size-1,C, cost_min, best, mat, farm, info, N, start_farm);
        if (size % 2 == 1)
        {
            swap(&commands[0].type, &commands[size-1].type);
        	swap(&commands[0].node, &commands[size-1].node);
        }
        else
        {
            swap(&commands[i].type, &commands[size-1].type);
            swap(&commands[i].node, &commands[size-1].node);
        }
    }
}
int main(int argc, char *argv[])
{
	FILE *in;
	FILE *out;
	in = fopen(argv[1], "r");
	out = fopen(argv[2], "w");
	int ID, N, M, C, i, x, y, z, **mat, cost_min = INF;
	int cost = 0;
	int farm = 0;
	int start_farm;
	command *commands, *best;
	node *info;

	fscanf(in, "%d", &ID);
	fscanf(in, "%d", &N);

	info = calloc(N, sizeof(node));
	for (i = 0; i < N; i++)
	{
		fscanf(in, "%d", &info[i].farm_type);
		if (info[i].farm_type == 1)
			farm = i;
	}
	fscanf(in, "%d", &M);
	mat = (int**)calloc(N, sizeof(int*));
	for (i = 0; i < N; i++)
		mat[i] = (int*)calloc(N, sizeof(int));
	for (i = 0; i < M; i++)
	{
		fscanf(in, "%d", &x);
		fscanf(in, "%d", &y);
		fscanf(in, "%d", &z);
		mat[x][y] = z;
	}
	fscanf(in, "%d", &C);
	commands = calloc(C, sizeof(command));
	best = calloc(C, sizeof(command));
	for (i = 0; i < C; i++)
	{
		fscanf(in, "%d%d", &commands[i].type, &commands[i].node);
	}
	//Pentru cerinta 1
	if (ID == 1)
		task1(commands, mat, C, farm, info, N, out);
	//Pentru cerinta 2
	if (ID == 2)
	{
		//Citim ferma de inceput si o afisam
		fscanf(in, "%d", &farm);
		fprintf(out, "%d", farm);
		task2(commands, mat, C, farm, info, N, out, &cost);
	}
	//Pentru cerinta 3
	if (ID == 3)
	{
		//Citim ferma de inceput
		fscanf(in, "%d", &farm);
		//Generam toate permutarile comenzilor si aflam ferma de start
		Permutation(commands, C, C, &cost_min, best, mat, farm, info, N, &start_farm);
		fprintf(out, "%d", farm);
		//Daca ferma de inceput corespunde cu prima comanda, 
		//apelam functia pentru taskul 2
		if (info[farm].farm_type == best[0].type)
		{
			task2(best, mat, C, farm, info, N, out, &cost);
		}
		//Daca ferma nu corespunde cu prima comanda
		if (info[farm].farm_type != best[0].type)
		{
			//Calculam drumul de la ferma de inceput
			path(farm, mat, info, N);
			//Afisam drumul de la ferma de inceput pana la cea de start
			show(farm, start_farm, info, out);
			//Adunam costul obtinut
			cost += info[start_farm].length;
			//Apelam functia pentru taskul 2
			task2(best, mat, C, start_farm, info, N, out, &cost);
		}
	}
	//Inchidem fisierele
	fclose(in);
	fclose(out);
	//Eliberam memoria
	free(commands);
	free(best);
	free(info);
	eliberare(mat,N);
	return 0;
}