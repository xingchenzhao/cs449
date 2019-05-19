//xiz168, Xingchen Zhao
#include <stdio.h>
int checkRange(char word){
    if((word >= 32) &&(word<= 126)){
        return 1;
    }else{
        return 0;
    }
}

int main(int argc, const char * argv[]) {
    
    char string[200];
    int len = 0;
    char word;
    

    if(argc != 2){
        printf("Please type the arguments correctly");
        return 0;
    }
    FILE* BinaryFile = fopen(argv[1],"rb");
    
    if(BinaryFile == NULL){
        printf("Cannot read the file.");
        return 0;
    }
    
    while(feof(BinaryFile) == 0){
        fread(&word, sizeof(word),1,BinaryFile);
        if(checkRange(word) == 1){
            string[len] = word;
            len++;
            continue;
        }
        
        string[len] = '\0';
        if(len >= 4 ){
            printf("%s\n",string);
            
        }

	len = 0;
       
    }
    fclose(BinaryFile);
}
