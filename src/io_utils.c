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

#include "hss_sign_inc.h" 

// --- utiliity to save private key 
static bool update_private_key(unsigned char* private_key, size_t private_key_len, void* target)
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
