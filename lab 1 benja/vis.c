/*
    Sistemas operativos Laboratorio 1

    Integrantes:
        Matías Ruíz
        Benjamín Sagredo
*/


/******** LIBRERIAS/DEPENDENCIAS ********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

/******** DECLARACION DE STRUCTS / DECLARACIÓN DE FUNCIONES / VARIABLES GLOBALES ********/


/******** BLOQUE PRINCIPAL ********/

int main(int argc, char *argv[]){
	//Se reciben los valores del pipe y se pasan nuevamente a números
	int pipeLectura = atoi(argv[0]);
	int pipeEscritura = atoi(argv[1]);
	int bflag = atoi(argv[2]); //Esta flag puede ser 0 o 1

	char linea[150]; //Buffer para recibir el string del pipe
	double u, v, real, im, ruido; //Variables para guardar los datos del string

	//Variables para ir guardando el resultado de las sumatorias
	double nVisibilidades = 0, mediaReal = 0, mediaIm = 0, ruidoTotal = 0, potencia = 0;

	//Arreglo para enviar al padre a través del pipe
	double resultados[5];
	
	int flagLectura = 1; //While true, mientras no se reciba el mensaje de fin del padre
	while(flagLectura){
		if(read(pipeLectura, linea, sizeof(char)*150) == -1){
			printf("Error al leer del pipe la linea del padre\n");
		}
		if(strcmp(linea,"FIN") == 0){ //Mensaje de finalización
			flagLectura = 0;
			continue;
		}
		
		nVisibilidades += 1; //Cantidad de visibilidades leídas hasta el momento

		//Se parsean los datos de la linea a las variables
		if(sscanf(linea, "%lf,%lf,%lf,%lf,%lf", &u, &v, &real, &im, &ruido) != 5){
			printf("Error al leer la linea\n");
		}

		//Se va haciendo las sumatorias
		mediaReal += real;
		mediaIm += im;
		potencia += sqrt(pow(real, 2) + pow(im, 2)); //Funciones de math.h
		ruidoTotal += ruido;
	}

	//Al final estos dos datos deben ser divididos por la cantidad de visibilidades recibidas
	mediaReal /= nVisibilidades;
	mediaIm /= nVisibilidades;

	//Se cierra el pipe de lectura
	close(pipeLectura);

	//Si es que la flag -b está presente, mostrar información extra
	if(bflag){
		printf("Soy el hijo de pid %d, procese %.0f visibilidades\n", getpid(), nVisibilidades);
	}
	
	//Y guardar en el arreglo los resultados obtenidos
	resultados[0] = nVisibilidades; //Por si acaso
	resultados[1] = mediaReal;
	resultados[2] = mediaIm;
	resultados[3] = potencia;
	resultados[4] = ruidoTotal;

	//Se envía por el pipe al padre el arreglo con los resultados
	if(write(pipeEscritura, resultados, sizeof(double)*5) == -1){
		printf("No se pudo enviar el resultado del hijo al padre\n");
	}

	//Se cierra el pipe de escritura
	close(pipeEscritura);
	
	//Y finaliza el programa
	return 0;
}

/******** DEFINICION DE FUNCIONES ********/