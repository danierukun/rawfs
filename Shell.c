//
//  Shell.c
//  Shell
//
//  Created by Ian Bravo on 5/16/14.
//  Copyright (c) 2014 Ibrarabay. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Shell.h"
#include "Files.h"

//Llama a launchShell
void Start()
{
    LaunchShell();
}

//Captura el comando de la terminal
char* RetrieveCommand()
{
    char *input;
    char cc;
    int cont = 0;
    
    while (true)
    {
        cont = 0;
        input = (char*)malloc(100 * sizeof(char));
        
        while ((cc = getchar()) != '\n')
        {
            input[cont] = cc;
            cont++;
        }
        return input;
    }
}

//Interpreta el comando de la terminal
void ReadCommand(char* input)
{
    if (strcmp(input, "exit") == 0)
    {
        exit(1);
    }
    else if (strcmp(input, "read") == 0)
    {
        char *buffer;
        FileSize(buffer);
    }
    else if (strcmp(input, "insert") == 0)
    {
        
    }
    else if (strcmp(input, "delete") == 0 )
    {
        
    }
    else if (strcmp(input, "export") == 0)
    {
        
    }
    else if (strcmp(input, "content") == 0)
    {
        
    }
    else if (strcmp(input, "usage") == 0)
    {
        
    }
    else
    {
        printf("Comando no reconocido\n");
    }
}

//Manda a la funcion que captura el comando de terminal
void LaunchShell()
{
    while (true)
    {
        ReadCommand(RetrieveCommand());
    }
}