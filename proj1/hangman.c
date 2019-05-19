#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
//Xingchen Zhao xiz168
void get_line(char* buffer, int size) {
    fgets(buffer, size, stdin);
    int len = strlen(buffer);
    if(len != 0 && buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';
}

void remove_newLine(char* str){
    int new_line = strlen(str)-1;
    if(str[new_line]=='\n'){
        str[new_line] = '\0';
    }
}
int streq_nocase(const char* a, const char* b) {
    // hohoho aren't I clever
    for(; *a && *b; a++, b++) if(tolower(*a) != tolower(*b)) return 0;
    return *a == 0 && *b == 0;
}

int random_range(int low_value,int high_value){
    srand((unsigned int)time(NULL));
    int random_num = rand() % (high_value - low_value) + low_value;
    return random_num;
}

int main(int argc, char ** argv) {

    char *random_word;
    
    //convert char number to integer number
    if(argc >1){
       random_word = argv[1];
    }
    else{
    FILE* fInput = fopen("dictionary.txt", "r");
    if(fInput == NULL){
	printf("that file doesn't exist!\n");
	exit(0);
    }
    char length[20];
    fgets(length, 20, fInput);
    int lengthNum = atoi(length);//convert string to int
    
    
    char words[lengthNum][20];//create words dictionary
    
    for(int i =0; i< lengthNum;i=i+1){
        fgets(words[i], 20, fInput); //put words into the dictionary
    }
    fclose(fInput);//close file
    
    int random_num = random_range(0, lengthNum);//generate a random number
    remove_newLine(words[random_num]);//remove '\n' from the word.
    random_word = words[random_num];
    }
    
    int random_word_length = (int)strlen(random_word);

    printf("Welcome to hangman! Your word has %d letters:\n",random_word_length);
    
    int strikes = 0;
    int correctGuess=0;
    char input[20];
    char guess_word[random_word_length];//create a guess string
    for(int i =0; i<random_word_length;i++){
        guess_word[i] = '_';
    }
       
    while((correctGuess < random_word_length)){
        if(strikes==5){
            break;
        }
        int true_or_false = 0;//0 is false, 1 is true
        for(int i=0; i < random_word_length;i++){
	  printf("%c ",guess_word[i]);
	}
        
        printf("Guess a letter or type the whole word: ");
        get_line(input, sizeof(input));
       
       
        if(strlen(input)==random_word_length){
            if(streq_nocase(input, random_word)==1){
                strcpy(guess_word, random_word);
                true_or_false = 1;
                break;
            }
        }else if(strlen(input)==1){
            char input_char = tolower(input[0]);
            for(int i=0; i< random_word_length ;i++){
                if(input_char ==tolower(random_word[i])){
                    if(guess_word[i] =='_'){
                        guess_word[i] = input_char;
                        correctGuess++;
                    }
                    true_or_false = 1;
                }
            }
        }
        if(true_or_false == 0){
            strikes++;
            printf("Strikes %d!\n",strikes);
        }
    }
    
    if(streq_nocase(guess_word,random_word)==1){
        printf("You got it! The word was '%s'.\n",random_word);
    }else{
        printf("Sorry, you lost! The word was '%s'.\n",random_word);
    }
    
}
