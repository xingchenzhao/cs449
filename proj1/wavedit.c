#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
//Xingchen Zhao xiz168
typedef struct WAVHeader{
    char riff_id[4];
    unsigned int file_size;
    char wave_id[4];
    char fmt_id[4];
    int fmt_size;
    uint16_t data_format;
    uint16_t number_of_channels;
    int samples_per_second;
    int bytes_per_second;
    uint16_t block_alignment;
    uint16_t bits_per_sample;
    char data_id[4];
    int data_size;
}WAVHeader;

void check_if_exists(FILE* file_name){
    if(file_name == NULL){
        printf("File not found. Exiting the program.\n");
        exit(0);
    }
}

void check_if_wav_real(FILE* wav_input,WAVHeader wav){
    int real_or_not = 1;//1 is true, 0 is false
    if(strncmp(wav.riff_id,"RIFF",4)!=0){
        real_or_not = 0;
    }
    if(strncmp(wav.wave_id, "WAVE", 4)!=0){
        real_or_not = 0;
    }
    if(strncmp(wav.fmt_id, "fmt ", 4)!=0){
        real_or_not = 0;
    }
    if(strncmp(wav.data_id, "data", 4)!=0){
        real_or_not = 0;
    }
    if(wav.fmt_size != 16){
        real_or_not = 0;
    }
    if(wav.data_format != 1){
        real_or_not = 0;
    }
    if(!((wav.number_of_channels == 1)||(wav.number_of_channels == 2))){
        real_or_not = 0;
    }
    
    if(!((wav.samples_per_second > 0) && (wav.samples_per_second <= 192000))){
        real_or_not = 0;
    }
    if(!((wav.bits_per_sample == 8)||(wav.bits_per_sample ==16))){
        real_or_not = 0;
    }
    if(!(wav.bytes_per_second == (wav.samples_per_second * (wav.bits_per_sample)/8 * wav.number_of_channels))){
        real_or_not = 0;
    }
    if(!(wav.block_alignment == ((wav.bits_per_sample)/8 * wav.number_of_channels))){
        real_or_not = 0;
    }
    if(real_or_not == 0){
        printf("The wav file is not real!\n");
        exit(0);
    }
}

void display_info(WAVHeader wav){
    char* channel;
    if(wav.number_of_channels==1){
        channel = "mono";
    }else {
        channel = "stereo";
    }
    printf("This is a %d-bit %dHz %s sound\n",wav.bits_per_sample,wav.samples_per_second,channel);
    int length_of_data_sample = wav.data_size/wav.block_alignment;
    float length_of_data_seconds =((float)length_of_data_sample/wav.samples_per_second);
    printf("It is %d samples (%.3f seconds) long.\n",length_of_data_sample,length_of_data_seconds);
    
}
void no_arguments(){
    printf("Sorry! You should put arguments.\n");
    printf("Usage:\n");
    printf("1.(Checking if it is a wav file and showing the info): ./wavedit [FILENAME] \n");
    printf("2.(Setting the sound's sample rate): ./wavedit [FILENAME] -rate[RATE] \n");
    printf("3.(Reversing a sound): ./wavedit [FILENAME] -reverse\n");
    printf("Examples:\n");
    printf("./wavedit somefile.wav\n");
    printf("./wavedit somefile.wav -rate 22050\n");
    printf("./wavedit somefile.wav -reverse\n");
}

WAVHeader set_rate(WAVHeader wav,int rate){
    wav.samples_per_second = rate;
    wav.bytes_per_second = (wav.samples_per_second * (wav.bits_per_sample)/8 * wav.number_of_channels);
    return wav;
}

void reverse(WAVHeader wav,FILE* wav_input){//return 0 means cannot reverse,return1 means successful
    int data_length = wav.data_size/ wav.block_alignment;
    uint8_t eightBitArr[data_length];
    uint16_t sixteenBitArr[data_length];
    uint32_t thirtyTwoBitArr[data_length];
    if(wav.bits_per_sample == 8 && wav.number_of_channels == 1){
        fread(&eightBitArr, sizeof(eightBitArr), 1, wav_input);
        for(int i = 0; i< data_length/2;i++){
            int temp = eightBitArr[i];
            eightBitArr[i] = eightBitArr[data_length-1-i];
            eightBitArr[data_length-1-i] = temp;
        }
        fseek(wav_input, sizeof(WAVHeader), SEEK_SET);
        fwrite(&eightBitArr,sizeof(eightBitArr),1,wav_input);
        fclose(wav_input);
    }else if((wav.bits_per_sample == 16 && wav.number_of_channels == 1)||
             (wav.bits_per_sample == 8 && wav.number_of_channels == 2)){
        fread(&sixteenBitArr, sizeof(sixteenBitArr), 1, wav_input);
        for(int i = 0; i< data_length/2;i++){
            int temp = sixteenBitArr[i];
            sixteenBitArr[i] = sixteenBitArr[data_length-1-i];
            sixteenBitArr[data_length-1-i] = temp;
        }
        fseek(wav_input, sizeof(WAVHeader), SEEK_SET);
        fwrite(&sixteenBitArr,sizeof(sixteenBitArr),1,wav_input);
        fclose(wav_input);
    }else if (wav.bits_per_sample == 16 && wav.number_of_channels == 2){
        fread(&thirtyTwoBitArr, sizeof(thirtyTwoBitArr), 1, wav_input);
        for(int i = 0; i< data_length/2;i++){
            int temp = thirtyTwoBitArr[i];
            thirtyTwoBitArr[i] = thirtyTwoBitArr[data_length-1-i];
            thirtyTwoBitArr[data_length-1-i] = temp;
        }
        fseek(wav_input, sizeof(WAVHeader), SEEK_SET);
        fwrite(&thirtyTwoBitArr,sizeof(thirtyTwoBitArr),1,wav_input);
        fclose(wav_input);
    }
}

int main(int argc, char ** argv) {
    FILE* wav_input;
    WAVHeader wav;
    if(argc == 1){
        no_arguments();
        exit(0);
    }else if (argc == 2){
        wav_input = fopen(argv[1],"rb");
        check_if_exists(wav_input);
        fread(&wav,sizeof(WAVHeader),1,wav_input);
        check_if_wav_real(wav_input, wav);
        display_info(wav);
    }else if((argc == 4) && (strcmp(argv[2],"-rate")==0)){
        wav_input = fopen(argv[1], "r+b");
        check_if_exists(wav_input);
        fread(&wav,sizeof(WAVHeader),1,wav_input);
        check_if_wav_real(wav_input, wav);
        WAVHeader newWav = set_rate(wav, atoi(argv[3]));
        fseek(wav_input, 0, SEEK_SET);
        fwrite(&newWav, sizeof(WAVHeader), 1, wav_input);
        fclose(wav_input);
    }else if((argc == 3)&& (strcmp(argv[2],"-reverse")==0)){
        wav_input = fopen(argv[1], "r+b");
        check_if_exists(wav_input);
        fread(&wav,sizeof(WAVHeader),1,wav_input);
        check_if_wav_real(wav_input, wav);
        reverse(wav, wav_input);
    }else{
        printf("Wrong arguments format!\n");
    }
}



