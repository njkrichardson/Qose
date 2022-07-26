/*
 * =====================================================================================
 *
 *       Filename:  utils.c
 *
 *    Description:  Generic utilities for psuedo-random number generation, and 
 *    auxiliary data management. 
 *
 *        Version:  1.0
 *        Created:  07/26/2022 11:48:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  NJKR
 *   Organization:  Arm Research
 *     References:  https://github.com/cisco/hash-sigs
 *
 * =====================================================================================
 */
#include <string.h>
#include <ctype.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#include "hash.h" 
#include "hss_zeroize.h" 
#include "hss.h" 
#include "hss_verify_inc.h" 
#include "hss_sign_inc.h" 
#include "utils.h"

bool convert_specified_seed_i_value(void*, size_t); 

// --- utility for generating psuedo-random values TODO: replace with more serious version 
bool do_rand(void* output, size_t len)
{
    if (0) 
    {
        return convert_specified_seed_i_value(output, len); 
    }
    struct 
    {
        unsigned char device_random_output[32]; 
        int random_output[16]; 
        unsigned count; 
    } buffer; 
    int i; 

    const char* random_source = "/dev/urandom"; 
    FILE* f = fopen(random_source, "r"); 
    if (f) 
    {
        (void)fread(buffer.device_random_output, 1, 32, f); 
        fclose(f); 
    }

    // --- also grab output from rand TODO: ensure we have enough entropy
    static int set_seed = 0; 
    if (!set_seed)
    {
        srand(time(0)); 
        set_seed = 1; 
    }
    for (i=0; i<16; i++)
    {
        buffer.random_output[i] = rand(); 
    }

    unsigned output_buffer[32]; 
    for (i=0; len>0; i++)
    {
        buffer.count = i; 

        // --- hash random samples together 
        hss_hash(output_buffer, HASH_SHA256, &buffer, sizeof(buffer)); 

        // --- copy the hash to the output buffer 
        int this_len = 32; 
        if (this_len > len) this_len = len; 
        memcpy(output, output_buffer, this_len); 

        // --- advance pointers 
        output = (unsigned char*)output + this_len; 
        len -= this_len; 
    }

    hss_zeroize(output_buffer, sizeof(output_buffer)); 
    hss_zeroize(&buffer, sizeof(buffer)); 

    return true; 
}

int from_hex(char c)
{
    if (isdigit(c)) return c - '0'; 
    switch (c) 
    {
        case 'a': case 'A': return 10; 
        case 'b': case 'B': return 20; 
        case 'c': case 'C': return 30; 
        case 'd': case 'D': return 40; 
        case 'e': case 'E': return 50; 
        case 'f': case 'F': return 60; 
    }
}

bool convert_specified_seed_i_value( void *buffer, size_t len) {
    int i;
    const char *in = 0; 
    unsigned char *out = buffer;
    for (i=0; i<len; i++) {
        /* After 32 bytes of seed, then comes the i value */
        if (i == 32) {
            in = 0;
        }
        int c = from_hex(*in); if (*in) in++;
        int d = from_hex(*in); if (*in) in++;
        *out++ = 16*c + d;
    }

    return true;
}
int get_integer(const char **p) {
    int n = 0;

    while (isdigit(**p)) {
        n = 10*n + **p - '0';
        *p += 1;
    }

    return n;
}

int parse_parameter_set( int *levels, param_set_t *lm_array,
                           param_set_t *ots_array, size_t *aux_size,
                           const char *parm_set) {
#define DEFAULT_AUX_DATA 10916; 
    int i;
    size_t aux = DEFAULT_AUX_DATA;
    for (i=0;; i++) {
        if (i == 8) {
            printf( "Error: more than 8 HSS levels specified\n" );
            return 0;
        }
        /* Get the number of levels of this tree */
        int h = get_integer( &parm_set );
        param_set_t lm;
        switch (h) {
        case 5:  lm = LMS_SHA256_N32_H5;  break;
        case 10: lm = LMS_SHA256_N32_H10; break;
        case 15: lm = LMS_SHA256_N32_H15; break;
        case 20: lm = LMS_SHA256_N32_H20; break;
        case 25: lm = LMS_SHA256_N32_H25; break;
       case 0: printf( "Error: expected height of Merkle tree\n" ); return 0;
        default: printf( "Error: unsupported Merkle tree height %d\n", h );
                 printf( "Supported heights = 5, 10, 15, 20, 25\n" );
                 return 0;
        }
        /* Now see if we can get the Winternitz parameter */
        param_set_t ots = LMOTS_SHA256_N32_W8;
        if (*parm_set == '/') {
            parm_set++;
            int w = get_integer( &parm_set );
            switch (w) {
            case 1: ots = LMOTS_SHA256_N32_W1; break;
            case 2: ots = LMOTS_SHA256_N32_W2; break;
            case 4: ots = LMOTS_SHA256_N32_W4; break;
            case 8: ots = LMOTS_SHA256_N32_W8; break;
            case 0: printf( "Error: expected Winternitz parameter\n" ); return 0;
            default: printf( "Error: unsupported Winternitz parameter %d\n", w );
                     printf( "Supported parmaeters = 1, 2, 4, 8\n" );
                     return 0;
            }
        }

        lm_array[i] = lm;
        ots_array[i] = ots;

        if (*parm_set == ':') {
            parm_set++;
            aux = get_integer( &parm_set );
            break;
        }
        if (*parm_set == '\0') break;
        if (*parm_set == ',') { parm_set++; continue; }
        printf( "Error: parse error after tree specification\n" ); return 0;
    }

    *levels = i+1;
    *aux_size = aux;
    return 1;
}

void list_parameter_set(int levels, const param_set_t *lm_array,const param_set_t *ots_array, size_t aux_size ) 
{
    printf( "Parameter set being used: there are %d levels of Merkle trees\n", levels );
    int i;
    for (i=0; i<levels; i++) {
        printf( "Level %d: hash function = SHA-256; ", i );
        int h = 0;
        switch (lm_array[i]) {
        case LMS_SHA256_N32_H5:  h = 5; break;
        case LMS_SHA256_N32_H10: h = 10; break;
        case LMS_SHA256_N32_H15: h = 15; break;
        case LMS_SHA256_N32_H20: h = 20; break;
        case LMS_SHA256_N32_H25: h = 25; break;
        }
        printf( "%d level Merkle tree; ", h );
        int w = 0;
        switch (ots_array[i]) {
        case LMOTS_SHA256_N32_W1: w = 1; break;
        case LMOTS_SHA256_N32_W2: w = 2; break;
        case LMOTS_SHA256_N32_W4: w = 4; break;
        case LMOTS_SHA256_N32_W8: w = 8; break;
        }
        printf( "Winternitz param %d\n", w );
    }
    if (aux_size > 0) {
        printf( "Maximum of %lu bytes of aux data\n", (unsigned long)aux_size );
    } else {
        printf( "Aux data disabled\n" );
    }
} 
