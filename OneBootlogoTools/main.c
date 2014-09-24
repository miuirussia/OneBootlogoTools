//
//  main.c
//  OnePlusOneBootConverter
//
//  Created by Matteo Gaggiano on 23/09/14.
//  Copyright (c) 2014 Matteo Gaggiano. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#if DEBUG
#define DLOG(fmt, ...) printf("Debug: " fmt "\n", ##__VA_ARGS__);
#else
#define DLOG(fmt, ...) do{} while(0)
#endif

void usage();

typedef unsigned short bool;
#define FALSE 0
#define TRUE  1

unsigned long FILE_LEN = 9821696;
unsigned long LOGO_LEN = 6220800;
unsigned long FAST_LEN = 315000;

uint8_t headerLogo[] = {
    0x53, 0x50, 0x4C, 0x41, 0x53, 0x48, 0x21, 0x21,
    0x38, 0x04, 0x00, 0x00, 0x7F, 0x07, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x6E, 0x00,
    0x00, 0xE2, 0x5E, 0x00, 0x00, 0x20, 0x93, 0x00,
    0x00, 0x52, 0x94, 0x00, 0x00, 0x5E, 0x60, 0x00,
    0x00, 0x68, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t header2[] = {};

uint8_t header3[] = {};

uint8_t headerFast[] = {
    0x53, 0x50, 0x4C, 0x41, 0x53, 0x48, 0x21, 0x21,
    0x5E, 0x01, 0x00, 0x00, 0x39, 0x01, 0x00, 0x00 };

uint8_t header5[] = {};

uint8_t header6[] = {};

int main(int argc, const char * argv[]) {
    
    long int logosize = 0;
    
    long int fastsize = 0;
    
	long unsigned int = 0;
    FILE *logoF = NULL;
    FILE *fastF = NULL;
    FILE *fileF = NULL;
    
    bool flashBoot = FALSE;
    bool flashFast = FALSE;
    
    uint8_t *logoBin = malloc(sizeof(uint8_t) * LOGO_LEN);
    
    uint8_t *fastBin = malloc(sizeof(uint8_t) * FAST_LEN);
    
    uint8_t *fileBin = malloc(sizeof(uint8_t) * FILE_LEN);
    
    if (argc == 2) {
        if (!strcmp(argv[1], "-b")) {
            flashBoot = TRUE;
        } else if (!strcmp(argv[1], "-f")) {
            flashFast = TRUE;
        }
    } else if (argc == 3) {
        if (!strcmp(argv[1], "-b")) {
            flashBoot = TRUE;
        } else if (!strcmp(argv[1], "-f")) {
            flashFast = TRUE;
        }
        if (!strcmp(argv[2], "-b")) {
            flashBoot = TRUE;
        } else if (!strcmp(argv[2], "-f")) {
            flashFast = TRUE;
        }
    } else {
        printf("Too many arguments!\n");
        usage();
        return 255;
    }
    
    
    
    logoF = fopen("bootlogo.raw", "r");
    if (!logoF && flashBoot) {
        perror("Error while read bootlogo.raw");
        return errno;
    }
        flashBoot = TRUE;
    
    fastF = fopen("fastboot.raw", "r");
    if (!fastF && flashFast) {
        perror("Error while read fastboot.raw");
        return errno;
    }
    
    fileF = fopen("logo.bin", "r");
    if (!fileF) {
        perror("Error while read logo.bin");
        return errno;
    }
    
    if (flashBoot) {
        fseek(logoF, 0L, SEEK_END);
        logosize = ftell(logoF);
        
        fseek(logoF, 0L, SEEK_SET);
    }
    
    if (flashFast) {
        fseek(fastF, 0L, SEEK_END);
        fastsize = ftell(fastF);
        
        fseek(fastF, 0L, SEEK_SET);
    }
    
    for (long int i = 0; i < FILE_LEN; i++) {
        fileBin[i] = fgetc(fileF);
    }
    
    fclose(fileF);

    for (long int i = 0; i < LOGO_LEN && flashBoot; i++) {
        logoBin[i] = fgetc(logoF);
    }
    
    for (long int i = 0; i < FAST_LEN && flashFast; i++) {
        fastBin[i] = fgetc(fastF);
    }
    
    for (long long int i = 0; i < FILE_LEN; i++)
    {
        if (i >= 0 && i < 48 && flashBoot) {
            fileBin[i] = headerLogo[i]; // main logo header
        } else
        
        if (i >= 48 && i < 512 && flashBoot) { // main logo header
            fileBin[i] = 0x00;
        } else
        
        if (i >= 512 && i < 6221312 && flashBoot)
        {
            fileBin[i] = logoBin[i - 512]; // bootlogo image
        } else
        
        if (i >= 7234048 && i < 7234064 && flashFast) {
            fileBin[i] = headerFast[i - 7234048]; // fastboot logo header
        } else
        
        if (i >= 7234560 && i < 7549560 && flashFast)
        {
            fileBin[i] = fastBin[i - 7234560]; // fastboot logo image
        }
    }
    
    fileF = fopen("logo-modified.bin", "w");
    if (!fileF) {
        perror("Error writing file logo-modified.bin");
        return errno;
    }
    
    for (long int i = 0; i < FILE_LEN; i++) {
        fwrite(&fileBin[i], sizeof(uint8_t), 1, fileF);
    }
    
    printf("All ok. logo-modified.bin created correctly! \n");
    
    free(fileBin);
    free(logoBin);
    free(fastBin);
    
    return 0;
}

void usage() {
    printf("OneBootlogoTools:\n\t-b bootlogo.raw \t Overwrite the bootlogo\n\t-f fastboot.raw \t Overwrite the fastboot image\n\nVersion: 1.0b1 by Matteo Gaggiano\n");
}
