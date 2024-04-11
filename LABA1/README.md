# Directory Walker

## Overview
This project implements a simple directory walker utility in C language. It traverses through directories and prints the paths of files, directories, or symbolic links based on specified options.

## Prerequisites
- This project is developed in C language and requires a C compiler (GCC recommended) to build and run.
- Ensure you have GNU Make installed to compile the project using the provided Makefile.

## Compilation
To compile the project, run the following command in the terminal:
```
make
```
This will build the project in debug mode. To build in release mode, use:
```
make MODE=release
```
## Usage
Once compiled, the executable `walkdir` will be generated in the `build/debug` or `build/release` directory based on the mode you selected. You can run the utility from the command line with the following syntax:
```
./walkdir [dirname] [options]
```
### Options
- `-l`: Only display symbolic links.
- `-d`: Only display directories.
- `-f`: Only display regular files.
- `-s`: Sort the output by LC_COLLATE.

If no options are provided, the utility will default to display all: symbolic links, directories, and regular files.

## Example Usage
To list all files and directories in the current directory:
```
./walkdir
```


## File Structure
- `dirwalk.h`: Header file containing function prototypes and necessary includes.
- `dirwalk.c`: Implementation of directory walking functions.
- `main.c`: Main program implementing command-line parsing and program execution logic.

## Build Structure
- `src/`: Contains the source files.
- `build/debug/`: Output directory for debug builds.
- `build/release/`: Output directory for release builds.

## Cleanup
To clean the project directory and remove all generated files, run:
```
make clean
```
