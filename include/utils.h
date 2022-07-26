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
#include "hss_sign_inc.h" 

//const char* seed_bits = 0; 
//const char* i_value = 0; 
bool convert_specified_seed_i_value(void*, size_t); 
int get_integer(const char **p); 
int parse_parameter_set(int* levels, param_set_t* lm_array, param_set_t* ots_array, size_t* aux_size, const char* param_set); 
void list_parameter_set(int levels, const param_set_t* lm_array, const param_set_t* ots_array, size_t aux_size); 

bool do_rand(void* output, size_t len); 

#endif 
