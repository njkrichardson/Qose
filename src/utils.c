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

static const char* seed_bits = 0; 
static bool convert_specified_seed_i_value(void*, size_t); 

// --- utility for generating psuedo-random values TODO: replace with more serious version 
bool do_rand(void* output, size_t len)
{
    if (seed_bits) 
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

static int from_hex(char c)
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

static int parse_parameter_set(int* levels, param_set_t* lm_array, param_set_t* ots_array, size_t* aux_size, const char* param_set); 
static void list_parameter_set(int levels, const param_set_t* lm_array, const param_set_t* ots_array, size_t aux_size); 
