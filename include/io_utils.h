/*
 * =====================================================================================
 *
 *       Filename:  io_utils.h
 *
 *    Description:  Header for io_utils.c (see that source file for a description). 
 *
 *        Version:  1.0
 *        Created:  07/26/2022 11:41:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  NJKR
 *   Organization:  Arm Research
 *
 * =====================================================================================
 */
#ifndef IO_UTILS_H
#define IO_UTILS_H 

#include <stdlib.h> 

bool update_private_key(unsigned char* private_key, size_t private_key_len, void* target); 
bool read_private_key(unsigned char *private_key, size_t len_private_key, void *filename); 
void* read_file(const char* filename, size_t *len); 

#endif 
