//
//  Shell.h
//  Shell
//
//  Created by Ian Bravo on 5/16/14.
//  Copyright (c) 2014 Ibrarabay. All rights reserved.
//

#ifndef Shell_Shell_h
#define Shell_Shell_h
#define true 1
#define false 0

void Start();
void LaunchShell();
void ReadFile();
void DeleteFile();
void ShowCommands();
void ReadCommand(char* input);
char* RetrieveCommand();

#endif
