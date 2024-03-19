#ifndef DIRWALK_H
#define DIRWALK_H
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <limits.h>
#include<getopt.h>
#include<locale.h>



void print_usage();
int compare(const void *a, const void *b);
void dirwalk(const char *dirname, int symbolic_links, int directories, int files, int sort);
int is_directory(const char *path) ;
void print(const char *path,int symbolic_links, int directories, int files) ;

#endif /* DIRWALK_H */
