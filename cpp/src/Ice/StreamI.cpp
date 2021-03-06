// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// We disable deprecation warning here, to allow clean compilation of
// of deprecated methods.
//
#ifdef _MSC_VER
#   pragma warning( disable : 4996 )
#endif

#include <Ice/StreamI.h>
#include <Ice/Initialize.h>
#include <Ice/LocalException.h>

using namespace std;
using namespace Ice;
using namespace IceInternal;

//
// InputStreamI
//
Ice::InputStreamI::InputStreamI(const Ice::CommunicatorPtr& communicator, const vector<Byte>& data) :
    _communicator(communicator)
{
    _is = new BasicStream(getInstance(communicator).get());
    _is->closure(this);
    _is->writeBlob(data);
    _is->i = _is->b.begin();
}

Ice::InputStreamI::InputStreamI(const Ice::CommunicatorPtr& communicator, const pair<const Byte*, const Byte*>& data) :
    _communicator(communicator)
{
    _is = new BasicStream(getInstance(communicator).get());
    _is->closure(this);
    _is->writeBlob(data.first, data.second - data.first);
    _is->i = _is->b.begin();
}

Ice::InputStreamI::~InputStreamI()
{
    delete _is;
}

CommunicatorPtr
Ice::InputStreamI::communicator() const
{
    return _communicator;
}

void
Ice::InputStreamI::sliceObjects(bool b)
{
    _is->sliceObjects(b);
}

bool
Ice::InputStreamI::readBool()
{
    bool v;
    _is->read(v);
    return v;
}

vector<bool>
Ice::InputStreamI::readBoolSeq()
{
    vector<bool> v;
    _is->read(v);
    return v;
}

bool*
Ice::InputStreamI::readBoolSeq(pair<const bool*, const bool*>& p)
{
    return _is->read(p);
}

Byte
Ice::InputStreamI::readByte()
{
    Byte v;
    _is->read(v);
    return v;
}

vector<Byte>
Ice::InputStreamI::readByteSeq()
{
    pair<const Byte*, const Byte*> p;
    _is->read(p);
    vector<Byte> v(p.first, p.second);
    return v;
}

void
Ice::InputStreamI::readByteSeq(pair<const Byte*, const Byte*>& p)
{
    _is->read(p);
}

Short
Ice::InputStreamI::readShort()
{
    Short v;
    _is->read(v);
    return v;
}

vector<Short>
Ice::InputStreamI::readShortSeq()
{
    vector<Short> v;
    _is->read(v);
    return v;
}

Short*
Ice::InputStreamI::readShortSeq(pair<const Short*, const Short*>& p)
{
    return _is->read(p);
}

Int
Ice::InputStreamI::readInt()
{
    Int v;
    _is->read(v);
    return v;
}

vector<Int>
Ice::InputStreamI::readIntSeq()
{
    vector<Int> v;
    _is->read(v);
    return v;
}

Int*
Ice::InputStreamI::readIntSeq(pair<const Int*, const Int*>& p)
{
    return _is->read(p);
}

Long
Ice::InputStreamI::readLong()
{
    Long v;
    _is->read(v);
    return v;
}

vector<Long>
Ice::InputStreamI::readLongSeq()
{
    vector<Long> v;
    _is->read(v);
    return v;
}

Long*
Ice::InputStreamI::readLongSeq(pair<const Long*, const Long*>& p)
{
    return _is->read(p);
}

Float
Ice::InputStreamI::readFloat()
{
    Float v;
    _is->read(v);
    return v;
}

vector<Float>
Ice::InputStreamI::readFloatSeq()
{
    vector<Float> v;
    _is->read(v);
    return v;
}

Float*
Ice::InputStreamI::readFloatSeq(pair<const Float*, const Float*>& p)
{
    return _is->read(p);
}

Double
Ice::InputStreamI::readDouble()
{
    Double v;
    _is->read(v);
    return v;
}

vector<Double>
Ice::InputStreamI::readDoubleSeq()
{
    vector<Double> v;
    _is->read(v);
    return v;
}

Double*
Ice::InputStreamI::readDoubleSeq(pair<const Double*, const Double*>& p)
{
    return _is->read(p);
}

string
Ice::InputStreamI::readString(bool convert)
{
    string v;
    _is->read(v, convert);
    return v;
}

vector<string>
Ice::InputStreamI::readStringSeq(bool convert)
{
    vector<string> v;
    _is->read(v, convert);
    return v;
}

wstring
Ice::InputStreamI::readWstring()
{
    wstring v;
    _is->read(v);
    return v;
}

vector<wstring>
Ice::InputStreamI::readWstringSeq()
{
    vector<wstring> v;
    _is->read(v);
    return v;
}

Int
Ice::InputStreamI::readSize()
{
    Int sz;
    _is->readSize(sz);
    return sz;
}

Int
Ice::InputStreamI::readAndCheckSeqSize(int minSize)
{
    Int sz;
    _is->readAndCheckSeqSize(minSize, sz);
    return sz;
}

ObjectPrx
Ice::InputStreamI::readProxy()
{
    Ice::ObjectPrx v;
    _is->read(v);
    return v;
}

namespace 
{

void
patchObject(void* addr, ObjectPtr& v)
{
    ReadObjectCallback* cb = static_cast<ReadObjectCallback*>(addr);
    assert(cb);
    cb->invoke(v);
}

}

void
Ice::InputStreamI::readObject(const ReadObjectCallbackPtr& cb)
{
    _callbacks.push_back(cb); // Keep reference to callback.
    _is->read(patchObject, cb.get());
}

string
Ice::InputStreamI::readTypeId()
{
    string id;
    _is->readTypeId(id);
    return id;
}

void
Ice::InputStreamI::read(bool& v)
{
    _is->read(v);
}

void
Ice::InputStreamI::read(::Ice::Byte& v)
{
    _is->read(v);
}

void
Ice::InputStreamI::read(::Ice::Short& v)
{
    _is->read(v);
}

void
Ice::InputStreamI::read(Ice::Int& v)
{
    _is->read(v);
}

void
Ice::InputStreamI::read(Ice::Long& v)
{
    _is->read(v);
}

void
Ice::InputStreamI::read(Ice::Float& v)
{
    _is->read(v);
}

void
Ice::InputStreamI::read(Ice::Double& v)
{
    _is->read(v);
}

void
Ice::InputStreamI::read(std::string& v, bool convert)
{
    _is->read(v, convert);
}

void
Ice::InputStreamI::read(std::vector<std::string>& v, bool convert)
{
    _is->read(v, convert);
}

void
Ice::InputStreamI::read(std::wstring& v)
{
    _is->read(v);
}

void
Ice::InputStreamI::read(std::pair<const bool*, const bool*>& p, ::IceUtil::ScopedArray<bool>& result)
{
    result.reset(_is->read(p));
}

void
Ice::InputStreamI::read(std::pair<const Ice::Byte*, const Ice::Byte*>& p)
{
    _is->read(p);
}
void
Ice::InputStreamI::read(std::pair<const Ice::Short*, const Ice::Short*>& p, ::IceUtil::ScopedArray<Ice::Short>& result)
{
    result.reset(_is->read(p));
}

void
Ice::InputStreamI::read(std::pair<const Ice::Int*, const Ice::Int*>& p, ::IceUtil::ScopedArray<Ice::Int>& result)
{
    result.reset(_is->read(p));
}

void
Ice::InputStreamI::read(std::pair<const Ice::Long*, const Ice::Long*>& p, ::IceUtil::ScopedArray<Ice::Long>& result)
{
    result.reset(_is->read(p));
}

void
Ice::InputStreamI::read(std::pair<const Ice::Float*, const Ice::Float*>& p, ::IceUtil::ScopedArray<Ice::Float>& result)
{
    result.reset(_is->read(p));
}

void
Ice::InputStreamI::read(std::pair<const Ice::Double*, const Ice::Double*>& p, 
                        ::IceUtil::ScopedArray<Ice::Double>& result)
{
    result.reset(_is->read(p));
}

void
Ice::InputStreamI::throwException()
{
    _is->throwException();
}

void
Ice::InputStreamI::startSlice()
{
    _is->startReadSlice();
}

void
Ice::InputStreamI::endSlice()
{
    _is->endReadSlice();
}

void
Ice::InputStreamI::startEncapsulation()
{
    _is->startReadEncaps();
}

void
Ice::InputStreamI::endEncapsulation()
{
    _is->endReadEncapsChecked();
}

void
Ice::InputStreamI::skipEncapsulation()
{
    _is->skipEncaps();
}

void
Ice::InputStreamI::skipSlice()
{
    _is->skipSlice();
}

void
Ice::InputStreamI::readPendingObjects()
{
    _is->readPendingObjects();
}

void
Ice::InputStreamI::rewind()
{
    _is->clear();
    _is->i = _is->b.begin();
}

//
// OutputStreamI
//
Ice::OutputStreamI::OutputStreamI(const Ice::CommunicatorPtr& communicator, BasicStream* os) :
    _communicator(communicator), _os(os), _own(!os)
{
    if(!_os)
    {
        _os = new BasicStream(getInstance(communicator).get());
    }
    _os->closure(this);
}

Ice::OutputStreamI::~OutputStreamI()
{
    if(_own)
    {
        delete _os;
    }
}

CommunicatorPtr
Ice::OutputStreamI::communicator() const
{
    return _communicator;
}

void
Ice::OutputStreamI::writeObject(const ObjectPtr& v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeException(const UserException& v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeProxy(const ObjectPrx& v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeSize(Int sz)
{
    if(sz < 0)
    {
        throw MarshalException(__FILE__, __LINE__);
    }

    _os->writeSize(sz);
}

void
Ice::OutputStreamI::writeTypeId(const string& id)
{
    _os->writeTypeId(id);
}

void
Ice::OutputStreamI::write(bool v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::write(Byte v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::write(Short v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::write(Int v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::write(Long v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::write(Float v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::write(Double v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::write(const string& v, bool convert)
{
    _os->write(v, convert);
}

void
Ice::OutputStreamI::write(const char* v, bool convert)
{
    _os->write(v, convert);
}

void
Ice::OutputStreamI::write(const vector<string>& v, bool convert)
{
    if(v.size() == 0)
    {
        _os->writeSize(0);
    }
    else
    {
        _os->write(&v[0], &v[0] + v.size(), convert);
    }
}

void
Ice::OutputStreamI::write(const wstring& v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::write(const bool* begin, const bool* end)
{
    _os->write(begin, end);
}

void
Ice::OutputStreamI::write(const Byte* begin, const Byte* end)
{
    _os->write(begin, end);
}

void
Ice::OutputStreamI::write(const Int* begin, const Int* end)
{
    _os->write(begin, end);
}

void
Ice::OutputStreamI::write(const Long* begin, const Long* end)
{
    _os->write(begin, end);
}

void
Ice::OutputStreamI::write(const Float* begin, const Float* end)
{
    _os->write(begin, end);
}

void
Ice::OutputStreamI::write(const Double* begin, const Double* end)
{
    _os->write(begin, end);
}

void
Ice::OutputStreamI::startSlice()
{
    _os->startWriteSlice();
}

void
Ice::OutputStreamI::endSlice()
{
    _os->endWriteSlice();
}

void
Ice::OutputStreamI::startEncapsulation()
{
    _os->startWriteEncaps();
}

void
Ice::OutputStreamI::endEncapsulation()
{
    _os->endWriteEncapsChecked();
}

void
Ice::OutputStreamI::writePendingObjects()
{
    _os->writePendingObjects();
}

void
Ice::OutputStreamI::finished(vector<Byte>& bytes)
{
    vector<Byte>(_os->b.begin(), _os->b.end()).swap(bytes);
}

void
Ice::OutputStreamI::reset(bool clearBuffer)
{
    _os->clear();
    
    if(clearBuffer)
    {
        _os->b.clear();
    }
    else
    {
        _os->b.reset();
    }
    
    _os->i = _os->b.begin();
}

void
Ice::OutputStreamI::writeBool(bool v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeBoolSeq(const vector<bool>& v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeBoolSeq(const bool* begin, const bool* end)
{
    write(begin, end);
}

void
Ice::OutputStreamI::writeByte(Byte v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeByteSeq(const vector<Byte>& v)
{
    if(v.size() == 0)
    {
        _os->writeSize(0);
    }
    else
    {
        _os->write(&v[0], &v[0] + v.size());
    }
}

void
Ice::OutputStreamI::writeByteSeq(const Ice::Byte* begin, const Ice::Byte* end)
{
    write(begin, end);
}

void
Ice::OutputStreamI::writeShort(Short v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeShortSeq(const vector<Short>& v)
{
    if(v.size() == 0)
    {
        _os->writeSize(0);
    }
    else
    {
        _os->write(&v[0], &v[0] + v.size());
    }
}

void
Ice::OutputStreamI::writeShortSeq(const Ice::Short* begin, const Ice::Short* end)
{
    write(begin, end);
}

void
Ice::OutputStreamI::writeInt(Int v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeIntSeq(const vector<Int>& v)
{
    if(v.size() == 0)
    {
        _os->writeSize(0);
    }
    else
    {
        _os->write(&v[0], &v[0] + v.size());
    }
}

void
Ice::OutputStreamI::writeIntSeq(const Ice::Int* begin, const Ice::Int* end)
{
    write(begin, end);
}

void
Ice::OutputStreamI::writeLong(Long v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeLongSeq(const vector<Long>& v)
{
    if(v.size() == 0)
    {
        _os->writeSize(0);
    }
    else
    {
        _os->write(&v[0], &v[0] + v.size());
    }
}

void
Ice::OutputStreamI::writeLongSeq(const Ice::Long* begin, const Ice::Long* end)
{
    write(begin, end);
}

void
Ice::OutputStreamI::writeFloat(Float v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeFloatSeq(const vector<Float>& v)
{
    if(v.size() == 0)
    {
        _os->writeSize(0);
    }
    else
    {
        _os->write(&v[0], &v[0] + v.size());
    }
}

void
Ice::OutputStreamI::writeFloatSeq(const Ice::Float* begin, const Ice::Float* end)
{
    write(begin, end);
}

void
Ice::OutputStreamI::writeDouble(Double v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeDoubleSeq(const vector<Double>& v)
{
    if(v.size() == 0)
    {
        _os->writeSize(0);
    }
    else
    {
        _os->write(&v[0], &v[0] + v.size());
    }
}

void
Ice::OutputStreamI::writeDoubleSeq(const Ice::Double* begin, const Ice::Double* end)
{
    write(begin, end);
}

void
Ice::OutputStreamI::write(const Short* begin, const Short* end)
{
    _os->write(begin, end);
}

void
Ice::OutputStreamI::writeString(const string& v, bool convert)
{
    _os->write(v, convert);
}

void
Ice::OutputStreamI::writeStringSeq(const vector<string>& v, bool convert)
{
    if(v.size() == 0)
    {
        _os->writeSize(0);
    }
    else
    {
        _os->write(&v[0], &v[0] + v.size(), convert);
    }
}

void
Ice::OutputStreamI::writeWstring(const wstring& v)
{
    _os->write(v);
}

void
Ice::OutputStreamI::writeWstringSeq(const vector<wstring>& v)
{
    if(v.size() == 0)
    {
        _os->writeSize(0);
    }
    else
    {
        _os->write(&v[0], &v[0] + v.size());
    }
}

//
// ObjectReader
//
void
Ice::ObjectReader::__write(BasicStream*) const
{
    assert(false);
}

void
Ice::ObjectReader::__read(BasicStream* is, bool rid)
{
    InputStreamI* stream = reinterpret_cast<InputStreamI*>(is->closure());
    assert(stream);
    read(stream, rid);
}

void
Ice::ObjectReader::__write(const Ice::OutputStreamPtr&) const
{
    assert(false);
}

void
Ice::ObjectReader::__read(const Ice::InputStreamPtr&, bool)
{
    assert(false);
}

//
// ObjectWriter
//
void
Ice::ObjectWriter::__write(BasicStream* os) const
{
    OutputStreamI* stream = reinterpret_cast<OutputStreamI*>(os->closure());
    assert(stream);
    write(stream);
}

void
Ice::ObjectWriter::__read(BasicStream*, bool)
{
    assert(false);
}

void
Ice::ObjectWriter::__write(const Ice::OutputStreamPtr&) const
{
    assert(false);
}

void
Ice::ObjectWriter::__read(const Ice::InputStreamPtr&, bool)
{
    assert(false);
}

//
// UserExceptionWriter
//
Ice::UserExceptionWriter::UserExceptionWriter(const Ice::CommunicatorPtr& communicator) :
    _communicator(communicator)
{
}

Ice::UserExceptionWriter::~UserExceptionWriter() throw()
{
}

void
Ice::UserExceptionWriter::__write(BasicStream* os) const
{
    OutputStreamI* stream = reinterpret_cast<OutputStreamI*>(os->closure());
    if(!stream)
    {
        //
        // Required for IcePy usage
        //
        stream = new OutputStreamI(_communicator, os);
    }
    write(stream);
}

void
Ice::UserExceptionWriter::__read(BasicStream*, bool)
{
    assert(false);
}

bool
Ice::UserExceptionWriter::__usesClasses() const
{
    return usesClasses();
}
