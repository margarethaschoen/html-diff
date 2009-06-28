#include <stdlib.h>
#include <stdio.h>
#include "parsediff.h"

//if set 1 the debug information will write to stdout
#define DEBUG 1

FILE *fp;
char* fname="out.df";
char buffer[1000];
errno_t err;

int main(int argc, char *argv[]){
	//process diff on normalizated file
	system("diff.exe o m > out.df");
	
		//debug information
		if(DEBUG) 
			fprintf(stdout, "...opening temporary file \"%s\" - (diff output)...\n", fname);

	if((err=fopen_s(&fp, fname, "rt")) !=0 )
	{
		fprintf(stderr, "Error opening file %s: %s\n", fname, strerror(err));
		return 1;
	}
	else
	{
			//debug information
			if(DEBUG) 
				fprintf(stdout, "...reading from temporary file \"%s\" - (diff output)...\n", fname);
		
		//line reading from file
		while(!feof(fp))
		{
			if(fgets(buffer, sizeof(buffer)-1, fp)!=NULL && errno==0)
			{
				//parse diff file
				//take operand
				take_op(buffer);

				
			}
			else if(errno)
			{
				fprintf(stderr, "Error reading file %s: %s\n", fname, strerror(errno));
				break;
			}
		}

		if(fp)
		{
				//debug information
				if(DEBUG) 
					fprintf(stdout, "...closing temporary file \"%s\" - (diff output)...\n", fname);

			//closing file
			if(fclose(fp))
				fprintf(stderr, "Error closing file %s: %s\n", fname, strerror(errno));

				//debug information
				if(DEBUG) 
					fprintf(stdout, "...deleting temporary file \"%s\" - (diff output)...\n", fname);

			//removing file
			//if(remove(fname) != 0)
			//	fprintf(stderr, "Error deleting file %s: %s\n", fname, strerror(errno));
		}
	}
	return 0;
}