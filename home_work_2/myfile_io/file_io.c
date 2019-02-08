#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_CHARS				256



int main(int argc, char** argv){

	printf("This program demonstrates file input and output operations!!\n");

	/*Create a new file*/
	FILE *pFile = fopen("test_file.txt", "w");

	if(pFile == NULL)
		printf("Unable to open file\n");
	
	/*Change the permission of the file to write*/
	const char* filePath = "/test_file.txt";
	

	chmod(filePath, S_IWUSR);

	/*Open the file in writing mode*/
	fopen(filePath, "w");

	/*Write a character to the file*/
	fputc('A', pFile);

	/*Clost the file*/
	fclose(pFile);

	/*Open the file in append mode*/
	fopen(filePath, "a");

	/*Dynamically allocate an array*/
	char *pMemory = (char*)malloc(sizeof(char) * MAX_CHARS);

	/*Read an input string and write it to the memory*/
	fgets(pMemory, MAX_CHARS, stdin);

	/*Write the string to the file*/
	fputs(pMemory, pFile);

	/*Flush file output*/
	fflush(pFile);

	/*Close the file*/
	fclose(pFile);

	/*Change file permissions before opeing it in reading mode*/
	chmod(filePath, S_IRUSR);

	/*Open file for reading*/
	fopen(filePath, "r");

	/*read a single character from the file*/
	char c = fgetc(pFile);
	printf("%c\n", c);

	/*read a string from the file*/
	char string[MAX_CHARS]; 

	fgets(string, 20, pFile);
	printf("%s\n", string); 

	/*Change file permission to read mode before reading it
	again without building and deleting the file fron the directory*/
	chmod(filePath, S_IRUSR | S_IWUSR);

	/*Close the file*/
	fclose(pFile);

	/*free memory*/
	free(pMemory);

	return 0;

}