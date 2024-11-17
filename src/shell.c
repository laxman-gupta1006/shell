#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

char parse_command(char* arr[]){
   printf("%s",arr);
   return arr;
}



int main(int argc,char* argv[]){
    char arr[200];
    while(1){
        printf("myshell>> ");
        fflush(stdout);
        if(fgets(arr,sizeof(arr),stdin)==NULL){
            printf("Thanks for using my shell\n");
            break;
        }
        arr[strcspn(arr,"\n")]=0;
        if(strcmp(arr,"exit")==0){
            break;
        }
        parse_command(arr);
    }
    return 0;
}