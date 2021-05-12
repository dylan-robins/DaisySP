#pragma once
#ifndef DSY_PLATFORM_H
#define DSY_PLATFORM_H

/******************************************************************************
This file defines miscellaneous macros for debugging, assertions, etc.
******************************************************************************/

// Define macro D_SP_ASSERTFALSE which triggers an assertion failure using the
// appropriate method for the current platform
#if defined(__arm__)
// On embedded platforms use the bkpt opcode
#define D_SP_ASSERTFALSE(expr) asm("bkpt 255")
#elif defined(JUCE_VERSION)
// In a JUCE project use the JUCE macros
#define D_SP_ASSERTFALSE(expr) jassertfalse
#else
// Otherwise, raise a default assertion failure
#include <assert.h>
#define D_SP_ASSERTFALSE(expr) assert(expr)
#endif

// If expression is false, trigger an assertion failure using the appropriate
// method for the current platform
#define D_SP_ASSERT(expression)            \
    do {                                   \
        if (!expression)                   \
            D_SP_ASSERTFALSE(#expression); \
    } while (0)

#endif // DSY_PLATFORM_H
