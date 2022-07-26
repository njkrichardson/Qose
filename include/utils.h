/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description: : 
 *
 *        Version:  1.0
 *        Created:  07/26/2022 11:56:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  NJKR
 *   Organization:  Arm Research
 *
 * =====================================================================================
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h> 

static const char* seed_bits = 0; 
static bool convert_specified_seed_i_value(void*, size_t); 

bool do_rand(void* output, size_t len); 

#endif 
