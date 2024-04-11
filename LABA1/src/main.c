
#define _POSIX_C_SOURCE 200809L
#include "dirwalk.h"



int main(int argc, char *argv[]) {
    int opt;
    int symbolic_links = 0, directories = 0, files = 0, sort = 0;

    setlocale(LC_COLLATE, "");
  
    while ((opt = getopt(argc, argv, "ldfs")) != -1) {
        switch (opt) {
            case 'l':
                symbolic_links = 1;
                break;
            case 'd':
                directories = 1;
                break;
            case 'f':
                files = 1;
                break;
            case 's':
                sort = 1;
                break;
            default:
                print_usage();
                return EXIT_FAILURE;
        }
    }
    if (symbolic_links == 0 && directories == 0 && files == 0 && sort == 0) {
        symbolic_links = 1;
        directories = 1;
        files = 1;
        sort = 0;
    }
    if (optind >= argc) {
        dirwalk(".", symbolic_links, directories, files, sort);
    } else {
        for (; optind < argc; optind++) {
            dirwalk(argv[optind], symbolic_links, directories, files, sort);
        }
    }

    return 0;
}
