/*
 * =====================================================================================
 *
 *       Filename:  arm_demo.c
 *
 *    Description:  A simple signature/verification utility demonstrating 
 *    usage of the hash-sigs library. Based on the hash-sigs provided demo.c 
 *    source. 
 *
 *        Version:  1.0
 *        Created:  07/26/2022 10:59:41
 *       Revision:  0.0
 *       Compiler:  gcc
 *
 *         Author: NJKR
 *   Organization: Arm Research
 *     References: https://github.com/cisco/hash-sigs
 *
 * =====================================================================================
 */
#include <ctype.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <time.h> 

#include "core.h"
#include "hash.h" 
#include "hss_zeroize.h" 
#include "hss.h" 
#include "hss_verify_inc.h" 
#include "hss_sign_inc.h" 
#include "io_utils.h"
#include "utils.h"

static void usage(char *program) {
    printf( "Usage:\n" );
    printf( " %s genkey [keyname]\n", program );
    printf( " %s genkey [keyname] [parameter set]\n", program );
    printf( " %s sign [keyname] [files to sign]\n", program );
    printf( " %s verify [keyname] [files to verify]\n", program );
    printf( " %s advance [keyname] [amount of advance]\n", program );
}

int main(int argc, char** argv)
{
    if (argc < 2) 
    {
        usage(argv[0]);
        return 0;
    }
    // --- parameter set and default configuration
    const char* parameters = "20/8,10/8"; 
    const char* key_name = "default"; 

    // --- generate a public/private keypair 
    if (0 == strcmp( argv[1], "genkey" ))
    {
        if (!keygen(key_name, parameters))
        {
            printf("Error generating key\n."); 
        }
    }

    // --- load the private key and sign files
    if (0 == strcmp(argv[1], "sign")) 
    {
        if (argc < 4) {
            printf( "Error: missing keyname and file argument\n" );
            usage(argv[0]);
            return 0;
        }
        if (!sign( argv[2], &argv[3] )) {
            printf( "Error signing\n" );
        }
        return 0;
    }

    // -- given the public key, verify the generated signatures 
    if (0 == strcmp( argv[1], "verify" )) 
    {
        if (argc < 4) 
        {
            printf( "Error: mssing keyname and file argument\n" );
            usage(argv[0]);
            return 0;
        }
        if (!verify( argv[2], &argv[3] )) 
        {
            printf( "Error verifying\n" );
        }
        return 0;
    }

    usage(argv[0]);
    
    return 1; 
}
