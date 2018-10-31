#include <sys/time.h>
#include <stdio.h>
#include <jansson.h>

int main(int argc, char *argv[])
{
    char buf[1024*1024];
    FILE *fp;
    
    if(argc < 2) {
       printf("miss arg json_file\n");
       return -1;
    }

    fp = fopen(argv[1], "r");
    fread(buf, 1, 1024*1024, fp);
    
    json_error_t error;
    struct timeval tv,tv2;
    int i;

    gettimeofday(&tv, NULL);
    
    for(i=0;i<100;i++) {
        json_t * root = json_loads(buf, 0, &error);

        if(!root) {
            printf("json load failed\n");
            return -1;
        }
    }
    gettimeofday(&tv2, NULL);
    printf("cost time %d\n", (tv2.tv_sec - tv.tv_sec)*1000000+tv2.tv_usec-tv.tv_usec);
    return 0; 
}
