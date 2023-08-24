#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define MAX 10 * 10 * 1024 * 1024

char *int_to_binaryChar(int n)
{
    char *binary_str = (char *)malloc(9 * sizeof(char));
    binary_str[8] = '\0';

    for (int i = 7; i >= 0; i--)
    {
        binary_str[i] = (n & 1) + '0';
        n = n >> 1;
    }

    return binary_str;
}

char *cal_remainder(char *dataword, char *generator)
{
    int d_len = strlen(dataword);
    int g_len = strlen(generator);
    int r_len = g_len - 1;
    char *dividend = (char *)malloc(d_len + r_len + 1);
    strcpy(dividend, dataword);
    memset(dividend + d_len, '0', r_len);
    dividend[d_len + r_len] = '\0';

    for (int i = 0; i < d_len; i++)
    {
        if (dividend[i] != '0')
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
    char *remainder = (char*)malloc(r_len + 1);
    strcpy(remainder, dividend + d_len);
    remainder[r_len] = '\0';
    free(dividend);
    return (remainder);
}

char *cal_codeword(char *dataword, char *generator)
{
    char *remainder = cal_remainder(dataword, generator);
    char *codeword = (char *)malloc(strlen(dataword) + strlen(remainder) + 1);
    codeword[0] = '\0';
    strcat(codeword, dataword);
    strcat(codeword, remainder);
    free(remainder);
    return (codeword);
}

void print_to_file(char *code, FILE *fout)
{
    char copy[9];
    while (*code)
    {
        unsigned num = 0;
        strncpy(copy, code, 8);
        copy[8] = '\0';
        code += 8;
        int bin = 1;
        // get pads binary to decimal
        for (int i = 7; i >= 0; i--)
        {
            if (copy[i] == '1')
                num += bin;
            bin *= 2;
        }
        fputc(num, fout);
    }
}

int main(int argc, char *argv[])
{
    // Initializing
    if (argc != 5)
    {
        fprintf(stderr, "usage: ./crc_encoder input_file output_file generator dataword_size\n");
        exit(1);
    }
    // rb : binary 형식으로 읽겠다
    FILE *fin = fopen(argv[1], "rb");
    if (fin == NULL)
    {
        fprintf(stderr, "input file open error.\n");
        exit(1);
    }

    // wb : binary 형식으로 쓰겠다.
    FILE *fout = fopen(argv[2], "wb");
    if (fout == NULL)
    {
        fprintf(stderr, "output file open error.\n");
        exit(1);
    }

    int dataword_size = atoi(argv[4]);
    if (dataword_size != 4 && dataword_size != 8)
    {
        fprintf(stderr, "dataword size must be 4 or 8.\n");
        exit(1);
    }

    char *generator = (char *)malloc(strlen(argv[3]) + 1);
    strcpy(generator, argv[3]);

    char buf;
    char *final_code = (char *)calloc(MAX, 1);
    char *code_word;

    // Main Loop
    while (fread(&buf, 1, 1, fin) != 0)
    { // 8bit씩 읽기
        char *buf_binary = int_to_binaryChar((int)buf);
        if (dataword_size == 8)
            code_word = cal_codeword(buf_binary, generator);
        else
        {
            char *temp_code_word[2];
            for (int i = 0; i < 2; i++)
            {
                char dataword[5];
                // 4bit 빼오기
                strncpy(dataword, buf_binary + i * 4, 4);
                dataword[4] = '\0';
                // codeword 계산
                temp_code_word[i] = cal_codeword(dataword, generator);
            }
            // 4bit씩 계산한 codeword 합치기
            code_word = (char *)malloc(strlen(temp_code_word[1]) * 2 + 1);
            code_word[0] = '\0';
            strcat(code_word, temp_code_word[0]);
            strcat(code_word, temp_code_word[1]);
            free(temp_code_word[0]);
            free(temp_code_word[1]);
        }
        strcat(final_code, code_word);
        free(code_word);
        free(buf_binary);
    }
    int pad_size = 16 - (strlen(final_code) % 16);
    if (pad_size == 16)
        pad_size = 0;
    char *padding = (char *)malloc(pad_size + 1);
    memset(padding, '0', pad_size);
    padding[pad_size] = '\0';
    char *pad_n = int_to_binaryChar(pad_size);
    char *print = (char *)malloc(strlen(pad_n) + strlen(padding) + strlen(final_code) + 1);
    print[0] = '\0';
    strcat(print, pad_n);
    strcat(print, padding);
    strcat(print, final_code);
    print_to_file(print, fout);
    free(pad_n);
    free(padding);
    free(final_code);
    fclose(fin);
    fclose(fout);
}