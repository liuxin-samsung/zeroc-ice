// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef THROUGHPUT_ICE
#define THROUGHPUT_ICE

module Demo
{

sequence<byte> ByteSeq;
const int ByteSeqSize = 500000;

sequence<string> StringSeq;
const int StringSeqSize = 50000;

struct StringDouble
{
    string s;
    double d;
};
sequence<StringDouble> StringDoubleSeq;
const int StringDoubleSeqSize = 50000;

struct Fixed
{
    int i;
    int j;
    double d;
};
sequence<Fixed> FixedSeq;
const int FixedSeqSize = 50000;

interface Throughput
{
    ["amd", "cpp:array"] ByteSeq echoByteSeq(["cpp:array"] ByteSeq seq);

    StringSeq echoStringSeq(StringSeq seq);

    StringDoubleSeq echoStructSeq(StringDoubleSeq seq);

    FixedSeq echoFixedSeq(FixedSeq seq);

    void shutdown();
};

};

#endif
