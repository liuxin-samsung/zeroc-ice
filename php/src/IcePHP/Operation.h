// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICEPHP_OPERATION_H
#define ICEPHP_OPERATION_H

#include <Config.h>

//
// Global functions.
//
extern "C"
{
ZEND_FUNCTION(IcePHP_defineOperation);
}

#define ICEPHP_OPERATION_FUNCTIONS \
    ZEND_FE(IcePHP_defineOperation,  NULL)

namespace IcePHP
{

class Operation : public IceUtil::Shared
{
public:

    virtual zend_function* function() = 0;

};
typedef IceUtil::Handle<Operation> OperationPtr;

}

#endif
