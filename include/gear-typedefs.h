/**
 * @file gear-types.h
 * @author github.com/TyfoC
 * @brief 
 * @version 1.0
 * @date 4 June 2023
 */

#pragma once
#ifndef GEAR_TYPEDEFS
#define GEAR_TYPEDEFS

#include <stdint.h>
#include <stddef.h>

#define GEAR_VERSION		0x01'04'00			//	1.4.0
#define GEAR_VERSION_STRING	"1.4.0"

#ifndef NPOS
#define NPOS				((long unsigned int)-1)
#endif

#ifndef NULL
#define NULL	0			//	((void*)0)
#endif

#ifndef HIDE
#define HIDE(...)			((void)__VA_ARGS__)
#endif

#ifndef FALSE
#define FALSE				0
#endif

#ifndef TRUE
#define TRUE				1
#endif

typedef unsigned char gbool_t;

#endif