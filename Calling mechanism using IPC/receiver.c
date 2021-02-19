#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFFER_SIZE 80
#define FIFO_FILE "/tmp/fifo"
int main(){
    int f;
    mkfifo(FIFO_FILE, 0666);
    char buffer_in[BUFFER_SIZE],buffer_out[BUFFER_SIZE],dummy,end[10];
    
    strcpy(end,"bye");
    while (1){
        f = open(FIFO_FILE, O_RDONLY);
        read(f, buffer_in, BUFFER_SIZE);
        printf("Caller: %s\n", buffer_in);
        close(f);
        f = open(FIFO_FILE,O_WRONLY);
        printf("Enter Message: ");
        scanf("%[^\n]s",buffer_out);
        scanf("%c",&dummy);
        write(f, buffer_out, strlen(buffer_out)+1);
        close(f);
        int end1 = strcmp(end,buffer_out);
        int end2 = strcmp(end,buffer_in);
        if(end1==0 || end2 == 0)
        {
            break;
        }
    }
    return 0;
}
