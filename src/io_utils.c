/*
 * =====================================================================================
 *
 *       Filename:  io_utils.c
 *
 *    Description:  Utilites for reading and writing files containing application-specific 
 *    data like cryptographic keys and signatures. 
 *
 *        Version:  1.0
 *        Created:  07/26/2022 11:41:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  NJKR
 *   Organization:  Arm Research 
 *     References:  https://github.com/cisco/hash-sigs
 *
 * =====================================================================================
 */
#include <ctype.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 

#include "hss_zeroize.h" 
#include "hss.h" 
#include "hss_verify_inc.h" 
#include "hss_sign_inc.h" 
#include "io_utils.h" 

// --- utiliity to save private key 
bool update_private_key(unsigned char* private_key, size_t private_key_len, void* target)
{
    FILE* f = fopen(target, "r+"); 
    if (!f) 
    {
        // --- open failed
        f = fopen(target, "w"); 
        if (!f) return false; 
    }
    if (1 != fwrite(private_key, private_key_len, 1, f))
    {
        // --- write failed 
        fclose(f); 
        return false; 
    }
    if (0 != fclose(f))
    {
        return false; 
    }
    return true; 
}

bool read_private_key( unsigned char *private_key, size_t len_private_key, void *filename) 
{
    FILE *f = fopen(filename, "r");
    if (!f) 
    {
        return false;
    }
    if (1 != fread( private_key, len_private_key, 1, f )) 
    {
        fclose(f);
        return false;
    }
    fclose(f);

    // --- success
    return true;
}

void* read_file(const char* filename, size_t *len)
{
    FILE* f = fopen(filename, "r"); 
    if (!f) return 0; 
#define FILE_INCREMENT 20000

    unsigned allocation_length = FILE_INCREMENT; 
    unsigned char* p = malloc(allocation_length); 
    if (!p) return 0; 

    unsigned current_length = 0; 
    for (;;) 
    {
        unsigned delta = allocation_length - current_length; 
        if (delta == 0) 
        {
            unsigned char* q = realloc(p, allocation_length + FILE_INCREMENT); 
            if (!q)
            {
                free(p); 
                return 0; 
            }
            p = q; 
            allocation_length += FILE_INCREMENT; 
            delta = FILE_INCREMENT; 
        }
        int n = fread(p + current_length, 1, delta, f); 
        if (n <= 0) break; 
        current_length += n; 
    }
    if (len) * len = current_length; 
    return p; 
}
