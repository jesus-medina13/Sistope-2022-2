/******** LIBRERIAS/DEPENDENCIAS ********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <conio.h>  
#include <sys/wait.h>

/******** DECLARACION DE STRUCTS / DECLARACIÓN DE FUNCIONES / VARIABLES GLOBALES ********/

//Struct que sirve para almacenar los file descriptors de cada proceso hijo
typedef struct FileDescriptor{
	int fd[2];
}Fd;

//Entrada: Vacio
//Salida: un entero que representa un booleano
//Descripción: Validacion de entradas 
int verificarEntradas(void);

//Entrada: Vacio
//Salida: Vacio
//Descripción: Función para esperar que los procesos hijos finalicen
void esperarHijos(void);

//Entrada: Vacio
//Salida: Vacio
//Descripción: Función encargada de recibir del pipe de cada proceso los resultados de cada hijo y escribirlos en un archivo de salida
void crearSalida(void);

//Entrada: Vacio
//Salida: Vacio
//Descripción: Función que se encarga de cerrar todos los pipes entre padre e hijos
void cerrarPipes(void);

//Variables globales

char* nombreEntrada; //Nombre ingresado por consola en la flag -i
char* nombreSalida; //Nombre ingresado por consola en la flag -o
Fd *arrayFds; //Puntero al arreglo en donde estarán los file descriptors
float price; // Dato ingresado por consola en la flag -p
int canWorkers; //Dato ingresado por consola en la flag -n
int year; //Dato ingresado por consola en la flag -d


/******** BLOQUE PRINCIPAL ********/
int main(int argc, char *argv[]){
	
	int c; //Variable para almacenar el valor de entrada
	int bflag; // Bandera que indica la opción "verbose" de los procesos hijos
	int indice; //Indice para contar la cantidad de argumentos de entrada al programa
	
	while ((c = getopt(argc, argv, "i:o:d:p:n:b")) != -1){
		switch (c){
			case 'i':
				nombreEntrada = optarg;
				break;
			case 'o':
				nombreSalida = optarg;
				break;
			case 'd':
				year = atoi(optarg);
				break;
			case 'd':
				price = atof(optarg);
				break;
			case 'n':
				canWorkers = atoi(optarg);
				break;
			case 'b':
				bflag = 1;
				break;
			default:
				printf("Error\n");
				return -1;
		}
		
	}

	//Se comprueban las entradas
	if(!verificarEntradas()){
		printf("Error en los parametros de entrada\n");
		exit(-1);
	}

	//Mensaje en caso de que exista un error en los parametros de entrada según la cantidad de entradas
	for (indice = optind; indice < argc; indice++){
		printf ("Argumento no valido en opcion %s\n", argv[indice]);
	}

    //Ahora se comienzan a crear procesos hijos de acuerdo a la cantidad de workers que se necesiten procesar
	//Primero se crean tantos files descriptors según la cantidad de hijos que deben existir
	arrayFds = (Fd*)malloc(sizeof(Fd)*canWorkers);
	for (int i = 0; i < canWorkers; ++i){
		if(pipe(arrayFds[i].fd) == -1){
			printf("No se pudo crear el pipe\n");
			exit(-4);
		}
	}

	//Luego se crean los hijos según la cantidad de workers solicitados
	for (int k = 0; k < discos; ++k){
		if(fork() == 0){
			char pLectura[12];
			char pEscritura[12];
			char flag[12];
			//Se pasan de entero a string para poder pasarlos al execve
			sprintf(pLectura, "%d", arrayFds[k].fd[0]); 
			sprintf(pEscritura, "%d", arrayFds[k].fd[1]);
			sprintf(flag, "%d", bflag);
			//Se guardan en un arreglo de strings
			char *argumentos[] = {pLectura, pEscritura, flag, NULL};
			char *mienvp[] = {NULL};

			//Y se llama al segundo programa
			if(execve("./worker", argumentos, mienvp) == -1){
				printf("No se pudo realizar execve\n");
				exit(-1);
			}
		}
	}

	//El padre es el que se encarga de leer y enviar los datos a los procesos hijos
	FILE *file = fopen(nombreEntrada, "r");
	char linea[150]; //Buffer auxiliar
	
	//Se lee el archivo de entrada
	while(fgets(linea, 150, file)){
		linea[strcspn(linea, "\n")] = '\0'; //Se elimina el '\n' que entrega fgets

		//Envia al fd correspondiente la linea con la visibilidad a través del pipe correspondiente
		if(write(arrayFds[rand() % canWorkers].fd[1], linea, sizeof(char)*150) == -1){				////////////////////////////////////////////////////////////////////ARREGLAR
			printf("No se pudo enviar la linea al pipe\n");
		}
	}
	
	//Una vez leído, se envía la señal por el pipe para que los hijos dejen de leer
	for (int i = 0; i < canWorkers; ++i){
		if(write(arrayFds[i].fd[1], "FIN", sizeof(char)*150) == -1){
			printf("NO PUDE MANDAR EL FIN AAAA\n");
		}
	}

	//Se cierra el archivo de entrada
	fclose(file);

	//Se espera a que finalicen los hijos
	esperarHijos();

	//Se crea el archivo de salida
	crearSalida();

	//Se cierran los pipes
	cerrarPipes();

	//Finaliza el programa
	return 0;
}

/******** DEFINICION DE FUNCIONES ********/

//Función que sirve que para comprobar que las entradas del programa sean válidas
int verificarEntradas(void){
	if(strcmp(nombreEntrada, nombreSalida) == 0){
		printf("El nombre del archivo de entrada no puede ser igual al nombre del archivo de salida\n");
		return 0; //No válido
	}else if((canWorkers <= 0)){
		printf("Ingrese valores positivos\n");
		return 0; //No válido
	}
	return 1; //Válido
}

//Función para esperar que los procesos hijos finalicen
void esperarHijos(void){
	pid_t status;
	while(wait(&status) != -1); //wait espera a que cualquier hijo finalice, por eso el while
	return;
}

//Función encargada de recibir del pipe de cada proceso los resultados de cada hijo y escribirlos en un archivo de salida
/*void crearSalida(void){
	FILE *salida = fopen(nombreSalida, "w"); //Se crea el archivo de salida en modo escritura
	if(salida == NULL){
		printf("Error al crear el archivo de salida\n");
		return;
	}
	double buffer[5]; //buffer auxiliar para recibir los datos enviados por los hijos
	for (int i = 0; i < discos; ++i){
		//Se leen todos los file descriptors de los hijos
		if(read(arrayFds[i].fd[0], buffer, sizeof(double)*5) == -1){
			printf("No se pudo recibir del hijo\n");
			return;
		}

		//Y se escribe en el archivo de salida
		fprintf(salida, "Disco: %d\n", i+1);
		fprintf(salida, "Media real: %f\n", buffer[1]);
		fprintf(salida, "Media imaginaria: %f\n", buffer[2]);
		fprintf(salida, "Potencia: %f\n", buffer[3]);
		fprintf(salida, "Ruido total: %f\n", buffer[4]);
	}

	printf("Archivo de salida creado con exito\n");
	fclose(salida); //Cerrar archivo
	return;
}*/

//Función que se encarga de cerrar todos los pipes entre padre e hijos
void cerrarPipes(void){
	for (int i = 0; i < canWorkers; ++i){
		close(arrayFds[i].fd[0]); //Cerrar lectura
		close(arrayFds[i].fd[1]); //Cerrar escritura
	}
	return;
}