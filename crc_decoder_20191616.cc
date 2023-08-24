#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define MAX 10 * 10 * 1024 * 1024

int cal_remainder(char *frame, char *generator)
{
    int f_len = strlen(frame);
    int g_len = strlen(generator);
    int r_len = g_len - 1;
    char *dividend = (char *)malloc(f_len + 1);
    strcpy(dividend, frame);
    dividend[f_len] = '\0';
    for (int i = 0; i < f_len - r_len; i++)
    {
        if (dividend[i] == '1')
        {
            for (int j = 0; j < g_len; j++)
            {
                if (dividend[i + j] == generator[j])
                    dividend[i + j] = '0';
                else
                    dividend[i + j] = '1';
            }
        }
    }
    for (int i= 0; i < f_len; i++){
        if(dividend[i] == '1')
        {
            free(dividend);
            return (1);
        }
    }
    free(dividend);
   return (0);
}

char *int_to_binaryChar(int decimal)
{
    char *binary = (char *)malloc(sizeof(char) * 9); // 8비트 + NULL 문자('\0')
    int i;
    for (i = 0; i < 8; i++)
    {
        *(binary + i) = ((decimal >> (7 - i)) & 1) + '0';
    }
    *(binary + i) = '\0'; // 문자열 끝에 NULL 문자 삽입
    return binary;
}

unsigned char binaryChar_to_int(char *binary)
{
    int bin = 1;
    unsigned char result = 0;
    for (int i = 7; i >= 0; i--)
    {
        if(binary[i] == '1') result += bin;
        bin *= 2;
    }
    return (result);
}

void print_to_file(char *decoded, FILE *fout)
{
    char copy[9];
	while (*decoded) {
		unsigned char num = 0;
		strncpy(copy, decoded, 8);
		copy[8] = '\0';
        num = binaryChar_to_int(copy);
		decoded += 8;
		fwrite(&num, sizeof(char), 1, fout);
	}
}

int main(int argc, char *argv[])
{
    //Initializing
    if (argc != 6)
    {
        fprintf(stderr, "usage: ./crc_decoder input_file output_file result_file generator dataword_size\n");
        exit(1);
    }

    FILE *fin = fopen(argv[1], "rb");
    if (fin == NULL)
    {
        fprintf(stderr, "input file open error.\n");
        exit(1);
    }

    FILE *fout = fopen(argv[2], "wb");
    if (fout == NULL)
    {
        fprintf(stderr, "output file open error.\n");
        exit(1);
    }

    FILE *fres = fopen(argv[3], "w");
    if (fres == NULL)
    {
        fprintf(stderr, "result file open error.\n");
        exit(1);
    }

    int dataword_size = atoi(argv[5]);
    if (dataword_size != 4 && dataword_size != 8)
    {
        fprintf(stderr, "dataword size must be 4 or 8.\n");
        exit(1);
    }

    int count = 0;
    int pad_n = 0;
    unsigned char buf;
    char *generator = (char*)calloc(strlen(argv[4]) + 1, 1);
    strcpy(generator, argv[4]);
    char *input_binary = (char*)malloc(MAX);
    input_binary[0] = '\0';
    int input_n = 0;
    while (fread(&buf, 1, 1, fin) != 0)
    { // 8bit씩 읽기
        if (count == 0)
            pad_n = buf;
        else 
        {
            char *buf_binary = int_to_binaryChar((int)buf);
            strcat(input_binary, buf_binary);
        }
        count++;
    }
    input_binary += pad_n;
    int frame_size = dataword_size + strlen(generator) - 1;
    char *frame = (char*)calloc(frame_size + 1, 1);
    count = 0;
    int error = 0;
    char *decoded = (char*)calloc(MAX, 1);
    for (int i = 0; input_binary[i]; i += frame_size)
    {
        strncpy(frame, input_binary + i, frame_size);
        frame[frame_size] = '\0';
        if(cal_remainder(frame, generator))
            error++;
        count++;
        strncat(decoded, frame, dataword_size);
    }
    print_to_file(decoded, fout);
    fprintf(fres,"%d %d\n", count, error);
    return (0);
}