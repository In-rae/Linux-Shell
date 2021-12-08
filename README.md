This is a reupload. Additional credit to my partner Malcolm Bell, credit to Colgate University for starter code.

# Linux Shell



To start the shell, run
```bash
$ ./shell
```

Example of running a program in the background:

```
shell> /bin/ls -l -r &
```
The shell can run programs at the specified path with valid command line arguments. Adding & to the end of the input will run the process in the background meaning you can immediately enter another command while the process is still running. If not specified, the program will run in the foreground, with the shell waiting for it to finish before anything else can be done. The built in "jobs" command will print a list background process ids and if they have finished or are still running. Once a process has been indicated as finished once, it will be removed from the jobs list. The built in "exit" command will clear all heap-allocated memory and exit the shell. 
