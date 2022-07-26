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

// --- parameter set and default configuration
const char* parameters = "20/8,10/8"; 


// -- load the private key and sign files

// -- given the public key, verify the generated signatures 

// -- advance the private key 

int main()
{
    // --- generate a public/private keypair 
    const char* key_name = "default"; 
    const char* parmname = key_name; 
    int result = keygen(key_name, parmname); 
    return 0; 
}
