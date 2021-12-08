# Project 03: Shell

## Overview
In this project, you will implement a simple shell. The shell should operate in the following basic way: when you type in a command (in response to the shell prompt), the shell creates a child process that executes the command that was entered, then prompts for more user input when finished. The shell you implement will be similar to one that you use every day in Linux (e.g., `bash`), but with many simplifications. 

### Learning objectives
After completing this project, you should be able to:
* Write programs that create and manage processes using the UNIX process API

### Logistics
This project only has one part, which must be completed by **Thursday, December 2 at 11pm**. You will not have an opportunity to revise your code or earn back points for this project, so you should use the feedback you receive on Lab 10 to avoid making similar mistakes.

### Important tips
* Read the entirety of the project description before you write any code.
* Work on the project over multiple sessions. Do not try to complete the project in a single session on the day (before) the project is due. This will give you time to think about how to solve problems, allow you to ask questions, and result in better outcomes.
* Ensure you follow good program design, coding, testing, and debugging practices (details below).

## Getting started 
Clone your repository on the COSC 208 servers. 

To compile the shell, run
```bash
$ make
``` 

To start the shell, run
```bash
$ ./shell
```

Your shell will run in an interactive fashion. It will display a prompt (`shell>`) and wait for the user to enter a command. The user will enter the program to execute (e.g., `/bin/ls`) or a "built-in" command (e.g., `exit`). Non-built-in commands should contain the full path to the executable file (e.g., `/bin/ls` rather than just `ls`) and may contain an arbitrary number of command-line options (e.g., `/bin/ls -l -r`).  If the user enters a non-built-in command, a new process will be created and the specified program will be started. The shell will wait for the program to finish (unless the command includes an ampersand `&` at the end to indicate it should run in the background), display the prompt, and allow the user to enter another command.

The main function in `main.c` includes code for:
1. Displaying the prompt (`shell>`)
2. Reading a line of user input into the char array `buffer`
3. Breaking the command into tokens, where tokens are delimited by one or more whitespace characters

## Non-built-in commands
When a user enters a non-built-in command (e.g., `/bin/ls -l`), the shell should create a new process and invoke the specified executable program in the new process. 

Optionally, a user can indicate that a command should be executed as a *background* process by including the ampersand (`&`) character at the end of the command. For simplicity, you can assume the ampersand will always be a separate token. For example:
```
shell> /bin/ls -l -r &
```
Commands without an ampersand should be executed as a *foreground* process.

The shell **must** gracefuly handle the situation where invoking the specified executable fails.

If the command should run in the **foreground**: the shell **must** wait for the process to complete before prompting for a new command. (Hint: use `waitpid` instead of `wait`, otherwise you'll run into problems when commands are running the background.)

If the command should run in the **background**: the shell must **not** wait for the process to complete before prompting for a new command—i..e., you should be able to type a new command in the shell immediately. Additionally, you should store the background process's pid in an array of active background process. You can assume a fixed upper bound on the number of active background processes, as defined by the constant `MAX_BACKGROUND`.

## Built-in commands
There are two built-in commands you need to handle: `exit` and `jobs`.

Note that built-in commands must be handled differently than other commands; they must **not be executed in a separate process** like `/bin/ls` or other programs invoked through the shell.  

### exit
To quit the shell, the user can type `exit`. The effect should be to quit the shell. (The `exit()` system call may be useful here.)

### jobs
The `jobs` comand should: (1) check whether any background processes have finished, and (2) print a list of processes that are still running in the background. For example:
```
shell> jobs
13346 Done
13347 Running
13348 Runing
13349 Done
```

In particular, you should scan the list of background processes. For each process on the list, do the following:
* Check whether it has completed using the `waitpid` system call, which takes three parameters: the process id, a pointer to an int (which will be the exit status of the child process), and an integer flag.  For the flag value you should use `WNOHANG`, which tells `waitpid` not to "hang" or wait if the process is not finished.  If the child is done, `waitpid` will return the child's pid.  If the child is not yet done, `waitpid` will return `0`.  If there's an error with `waitpid`, it will return `-1`.  (See `man 2 waitpid` for more details.)
* If the processes has finished, the shell should:
	* Print `PID Done`, replacing ``PID` with the pid of the process that finished
	* Remove the pid from the array of active background processes
* If the process has not finished, the shell should:
	* Print `PID Running`, replacing ``PID` with the pid of the process that has not yet finished


## Testing your code
Below is an example shell session that demonstrates some of the ways you can test your shell.

```
shell> /bin/ls
Makefile  README.md  shell  shell.c  shell.o
shell> /bin/ls -a
.  ..  .git  .gitignore  Makefile  README.md  shell  shell.c  shell.o
shell> /bin/date
Fri Nov 12 20:14:06 UTC 2021
shell> /bin/sleep 5
shell> /bin/sleep 10 &
shell> jobs
5329 Running
shell> /bin/ps
    PID TTY          TIME CMD
   5066 pts/1    00:00:00 bash
   5092 pts/1    00:00:00 shell
   5329 pts/1    00:00:00 sleep
   5330 pts/1    00:00:00 ps
shell> /bin/which ls
/usr/bin/ls
shell> jobs
5329 Done 
shell > exit
```

Notes: 
* When you run `/bin/sleep 5`, the following `shell>` prompt should not appear for 5 seconds.  However, when your run `/bin/sleep 10 &`, the following `shell>` prompt should appear immediately.  
* If you run `jobs` less than 10 seconds after running `/bin/sleep 10 &`, then `jobs` should display the PID associated with the sleep process as running. Whereas, if you run `jobs` at least 10 seconds after running `/bin/sleep 10 &`, then `jobs` should display the PID associated with the sleep process as done.

## Program design

You **must follow good program design and coding practices**, including:
* Checking the return values of all system calls (excluding `printf`) — This will often catch errors in how you are invoking these systems calls.
* Using multiple functions — Do not put all of your code in the `main` function. You should use multiple functions, where each function: is Short, does One thing, takes Few parameters, and maintains a single level of Abstraction. In other words, follow the _SOFA_ criteria from COSC 101.
* Freeing all heap-allocated memory before exiting — Run `valgrind` to confirm all memory is freed and no other memory errors exist in your program.
* Properly indenting your code — Recall that indentation is not semantically significant in C, but it makes your code much easier to read.
* Including comments — If you write more than a few lines of code in the body of a function, then you must include comments; generally, you should include a comment before each conditional statement, loop, and set of statements that perform some calculation. **Do not** include a comment for every line of code, and **do not** simply restate the code.
* Making multiple commits to your git repository — Do not wait until your entire program is working before you commit it to your git repository. You should commit your code each time you write and debug a piece of functionality.

## Submission instructions
You should **commit and push** your updated files to your git repository. However, as noted above, do not wait until your entire program is working before you commit it to your git repository; you should commit your code each time you write and debug a piece of functionality. 