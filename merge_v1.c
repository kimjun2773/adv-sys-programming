#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define BUFFSZ 100

void strrev(char str[]);
int readaline_and_out(FILE *fin, FILE *fout);

int
main(int argc, char *argv[])
{
    FILE *file1, *file2, *fout;
    int eof1 = 0, eof2 = 0;
    long line1 = 0, line2 = 0, lineout = 0;
    struct timeval before, after;
    int duration;
    int ret = 1;

    if (argc != 4) {
        fprintf(stderr, "usage: %s file1 file2 fout\n", argv[0]);
        goto leave0;
    }
    if ((file1 = fopen(argv[1], "rt")) == NULL) {
        perror(argv[1]);
        goto leave0;
    }
    if ((file2 = fopen(argv[2], "rt")) == NULL) {
        perror(argv[2]);
        goto leave1;
    }
    if ((fout = fopen(argv[3], "wt")) == NULL) {
        perror(argv[3]);
        goto leave2;
    }
    gettimeofday(&before, NULL);
    do {
        if (!eof1) {
            if (!readaline_and_out(file1, fout)) {
                line1++; lineout++;
            } else
                eof1 = 1;
        }
        if (!eof2) {
            if (!readaline_and_out(file2, fout)) {
                line2++; lineout++;
            } else
                eof2 = 1;
        }
    } while (!eof1 || !eof2);
    gettimeofday(&after, NULL);
    
    duration = (after.tv_sec - before.tv_sec) * 1000000 + (after.tv_usec - before.tv_usec);
    printf("Processing time = %d.%06d sec\n", duration / 1000000, duration % 1000000);
    printf("File1 = %ld, File2= %ld, Total = %ld Lines\n", line1, line2, lineout);
    ret = 0;
    
leave3:
    fclose(fout);
leave2:
    fclose(file2);
leave1:
    fclose(file1);
leave0:
    return ret; 
}

/* Read a line from fin and write it to fout */
/* return 1 if fin meets end of file */
int
readaline_and_out(FILE *fin, FILE *fout)
{    
    char* ptr;
    char buffer[BUFFSZ];
    int cnt;

    cnt = fread(buffer, 1 , BUFFSZ, fin);
    if (cnt < BUFFSZ){
	strrev(buffer);
	fwrite(buffer, 1, BUFFSZ, fout);
	return 1;
    }
    else {
	ptr = strchr(buffer, 0x0a);
        cnt = ptr - buffer + 1;
	strrev(buffer);
	fwrite(buffer, 1, cnt, fout);
	fseek(fin, -(BUFFSZ - cnt), SEEK_CUR);
    	return 0;
   }
}

void strrev(char str[])
{
    char* start = str;
    char* end = str;
    char tmp;
  
    while (*end != 0x0a)
	end++;
    end--;
    while (start < end){
	tmp = *start;
	*start++ = *end;
	*end-- = tmp;
    }   	
}
