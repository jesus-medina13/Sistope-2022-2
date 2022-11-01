/******** LIBRERIAS/DEPENDENCIAS ********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

/******** DECLARACION DE STRUCTS / DECLARACIÓN DE FUNCIONES / VARIABLES GLOBALES ********/

typedef struct resultadoYear{
	int y;
	int canJuegos;
	int idCaro;
	float pCaro;
	int idBarato;
	float pBarato;
	float pPromedio;
	float w;
	float m;
	float l;
	struct resultadoYear * sig;
}resultadoYear;

resultadoYear * crearResultado(int y, int canJuegos, int idCaro, float pCaro, int idBarato, float pBarato, float pPromedio,	float w, float m, float l){
	resultadoYear * newRes = malloc(sizeof(resultadoYear));
	newRes->y = y;
	newRes->canJuegos = canJuegos;
	newRes->idCaro = idCaro;
	newRes->pCaro = pCaro;
	newRes->idBarato = idBarato;
	newRes->pBarato = pBarato;
	newRes->pPromedio = pPromedio;
	newRes->w = w;
	newRes->m = m;
	newRes->l = l;
	newRes->sig = NULL;
}

resultadoYear * insertarResultado(resultadoYear * lista, resultadoYear * res){
	
	resultadoYear * aux = lista;

	if (lista == NULL)
	{
		lista = res;
		return lista;
	}

	while(aux->sig != NULL){
		aux = aux->sig;
	}
	aux->sig = res;
	return lista;	
}

int revisarYear(resultadoYear * lista, resultadoYear * year){
	
	resultadoYear * aux = lista;
	if (lista == NULL)
	{
		return 0;
	}
	while(aux != NULL){
		if (aux->y == year->Y)
		{
			aux->canJuegos = aux->canJuegos + 1;
			if(aux->pCaro < year->pCaro){
				aux->pCaro = year->pCaro;
				aux->idCaro = year->idCaro;
			}else if(aux->pBarato > year->pBarato){
				aux->pBarato = year->pBarato;
				aux->idBarato = year->idBarato;
			}
			aux->pPromedio = aux->pPromedio + year->pPromedio;
			aux->w = aux->w + year-> w;
			return 1;
		}
		aux = aux->sig;
	}
	return 0;
}

int main(int argc, char *argv[]){
	//Se reciben los valores del pipe y se pasan nuevamente a números
	int pipeLectura = atoi(argv[0]);
	int pipeEscritura = atoi(argv[1]);
	int bflag = atoi(argv[2]); //Esta flag puede ser 0 o 1

	char linea[150]; //Buffer para recibir el string del pipe
	char delimitador[] = ","; // separador de datos en la linea

	int id;
	char nombre[50];
	int edad;
	float precio;
	char coomingSoon[5];
	int year;
	char free[5];
	char w[3];
	char m[3];
	char l[3];
	float w2 = 0;
	float m2 = 0;
	float l2 = 0;

	int nLineas // numero de lineas que se leen

	resultadoYear * lista;

	int flagLectura = 1; //While true, mientras no se reciba el mensaje de fin del padre
	while(flagLectura){
		if(read(pipeLectura, linea, sizeof(char)*150) == -1){
			printf("Error al leer del pipe la linea del padre\n");
		}
		if(strcmp(linea,"FIN") == 0){ //Mensaje de finalización
			flagLectura = 0;
			continue;
		}
		nLineas += 1;
		
    
		//Se parsean los datos de la linea a las variables
		char * token = strtok(linea, delimitador);
		int count = 0;
		if (token != NULL)
		{
			while(token != NULL){
				if (count == 0)
				{
					id = atoi(token);
					printf("%d, ", id);
				}
				if (count == 1)
				{
					strcpy(nombre, token);
					printf("%s, ", nombre);
				}
				if (count == 2)
				{
					edad = atoi(token);
					printf("%d, ", edad);
				}
				if (count == 3)
				{
					precio = atof(token);
					printf("%f, ", precio);
				}
				if (count == 4)
				{
					strcpy(comingSoon, token);
					printf("%s, ", comingSoon);
				}
				if (count == 5)
				{
					year = atoi(token);
					printf("%d, ", year);
				}
				if (count == 6)
				{
					strcpy(free, token);
					printf("%s, ", free);
				}
				if (count == 7)
				{
					strcpy(w, token);
					printf("%s, ", w);
				}
				if (count == 8)
				{
					strcpy(m, token);
					printf("%s, ", m);
				}
				if (count == 9)
				{
					strcpy(l, token);
					printf("%s, ", l);
				}
				
				token = strtok(NULL, delimitador);
				count++;
			}
		}
		if(strcmp()){
			w2 = 1;
		}else{
			w2 = 0;
		}
		if(strcmp()){
			m2 = 1;
		}else{
			m2 = 0;
		}
		if(strcmp()){
			l2 = 1;
		}else{
			l2 = 0;
		}
		
		resultadoYear * lActual = crearResultado(year, 1, id, precio, id, precio, precio, w2, m2, l2);
		if (revisarYear(lista, lActual) == 0) // se revisa que no este el año
		{
			crearResultado();
		}
		
	}
}