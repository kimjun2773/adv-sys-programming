#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define BUFFSZ 512
#define LINESZ 100

void strRev(char* head, char* tail);
void strCat(char* des,char* src);
void read_n_init(char* buffer, FILE* fp, int* eof, char* prev, char* pos);
int main(int argc, char *argv[])
{
    char buf[2][BUFFSZ];
    char bufout[2*BUFFSZ];
    char *pos[2];
    char *prev[2];
	char *posout;
    int i = 0; 
	int cnt;
    FILE *file[2], *fout;
	int eof[2] = {0};
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
	if (fread(buf[0], 1, BUFFSZ, file[0]) < BUFFSZ) eof[0] = 1;
	if (fread(buf[1], 1, BUFFSZ ,file[1]) < BUFFSZ) eof[1] = 1;
	prev[0] = buf[0];
	prev[1] = buf[1];
	pos[0] = buf[0];
	pos[1] = buf[1];
   
	do {	
		posout = bufout;
		while (1){
			if ((pos[i] = strchr(prev[i],0x0a)) == NULL){
				cnt = prev[i] - buf[i];
				strncpy(buf[i], prev[i], BUFFSZ - cnt);
				cnt = BUFFSZ - cnt;
				if (fread(buf[i] + cnt, 1, BUFFSZ - cnt, file[i]) < BUFFSZ) eof[i] = 1;
				prev[i] = buf[i];
				pos[i] = buf[i];
				continue;
			}


			if(2*BUFFSZ - (posout-bufout) < LINESZ)
				break;


			strRev(prev[i], pos[i]);
			strCat(posout, prev[i]);
			posout += pos[i]-prev[i];
			line[i]++, lineout++;


			if(pos[i] - buf[i] < BUFFSZ)
				prev[i] = pos[i] + 1;
			else {
				if (fread(buf[i], 1, BUFFSZ, file[i]) < BUFFSZ) eof[i] = 1;
				prev[i] = buf[i];
				pos[i] = buf[i];
			}


			i = (++i)%2;
		}


		fwrite(bufout, 1, posout-bufout, fout);
	} while (!(eof[0] && eof[1]));


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

void strRev(char* head, char* tail)
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
	while(*src != 0x0a)
		*des++ = *src++;
	*des = *src;
}
void read_n_init(char* buffer, FILE* fp, int* eof, char* prev, char* pos)
{
	if (fread(buffer, 1, BUFFSZ, fp) < BUFFSZ) *eof = 1;
	prev = buffer;
	pos = buffer;
}
