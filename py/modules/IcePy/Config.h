// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEPY_CONFIG_H
#define ICEPY_CONFIG_H

//
// This file includes <Python.h> and should always be included first,
// see http://www.python.org/doc/api/includes.html for the details.
//

//
// COMPILERFIX: This is required to prevent annoying warnings with aCC.
// The aCC -mt option causes the definition of the _POSIX_C_SOURCE macro
// (with another lower value.) and this is causing a warning because of 
// the redefinition.
//
#if defined(__HP_aCC) && defined(_POSIX_C_SOURCE)
#    undef _POSIX_C_SOURCE
#endif

#include <Python.h>

#ifdef STRCAST
#   error "STRCAST already defined!"
#endif
#define STRCAST(s) const_cast<char*>(s)

//
// Python 2.5 compatibility.
//
#if PY_VERSION_HEX < 0x02050000
    typedef int Py_ssize_t;
#   define ICEPY_OLD_EXCEPTIONS
#endif

#endif
