#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char * elimComasNombre(char * linea){
    
    int comas = 0;
    int comillas = 0;

    for(int i = 0; i < strlen(linea); i++){
        if (linea[i] == '"')
        {
            comillas += 1;
        }
        if (comillas == 1 && linea[i] == ',')
        {
            linea[i] = '.';
        }
    }
    return linea;
}

void main(){


    int id;
	char nombre[50];
	int edad;
	float precio;
	char comingSoon[5];
	int year;
	char free[5];
	char w[3];
	char m[3];
	char l[3];


    char linea[] = "58570,The Next BIG Thing,0,3.74,False,2011,False,Yes,Yes,No";
    char delimitador[] = ",";
    printf("%s\n", linea);
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
    

    /*if(sscanf(linea, "%d,%s,%d,%lf,%s,%d,%s,%s,%s,%s", &id, &nombre, &edad, &precio, &coomingSoon, &year, &free, &w, &m, &l) != 10){
		printf("Error al leer la linea, %d\n", sscanf(linea, "%d, %s,%d,%lf,%s,%d,%s,%s,%s,%s", &id, &nombre, &edad, &precio, &coomingSoon, &year, &free, &w, &m, &l));
	}*/

    //printf("%d, %s,%d,%f,%s,%d,%s,%s,%s,%s\n", id, nombre, edad, precio, comingSoon, year, free, w, m, l);

    return;
}