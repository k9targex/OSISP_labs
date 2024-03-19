#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "dirwalk.h"
void print(const char *path, int symbolic_links, int directories, int files) {
    struct stat statbuf;

    if (stat(path, &statbuf) == -1) {
        fprintf(stderr, "Cannot access file: %s\n", path);
        return;
    }

    int is_dir = S_ISDIR(statbuf.st_mode);
    int is_link = S_ISLNK(statbuf.st_mode);
    int is_file = S_ISREG(statbuf.st_mode);
    
    if ((symbolic_links && is_link) || (directories && is_dir) || (files && is_file)) {
        printf("%s\n", path);
    }
}
void print_usage() {
    printf("Usage: dirwalk [dirname] [options]\n");
    printf("Options:\n");
    printf("-l   Only symbolic links\n");
    printf("-d   Only directories\n");
    printf("-f   Only files\n");
    printf("-s   Sort output by LC_COLLATE\n");
}
// int compare(const struct dirent **a, const struct dirent **b) {
//     return strcoll((*a)->d_name, (*b)->d_name);
// }
int compare(const void *a, const void *b) {
    const struct dirent *entry_a = *(const struct dirent **)a;
    const struct dirent *entry_b = *(const struct dirent **)b;
    return strcoll(entry_a->d_name, entry_b->d_name);
}
int is_directory(const char *path) {
    struct stat path_stat;
    if (lstat(path, &path_stat) != 0) {
        return 0;
    }
    return S_ISDIR(path_stat.st_mode);
}

void dirwalk(const char *dirname, int symbolic_links, int directories, int files, int sort){
    
    struct dirent **namelist;
    int count = scandir(dirname, &namelist, NULL, NULL);

    if (count == -1) {
        fprintf(stderr, "Cannot open directory: %s\n", dirname);
        return;
    }
     if (sort){
        qsort(namelist, count, sizeof(struct dirent *), compare);
    }

    for (int i = 0; i < count; i++) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dirname, namelist[i]->d_name);

        if (strcmp(namelist[i]->d_name, ".") == 0 || strcmp(namelist[i]->d_name, "..") == 0) {
            free(namelist[i]);
            continue;
        }

        print(path, symbolic_links,directories,files);
      

       if (is_directory(path)){
            dirwalk(path, symbolic_links,directories,files,sort);
        }

        free(namelist[i]);
    }

    free(namelist);
}

