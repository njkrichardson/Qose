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
