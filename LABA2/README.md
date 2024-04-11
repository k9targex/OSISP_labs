
# 2nd lab

### What does this program do?

This program is able to fork and run a child process speicfied with `CHILD_PATH` variable.

### How to build this project

This project can be built using `make` utility. 

```
make 
```



### How to use this program
## Example Usage
To list all files and directories in the current directory:
```
./parent env.txt
```
Program can be controlled using following keys:

* `+` to run child according to `getenv()`
* `*` to run child according to `envp`
* `&` to run child according to `environ`
* `q` to exit
