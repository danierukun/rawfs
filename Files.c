//
//  Files.c
//  Shell
//
//  Created by Ian Bravo on 5/16/14.
//  Copyright (c) 2014 Ibrarabay. All rights reserved.
//

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Shell.h"
#include "Files.h"

//Lee el archivo y lo pasa a un arreglo de bytes
void FileSize(char* buffer)
{
    FILE *f;
    //BYTE1024 *buffer;
    //char *buffer;
    long lSize = 0;
    size_t result;
    
    f = fopen("/Users/Ian_Bravo/Desktop/Shell/Shell/kanji.ai", "rb");
    if (f == NULL)
    {
        printf("Error al abrir archivo\n");
    }
    fseek(f, 0, SEEK_END);
    lSize = ftell(f);
    rewind(f);
    
    //buffer = (BYTE1024*) malloc(sizeof(BYTE1024)*lSize);
    buffer = (char*) malloc(sizeof(char)*lSize);
    if (buffer == NULL)
    {
        free(buffer);
        printf("Error al llenar el buffer\n");
        return;
    }
    
    result = fread(buffer, 1, lSize, f);
    
    if (result != lSize)
    {
        free(buffer);
        printf("Error al leer\n");
        return;
    }
    fclose(f);
}