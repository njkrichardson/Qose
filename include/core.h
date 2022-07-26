/*
 * =====================================================================================
 *
 *       Filename:  core.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/26/2022 14:25:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CORE_H
#define CORE_H 

///const char* default_parameters = "20/8,10/8"; 
int keygen(const char *keyname, const char *parm_set); 
int sign(const char *keyname, char **files); 
int verify(const char *keyname, char **files); 

#endif 
