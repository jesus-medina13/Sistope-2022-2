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
#include <sys/types.h>
#include <sys/wait.h>

/******** DECLARACION DE STRUCTS / DECLARACIÓN DE FUNCIONES / VARIABLES GLOBALES ********/

//Struct que sirve para almacenar los file descriptors de cada proceso hijo
typedef struct FileDescriptor{
	int fd[2];
}Fd;

//Entrada: Vacio
//Salida: un entero que representa un booleano
//Descripción: Función que sirve que para comprobar que las entradas del programa sean válidas
int verificarEntradas(void);

//Entrada: Un string con la línea leída por el proceso padre
//Salida: Un entero de 1 en adelante que indica el disco al que pertenece una visibilidad
//Descripción: Función que se encarga de calcular y entregar el disco al que debe ir una visibilidad
int asignarDisco(char* string);

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

char* nombreVisibilidades; //Nombre ingresado por consola en la flag -i
char* nombreSalida; //Nombre ingresado por consola en la flag -o
Fd *arrayFds; //Puntero al arreglo en donde estarán los file descriptors
int discos; //Dato ingresado por consola en la flag -n
int ancho; //Dato ingresado por consola en la flag -d


/******** BLOQUE PRINCIPAL ********/
int main(int argc, char *argv[]){
	int c; //Variable para almacenar el valor de entrada
	int bflag; // Bandera que indica la opción "verbose" de los procesos hijos
	int indice; //Indice para contar la cantidad de argumentos de entrada al programa
	while ((c = getopt(argc, argv, "i:o:d:n:b")) != -1){
		switch (c){
			case 'i':
				nombreVisibilidades = optarg;
				break;
			case 'o':
				nombreSalida = optarg;
				break;
			case 'd':
				ancho = atoi(optarg);
				break;
			case 'n':
				discos = atoi(optarg);
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

	//Ahora se comienzan a crear procesos hijos de acuerdo a la cantidad de discos que se necesiten procesar
	//Primero se crean tantos files descriptors según la cantidad de hijos que deben existir
	arrayFds = (Fd*)malloc(sizeof(Fd)*discos);
	for (int i = 0; i < discos; ++i){
		if(pipe(arrayFds[i].fd) == -1){
			printf("No se pudo crear el pipe\n");
			exit(-4);
		}
	}

	//Luego se crean los hijos según la cantidad de discos solicitados
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
			if(execve("./vis", argumentos, mienvp) == -1){
				printf("No se pudo realizar execve\n");
				exit(-1);
			}
		}
	}
	

	//El padre es el que se encarga de leer y enviar los datos a los procesos hijos
	FILE *file = fopen(nombreVisibilidades, "r");
	char linea[150]; //Buffer auxiliar

	//Se lee el archivo de entrada
	while(fgets(linea, 150, file)){
		linea[strcspn(linea, "\n")] = '\0'; //Se elimina el '\n' que entrega fgets

		//Envia al fd correspondiente la linea con la visibilidad a través del pipe correspondiente
		if(write(arrayFds[asignarDisco(linea)-1].fd[1], linea, sizeof(char)*150) == -1){
			printf("No se pudo enviar la linea al pipe\n");
		}
	}

	//Una vez leído, se envía la señal por el pipe para que los hijos dejen de leer
	for (int i = 0; i < discos; ++i){
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
	if(strcmp(nombreVisibilidades, nombreSalida) == 0){
		printf("El nombre del archivo de entrada no puede ser igual al nombre del archivo de salida\n");
		return 0; //No válido
	}else if((discos <= 0) || (ancho <= 0)){
		printf("Ingrese valores positivos\n");
		return 0; //No válido
	}
	return 1; //Válido
}


//Función que se encarga de calcular y entregar el disco al que debe ir una visibilidad
int asignarDisco(char* string){
	double datos[2]; //Arreglo en donde guardar los datos de la coordenada
	//Se leen los dos primeros datos de la linea
	if(sscanf(string, "%lf,%lf,real,im,ruido", &datos[0], &datos[1]) != 2){
		printf("Ocurrio un error\n");
	}

	//Se calcula la distancia al centro
	double distancia = sqrt(pow(datos[0], 2) + pow(datos[1],2)); //Funciones de math.h

	//Se comienza con el disco 1 porque el 0 no existe
	int disco = 1;
	int aux = ancho; //auxiliar
	int aux2 = 2; //auxiliar que sirve para ir multiplicando el ancho para hacer los intervalos

	if((0 <= distancia) && (distancia < ancho)){
		//Si está entre el primer intervalo, va al primer disco
		return disco;
	}
	//Si no, se aumenta el valor de disco
	disco += 1;

	//Y se pregunta hasta encontrar el intervalo
	while(disco < discos){	
		if((aux <= distancia) && (distancia < ancho*aux2)){
			return disco;
		}else{
			//Si no está en el intervalo, el disco aumenta, el límite izquierdo toma el valor del límite derecho
			//y el nuevo límite derecho es el ancho original multiplicado por el segundo auxiliar
			disco += 1;
			aux = ancho*aux2;
			aux2 += 1;
		}
	}

	//Si llega al límite de discos se retorna el último disco
	return discos;

}

//Función para esperar que los procesos hijos finalicen
void esperarHijos(void){
	pid_t status;
	while(wait(&status) != -1); //wait espera a que cualquier hijo finalice, por eso el while
	return;
}


//Función encargada de recibir del pipe de cada proceso los resultados de cada hijo y escribirlos en un archivo de salida
void crearSalida(void){
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
}

//Función que se encarga de cerrar todos los pipes entre padre e hijos
void cerrarPipes(void){
	for (int i = 0; i < discos; ++i){
		close(arrayFds[i].fd[0]); //Cerrar lectura
		close(arrayFds[i].fd[1]); //Cerrar escritura
	}
	return;
}
