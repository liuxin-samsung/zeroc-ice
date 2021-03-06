// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef ICE_DB_API_EXPORTS
#   define ICE_DB_API_EXPORTS
#endif

#include <IceDB/IceDB.h>

using namespace IceDB;
using namespace std;

string
DatabaseException::ice_name() const
{
    return "IceDB::DatabaseException";
}

string
DeadlockException::ice_name() const
{
    return "IceDB::DeadlockException";
}

string
NotFoundException::ice_name() const
{
    return "IceDB::NotFoundException";
}

TransactionHolder::TransactionHolder(const DatabaseConnectionPtr& connection) : _connection(connection)
{
    _connection->beginTransaction();
}

TransactionHolder::~TransactionHolder()
{
    try
    {
        rollback();
    }
    catch(...)
    {
        //
        // Ignored to avoid crash during stack unwinding
        //
    }
}

void
TransactionHolder::commit()
{
    if(_connection != 0)
    {
        try
        {
            _connection->commitTransaction();
            _connection = 0;
        }
        catch(...)
        {
            _connection = 0;
            throw;
        }
    }
}

void
TransactionHolder::rollback()
{
    if(_connection != 0)
    {
        try
        {
            _connection->rollbackTransaction();
            _connection = 0;
        }
        catch(...)
        {
            _connection = 0;
            throw;
        }
    }
}


