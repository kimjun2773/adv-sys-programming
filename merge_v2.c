#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define BUFFSZ 512
#define LINESZ 100

void strrev(char* head, char* tail);
void strCat(char* des,char* src);
int main(int argc, char *argv[])
{
    char buf[2][BUFFSZ];
    char tmp[LINESZ];
    char bufout[2*BUFFSZ];
    int flag[2] = {0};
    char *pos[2] = {buf[0], buf[1]};
    char *prev[2] = {buf[0], buf[1]};
    char *postmp = tmp;
    char *posout = bufout;
    int i = 0; 
    FILE *file[2], *fout;
    int eof1 = 0, eof2 = 0;
    long line[2] = {0}, lineout = 0;
    struct timeval before, after;
    int duration;
    int ret = 1;

    if (argc != 4) {
        fprintf(stderr, "usage: %s file1 file2 fout\n", argv[0]);
        goto leave0;
    }
    if ((file[0] = fopen(argv[1], "rt")) == NULL) {
        perror(argv[1]);
        goto leave0;
    }
    if ((file[1] = fopen(argv[2], "rt")) == NULL) {
        perror(argv[2]);
        goto leave1;
    }
    if ((fout = fopen(argv[3], "wt")) == NULL) {
        perror(argv[3]);
        goto leave2;
    }
    gettimeofday(&before, NULL);
    do {
	if (fread(buf[0], 1, BUFFSZ, file[0]) < BUFFSZ) eof1 = 1;
	if (fread(buf[1], 1, BUFFSZ ,file[1]) < BUFFSZ) eof2 = 1;
      	
	//smallMerge
	while (flag[0] && flag[1]){
		if ((pos[i] = strchr(prev[i],0x0a)) == NULL){ 
			fseek(file[i], -(BUFFSZ-(prev[i]-buf[i])+1), SEEK_CUR);
			fread(tmp, 1, LINESZ, file[i]);
			postmp = strchr(tmp,0x0a);
			fseek(file[i], -(SEEK_CUR-(postmp-tmp)), SEEK_CUR);
			flag[i] = 1;
		}
		if (flag[i] == 1){
			strrev(tmp,postmp);
			strCat(posout, tmp);
			posout += postmp-tmp;
		}
		else {
			strrev(prev[i], pos[i]);
			strCat(posout, prev[i]);
			posout += pos[i]-prev[i];
			line[i]++, lineout++;
		}
		i = (++i)%2;
	}
	fwrite(bufout, 1, posout-bufout, fout);
    } while (!eof1 || !eof2);
    gettimeofday(&after, NULL);
  
    duration = (after.tv_sec - before.tv_sec) * 1000000 + (after.tv_usec - before.tv_usec);
    printf("Processing time = %d.%06d sec\n", duration / 1000000, duration % 1000000);
    printf("File1 = %ld, File2= %ld, Total = %ld Lines\n", line[0], line[1], lineout);
    ret = 0;
    
leave3:
    fclose(fout);
leave2:
    fclose(file[1]);
leave1:
    fclose(file[0]);
leave0:
    return ret; 
}

void strrev(char* head, char* tail)
{
    char* start = head;
    char* end = tail-1;
    char tmp;
  
    while (start < end){
	tmp = *start;
	*start++ = *end;
	*end-- = tmp;
    }
}

void strCat(char* des, char* src)
{
	while(*src == 0x0a)
		*des++ = *src++;
	*des = *src;
}
