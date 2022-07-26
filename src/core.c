/* =====================================================================================
 *
 *       Filename:  core.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/26/2022 14:23:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  NJKR
 *   Organization:  Arm Research
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "core.h"
#include "hash.h" 
#include "hss_zeroize.h" 
#include "hss.h" 
#include "hss_verify_inc.h" 
#include "hss_sign_inc.h" 
#include "io_utils.h"
#include "utils.h"

// --- key generation 
int keygen(const char *keyname, const char *parm_set) {
    /* Parse the parameter set */
    int levels;
    param_set_t lm_array[ MAX_HSS_LEVELS ];
    param_set_t ots_array[ MAX_HSS_LEVELS ];
    size_t aux_size;
    if (!parm_set) parm_set = "20/8,10/8";
    if (!parse_parameter_set( &levels, lm_array, ots_array, &aux_size, parm_set)) {
        return 0;
    }

    /* Tell the user how we interpreted the parameter set he gave us */
    list_parameter_set( levels, lm_array, ots_array, aux_size );

    /* We'll place the private key here */
    size_t private_key_filename_len = strlen(keyname) + sizeof (".prv" ) + 1;
    char *private_key_filename = malloc(private_key_filename_len);
    if (!private_key_filename) return 0;
    sprintf( private_key_filename, "%s.prv", keyname );

    /* We'll place the public key in this array */
    unsigned len_public_key = hss_get_public_key_len(levels,
                                                lm_array, ots_array);
    if (len_public_key == 0) { free(private_key_filename); return 0; }
    unsigned char public_key[HSS_MAX_PUBLIC_KEY_LEN];

    /* And we'll place the aux data in this array */
    unsigned aux_len;
    if (aux_size > 0) {
        aux_len = hss_get_aux_data_len( aux_size, levels,
                                               lm_array, ots_array);
        printf( "aux_len = %d\n", aux_len );
    } else {
        aux_len = 1;
    }
    unsigned char *aux = malloc(aux_len);
    if (!aux) {
        printf( "error mallocing aux; not generating aux\n" );
        aux_len = 0;
        aux = 0;
    }

    printf( "Generating private key %s (will take a while)\n",
                                       private_key_filename );
    if (!hss_generate_private_key(
        do_rand,       /* Routine to generate randomness */
        levels,        /* # of Merkle levels */
        lm_array, ots_array,  /* The LM and OTS parameters */
        update_private_key, private_key_filename, /* Routine to write out */
                                       /* the genearted private key */
        public_key, len_public_key,  /* The public key is placed here */
        aux_size > 0 ? aux : 0, aux_len, /* Where to place the aux data */
        0)) {            /* Use the defaults for extra info */
            free(private_key_filename);
            free(aux);
            return 0;
    }
    free(private_key_filename); private_key_filename = 0;

    size_t public_key_filename_len = strlen(keyname) + sizeof (".pub" ) + 1;
    char *public_key_filename = malloc(public_key_filename_len);
    if (!public_key_filename) {
        free(aux);
        return 0;
    }
    sprintf( public_key_filename, "%s.pub", keyname );

    printf( "Success!\nWriting public key %s\n", public_key_filename );
    FILE *f = fopen( public_key_filename, "w" );
    free(public_key_filename ); public_key_filename = 0;
    if (!f) {
        fprintf( stderr, "Error: unable to write public key\n" );
        free(aux);
        return 0;
    }
    if (1 != fwrite( public_key, len_public_key, 1, f )) {
        /* Write failed */
        fclose(f);
        free(aux);
        return 0;
    }
    if (0 != fclose(f)) {
        fprintf( stderr, "Error: unable to close public key file\n" );
        /* Close failed (possibly because pending write failed) */
        free(aux);
        return 0;
    }

    /* If the key was specified manually, put in our warning */
    if (0) {
        fprintf( stderr,
               "*** Warning: the key was not generated manually\n"
               "    This key should not be used for real security\n" );
    }

    if (aux_size > 0) {
        size_t aux_filename_len = strlen(keyname) + sizeof (".aux" ) + 1;
        char *aux_filename = malloc(aux_filename_len);
        if (!aux_filename) {
            fprintf( stderr, "Warning: malloc failure writing to aux file\n" );
            free(aux);
            return 1;
        }
        sprintf( aux_filename, "%s.aux", keyname );

        /* Attempt to write the aux file.  Note that if we fail, we'll still */
        /* claim to have succeeded (as the aux file is optional) */
        printf( "Writing aux data %s\n", aux_filename );
        f = fopen( aux_filename, "w" );
        free(aux_filename); aux_filename = 0;
        if (!f) {
            fprintf( stderr, "Warning: unable to write aux file\n" );
            free(aux);
            return 1;
        }
        if (1 != fwrite( aux, aux_len, 1, f )) {
            fprintf( stderr, "Warning: unable to write aux file\n" );
            /* Write failed */
            fclose(f);
            free(aux);
            return 1;
        }
        if (0 != fclose(f)) {
            fprintf( stderr, "Warning: close failed writing aux file\n" );
            /* Close failed (possibly because pending write failed) */
            free(aux);
            return 1;
        }
    }
    free(aux);

    return 1;
}

int sign(const char *keyname, char **files) {
    int private_key_filename_len = strlen(keyname) + sizeof (".prv" ) + 1;
    char *private_key_filename = malloc(private_key_filename_len);
    if (!private_key_filename) {
        printf( "Malloc failure\n" );
        return 0;
    }
    sprintf( private_key_filename, "%s.prv", keyname );

        /* Read in the auxilliary file */   
    size_t aux_filename_len = strlen(keyname) + sizeof (".aux" ) + 1;
    char *aux_filename = malloc(aux_filename_len);
    if (!aux_filename) {
        printf( "Malloc failure\n" );
        free(private_key_filename);
        return 0;
    }
    sprintf( aux_filename, "%s.aux", keyname );
    size_t len_aux_data = 0;
    void *aux_data = read_file( aux_filename, &len_aux_data );
    if (aux_data != 0) {
        printf( "Processing with aux data\n" );
    } else {
        /* We don't have the aux data; proceed without it */
        printf( "Processing without aux data\n" );
    }

        /* Load the working key into memory */
    printf( "Loading private key\n" );
    fflush(stdout);
    struct hss_working_key *w = hss_load_private_key(
             read_private_key, private_key_filename, /* How to load the */
                                         /* private key */
             0,                          /* Use minimal memory */
             aux_data, len_aux_data,     /* The auxiliary data */
             0);                         /* Use the defaults for extra info */
    if (!w) {
        printf( "Error loading private key\n" );
        free(aux_data);
        hss_free_working_key(w);
        free(aux_filename);
        free(private_key_filename);
        return 0;
    }
    free(aux_data);

    printf( "Loaded private key\n" );  /* printf here mostly so the user */
    fflush(stdout);              /* gets a feel for how long this step took */
                                 /* compared to the signing steps below */

    /* Now, go through the file list, and generate the signatures for each */

    /* Look up the signature length */
    size_t sig_len;
    sig_len = hss_get_signature_len_from_working_key(w);
    if (sig_len == 0) {
        printf( "Error getting signature len\n" );
        hss_free_working_key(w);
        free(aux_filename);
        free(private_key_filename);
        return 0;
    }

    unsigned char *sig = malloc(sig_len);
    if (!sig) {
        printf( "Error during malloc\n" );
        hss_free_working_key(w);
        free(aux_filename);
        free(private_key_filename);
        return 0;
    }
    int i;
    for (i=0; files[i]; i++) {
        printf( "Signing %s\n", files[i] );

        /*
         * Read the file in, and generate the signature.  We don't want to
         * assume that we can fit the entire file into memory, and so we
         * read it in in pieces, and use the API that allows us to sign
         * the message when given in pieces
         */
        FILE *f = fopen( files[i], "r" );
        if (!f) {
            printf( "    %s: unable to read\n", files[i] );
            continue;
        }

        struct hss_sign_inc ctx;
        (void)hss_sign_init(
             &ctx,                 /* Incremental signing context */
             w,                    /* Working key */
             update_private_key,    /* Routine to update the */
             private_key_filename, /* private key */
             sig, sig_len,         /* Where to place the signature */
             0);                   /* Use the defaults for extra info */

        char buffer[1024];
        for (;;) {
            int n = fread( buffer, 1, sizeof buffer, f );
            if (n <= 0) break;
            (void)hss_sign_update(
                &ctx,           /* Incremental signing context */
                buffer,         /* Next piece of the message */
                n);             /* Length of this piece */
        }
        fclose(f);

        bool status = hss_sign_finalize(
             &ctx,               /* Incremental signing context */
             w,                  /* Working key */
             sig,                /* Signature */
             0);                 /* Use the defaults for extra info */

        if (!status) {
            printf( "    Unable to generate signature\n" );
            continue;
        }

        size_t sig_file_name_len = strlen(files[i]) + sizeof( ".sig" ) + 1;
        char *sig_file_name = malloc( sig_file_name_len );
        if (!sig_file_name) {
            printf( "    Malloc failure\n" );
            continue;
        }
        sprintf( sig_file_name, "%s.sig", files[i] );
        f = fopen( sig_file_name, "w" );
        if (!f) {
            printf( "    %s: unable to create\n", sig_file_name );
            free(sig_file_name);
            continue;
        }
        if (1 != fwrite( sig, sig_len, 1, f )) {
            printf( "    %s: unable to write\n", sig_file_name );
            fclose(f);
            free(sig_file_name);
            continue;
        }
        fclose(f);
        printf( "    signed (%s)\n", sig_file_name );
        free(sig_file_name);
    }

    hss_free_working_key(w);
    free(aux_filename);
    free(private_key_filename);
    free(sig);
    return 1;
}

int verify(const char *keyname, char **files) {
    /* Step 1: read in the public key */
    size_t public_key_filename_len = strlen(keyname) + sizeof ".pub" + 1;
    char *public_key_filename = malloc(public_key_filename_len);
    if (!public_key_filename) {
         printf( "Error: malloc failure\n" );
         return 0;
    }
    sprintf( public_key_filename, "%s.pub", keyname );
    unsigned char *pub = read_file( public_key_filename, 0 );
    if (!pub) {
         printf( "Error: unable to read %s.pub\n", keyname );
         free(public_key_filename);
         return 0;
    }
    int i;
    for (i=0; files[i]; i++) {
        printf( "Verifying %s\n", files[i] );

            /* Read in the signatre */
        size_t sig_file_name_len = strlen(files[i]) + sizeof( ".sig" ) + 1;
        char *sig_file_name = malloc(sig_file_name_len);
        if (!sig_file_name) {
            printf( "Error: malloc failure\n" );
            free(public_key_filename);
            return 0;
        }
        sprintf( sig_file_name, "%s.sig", files[i] );
        size_t sig_len;
        void *sig = read_file( sig_file_name, &sig_len );
        free(sig_file_name ); sig_file_name = 0;
        if (!sig) {
            printf( "    %s: unable to read signature file %s.sig\n", files[i], files[i] );
            continue;
        }

        /*
         * Read the file in, and verify the signature.  We don't want to
         * assume that we can fit the entire file into memory, and so we
         * read it in in pieces, and use the API that allows us to verify
         * the message when given in pieces
         */
        FILE *f = fopen( files[i], "r" );
        if (!f) {
            printf( "    %s: unable to read\n", files[i] );
            free(sig);
            continue;
        }

        struct hss_validate_inc ctx;
        (void)hss_validate_signature_init(
             &ctx,               /* Incremental validate context */
             pub,                /* Public key */
             sig, sig_len,       /* Signature */
             0);                 /* Use the defaults for extra info */

        char buffer[1024];
        for (;;) {
            int n = fread( buffer, 1, sizeof buffer, f );
            if (n <= 0) break;
            (void)hss_validate_signature_update(
                &ctx,           /* Incremental validate context */
                buffer,         /* Next piece of the message */
                n);             /* Length of this piece */
        }
        fclose(f);

        bool status = hss_validate_signature_finalize(
             &ctx,               /* Incremental validate context */
             sig,                /* Signature */
             0);                 /* Use the defaults for extra info */

        free(sig);

        if (status) {
            printf( "    Signature verified\n" );
        } else {
            printf( "    Signature NOT verified\n" );
        }
    }
    free(public_key_filename);
    return 1;
}
