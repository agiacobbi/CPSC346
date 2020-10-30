/**
 Alex Giacobbi
 CPSC 346 - Project 3
 10/29/20
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#define MAX_LINE       80 /* 80 chars per line, per command, should be enough. */
#define MAX_COMMANDS    9 /* size of history */

char history[MAX_COMMANDS][MAX_LINE]; //the array used to store history commands.
char display_history[MAX_COMMANDS][MAX_LINE]; 
/*the array used for "printf" to display history nicely. Remove special characters like "\n" or "\0"*/

int command_count = 0;

/**
 * Add the most recent command to the history.
 */
void addtohistory(char inputBuffer[]) {
	// shift all history entries back
	int commandIndex = (command_count < MAX_COMMANDS) ? command_count : MAX_COMMANDS;
	if (command_count > MAX_COMMANDS) {
		for (int i = 1; i < commandIndex; i++) {
			strncpy(history[i - 1], history[i], MAX_LINE);
			strncpy(display_history[i - 1], "                    ", MAX_LINE);
			strncpy(display_history[i - 1], display_history[i], MAX_LINE);
		}
	}

	// update array "history": add the command to history, strcpy(str1,str2);
	strcpy(history[commandIndex - 1], inputBuffer);
	
	// update array "display_history": remove characters like '\n', '\0' in order to display nicely
	int i = 0;
	while (inputBuffer[i] != '\n' && inputBuffer[i] != '\0') {
		display_history[commandIndex - 1][i] = inputBuffer[i];
		i++;
	}
	while (i < MAX_LINE) {
		display_history[command_count - 1][i] = '\0';
		i++;
	}

	return;
}

/** 
 * The setup function below will not return any value, but it will just: read
 * in the next command line; separate it into distinct arguments (using blanks as
 * delimiters), and set the args array entries to point to the beginning of what
 * will become null-terminated, C-style strings. 
 */

int setup(char inputBuffer[], char *args[], int *background)
{
    int length,		/* # of characters in the command line */
	i,				/* loop index for accessing inputBuffer array */
	command_number;	/* index of requested command number */

	int isFirstCharacter = 1;
	int stringStartIndex = 0;
	int newLineDetected = 0;
	int argCount = 0;
	int commandIndex = (command_count < MAX_COMMANDS) ? command_count : MAX_COMMANDS;
	
    /* read what the user enters on the command line */
	do {
		printf("osh> ");
		fflush(stdout);
		length = read(STDIN_FILENO, inputBuffer, MAX_LINE); 
	}
	while (inputBuffer[0] == '\n'); /* swallow newline characters */


    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
 
    if ( (length < 0) ) {
		perror("error reading the command");
		exit(-1);           /* terminate with error code of -1 */
    }
	
	/**
	 * Check if they are using history
	 */
	if (inputBuffer[0] == '!') {
		if (command_count == 0) {
			printf("No commands in history\n");
			return -1;
		} 
		
		if (inputBuffer[1] == '!') {
			strcpy(inputBuffer, history[commandIndex - 1]);
		} else {
			command_number = atoi(&inputBuffer[1]);
			if (command_number > 0 && command_number <= commandIndex) {
				strcpy(inputBuffer, history[command_number - 1]);
			} else {
				printf("No such command in history\n");
				return -1;
			}
		}
	}

	/**
	 * Add the command to the history
	 */
	length = strlen(inputBuffer);
	command_count++;
	addtohistory(inputBuffer); 
	
	/**
	 * Parse the contents of inputBuffer
	 */
    for (i = 0; i < length; i++) { 
		/* examine every character in the inputBuffer */
        switch (inputBuffer[i]) {
			case ' ':
			case '\t' : /* argument separators */
				if (!isFirstCharacter) {
					args[argCount] = &inputBuffer[stringStartIndex];
					isFirstCharacter = 1;
					argCount++;
				}

				inputBuffer[i] = '\0';
				break;

			case '\n':  /* should be the final char examined */
				/* no more arguments to this command */	
				if (!isFirstCharacter) {
					args[argCount] = &inputBuffer[stringStartIndex];
					newLineDetected = 1;
					argCount++;
				}

				inputBuffer[i] = '\0';
				break;

				
			default:  /* some other character */
				if (isFirstCharacter) {
					if (inputBuffer[i] == '&') {
						*background = 1;
						break;
					}

					stringStartIndex = i;
					isFirstCharacter = 0;
				} 
			/* args[i] is a pointer to a string, its value is the address of the first charater of that string
			* You want to track the location of the beginning character of each string. 
			* The location is the first character, which is not '\t', not '\t', and not '\n'
			* You also need check "&". If '&' is detected, setup background flag.
			*/  
		} /* end of switch */

		// Stop reading at new line
		if (newLineDetected) {
			break;
		}
	} /* end of for */
	
	/**
	 * Here you finish parsing the input. 
	 * There is one more thing to assure. If we get '&', make sure you don't enter it in the args array
	 */
	args[argCount] = NULL;

	return 1;
} /* end of setup routine */


int main(void)
{
	char inputBuffer[MAX_LINE]; 	/* buffer to hold the command entered */
	int background;             	/* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE / 2 + 1];	/* command line (of 80) has max of 40 arguments */
	pid_t child;            		/* process id of the child process */
	
	//define your local variables here, at the beginning of your program.
	int shouldrun = 1;
		
    while (shouldrun) {            		/* Program terminates normally inside setup */
		background = 0;
		shouldrun = setup(inputBuffer, args, &background);       /* get next command */

		/* 
		* if the user typed in "exit", the shell program will return (or terminate). 
		* Call strncmp(str1,str1,count). The function call will return 0 if str1 == str2.
		* "count" is the number of characters we use to compare.    
		*/
		if (strncmp(inputBuffer, "exit", 4) == 0) {
			printf("Bye!\n");
			// return 0;
			exit(0);
		}
		
		/* if the user typed in "history", the shell program will display the history commands. 
		* you will use "printf" to print the display_history
		* after you display all the array, this command is done. 
		* Your program should go to read again, which means calling the "setup" function.  
		*/
		else if (strncmp(inputBuffer, "history", 7) == 0) {
			int count = (command_count < MAX_COMMANDS) ? command_count : MAX_COMMANDS;
			while (count > 0) {
				printf("%d. %s\n", count, display_history[count - 1]);
				count--;
			}
			continue;
		}
		
		if (shouldrun) {
			/* creates a duplicate process! */
			/* pid < 0  error
			*  pid == 0, it is the child process. use the system call execvp(args[0], args);
			*  pid > 0, it is the parent. Here you need consider it is foreground or background
			*/
			child = fork();
			
			if (child < 0) {
				printf("An error occurred. Exiting.\n");
				return -1;
			} else if (child == 0) {
				execvp(args[0], args);
			} else {
				if (!background) {
					while (child != wait(NULL));
				}
			}
		}
    }
	
	return 0;
}