// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Diagnostics;
using System.Threading;
using Test;

public class AllTests
{
    private static void test(bool b)
    {
        if(!b)
        {
            throw new System.Exception();
        }
    }

    private class Callback
    {
        internal Callback()
        {
            _called = false;
        }

        public virtual void check()
        {
            _m.Lock();
            try
            {
                while(!_called)
                {
                    _m.Wait();
                }

                _called = false;
            }
            finally
            {
                _m.Unlock();
            }
        }

        public virtual void called()
        {
            _m.Lock();
            try
            {
                Debug.Assert(!_called);
                _called = true;
                _m.Notify();
            }
            finally
            {
                _m.Unlock();
            }
        }

        private bool _called;
        private readonly IceUtilInternal.Monitor _m = new IceUtilInternal.Monitor();
    }

    private class AsyncCallback
    {
        public void response()
        {
            AllTests.test(false);
        }

        public void exception_baseAsBase(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Base b)
            {
                AllTests.test(b.b.Equals("Base.b"));
                AllTests.test(b.GetType().Name.Equals("Base"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_unknownDerivedAsBase(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Base b)
            {
                AllTests.test(b.b.Equals("UnknownDerived.b"));
                AllTests.test(b.GetType().Name.Equals("Base"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_knownDerivedAsBase(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(KnownDerived k)
            {
                AllTests.test(k.b.Equals("KnownDerived.b"));
                AllTests.test(k.kd.Equals("KnownDerived.kd"));
                AllTests.test(k.GetType().Name.Equals("KnownDerived"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_knownDerivedAsKnownDerived(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(KnownDerived k)
            {
                AllTests.test(k.b.Equals("KnownDerived.b"));
                AllTests.test(k.kd.Equals("KnownDerived.kd"));
                AllTests.test(k.GetType().Name.Equals("KnownDerived"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_unknownIntermediateAsBase(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Base b)
            {
                AllTests.test(b.b.Equals("UnknownIntermediate.b"));
                AllTests.test(b.GetType().Name.Equals("Base"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_knownIntermediateAsBase(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(KnownIntermediate ki)
            {
                AllTests.test(ki.b.Equals("KnownIntermediate.b"));
                AllTests.test(ki.ki.Equals("KnownIntermediate.ki"));
                AllTests.test(ki.GetType().Name.Equals("KnownIntermediate"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_knownMostDerivedAsBase(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(KnownMostDerived kmd)
            {
                AllTests.test(kmd.b.Equals("KnownMostDerived.b"));
                AllTests.test(kmd.ki.Equals("KnownMostDerived.ki"));
                AllTests.test(kmd.kmd.Equals("KnownMostDerived.kmd"));
                AllTests.test(kmd.GetType().Name.Equals("KnownMostDerived"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_knownIntermediateAsKnownIntermediate(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(KnownIntermediate ki)
            {
                AllTests.test(ki.b.Equals("KnownIntermediate.b"));
                AllTests.test(ki.ki.Equals("KnownIntermediate.ki"));
                AllTests.test(ki.GetType().Name.Equals("KnownIntermediate"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_knownMostDerivedAsKnownIntermediate(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(KnownMostDerived kmd)
            {
                AllTests.test(kmd.b.Equals("KnownMostDerived.b"));
                AllTests.test(kmd.ki.Equals("KnownMostDerived.ki"));
                AllTests.test(kmd.kmd.Equals("KnownMostDerived.kmd"));
                AllTests.test(kmd.GetType().Name.Equals("KnownMostDerived"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_knownMostDerivedAsKnownMostDerived(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(KnownMostDerived kmd)
            {
                AllTests.test(kmd.b.Equals("KnownMostDerived.b"));
                AllTests.test(kmd.ki.Equals("KnownMostDerived.ki"));
                AllTests.test(kmd.kmd.Equals("KnownMostDerived.kmd"));
                AllTests.test(kmd.GetType().Name.Equals("KnownMostDerived"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_unknownMostDerived1AsBase(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(KnownIntermediate ki)
            {
                AllTests.test(ki.b.Equals("UnknownMostDerived1.b"));
                AllTests.test(ki.ki.Equals("UnknownMostDerived1.ki"));
                AllTests.test(ki.GetType().Name.Equals("KnownIntermediate"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_unknownMostDerived1AsKnownIntermediate(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(KnownIntermediate ki)
            {
                AllTests.test(ki.b.Equals("UnknownMostDerived1.b"));
                AllTests.test(ki.ki.Equals("UnknownMostDerived1.ki"));
                AllTests.test(ki.GetType().Name.Equals("KnownIntermediate"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public void exception_unknownMostDerived2AsBase(Ice.Exception exc)
        {
            try
            {
                throw exc;
            }
            catch(Base b)
            {
                AllTests.test(b.b.Equals("UnknownMostDerived2.b"));
                AllTests.test(b.GetType().Name.Equals("Base"));
            }
            catch(Exception)
            {
                AllTests.test(false);
            }
            callback.called();
        }

        public virtual void check()
        {
            callback.check();
        }

        private Callback callback = new Callback();
    }

    public static TestIntfPrx allTests(Ice.Communicator communicator, bool collocated)
    {
        Console.Out.Write("testing stringToProxy... ");
        Console.Out.Flush();
        String @ref = "Test:default -p 12010 -t 2000";
        Ice.ObjectPrx @base = communicator.stringToProxy(@ref);
        test(@base != null);
        Console.Out.WriteLine("ok");

        Console.Out.Write("testing checked cast... ");
        Console.Out.Flush();
        TestIntfPrx testPrx = TestIntfPrxHelper.checkedCast(@base);
        test(testPrx != null);
        test(testPrx.Equals(@base));
        Console.Out.WriteLine("ok");

        Console.Out.Write("base... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.baseAsBase();
                test(false);
            }
            catch(Base b)
            {
                test(b.b.Equals("Base.b"));
                test(b.GetType().FullName.Equals("Test.Base"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("base (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_baseAsBase().whenCompleted(cb.response, cb.exception_baseAsBase);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of unknown derived... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.unknownDerivedAsBase();
                test(false);
            }
            catch(Base b)
            {
                test(b.b.Equals("UnknownDerived.b"));
                test(b.GetType().FullName.Equals("Test.Base"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of unknown derived (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_unknownDerivedAsBase().whenCompleted(cb.response, cb.exception_unknownDerivedAsBase);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("non-slicing of known derived as base... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.knownDerivedAsBase();
                test(false);
            }
            catch(KnownDerived k)
            {
                test(k.b.Equals("KnownDerived.b"));
                test(k.kd.Equals("KnownDerived.kd"));
                test(k.GetType().FullName.Equals("Test.KnownDerived"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("non-slicing of known derived as base (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_knownDerivedAsBase().whenCompleted(cb.response, cb.exception_knownDerivedAsBase);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("non-slicing of known derived as derived... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.knownDerivedAsKnownDerived();
                test(false);
            }
            catch(KnownDerived k)
            {
                test(k.b.Equals("KnownDerived.b"));
                test(k.kd.Equals("KnownDerived.kd"));
                test(k.GetType().FullName.Equals("Test.KnownDerived"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("non-slicing of known derived as derived (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_knownDerivedAsKnownDerived().whenCompleted(
                        cb.response, cb.exception_knownDerivedAsKnownDerived);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of unknown intermediate as base... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.unknownIntermediateAsBase();
                test(false);
            }
            catch(Base b)
            {
                test(b.b.Equals("UnknownIntermediate.b"));
                test(b.GetType().FullName.Equals("Test.Base"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of unknown intermediate as base (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_unknownIntermediateAsBase().whenCompleted(
                        cb.response, cb.exception_unknownIntermediateAsBase);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of known intermediate as base... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.knownIntermediateAsBase();
                test(false);
            }
            catch(KnownIntermediate ki)
            {
                test(ki.b.Equals("KnownIntermediate.b"));
                test(ki.ki.Equals("KnownIntermediate.ki"));
                test(ki.GetType().FullName.Equals("Test.KnownIntermediate"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of known intermediate as base (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_knownIntermediateAsBase().whenCompleted(
                        cb.response, cb.exception_knownIntermediateAsBase);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of known most derived as base... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.knownMostDerivedAsBase();
                test(false);
            }
            catch(KnownMostDerived kmd)
            {
                test(kmd.b.Equals("KnownMostDerived.b"));
                test(kmd.ki.Equals("KnownMostDerived.ki"));
                test(kmd.kmd.Equals("KnownMostDerived.kmd"));
                test(kmd.GetType().FullName.Equals("Test.KnownMostDerived"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of known most derived as base (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_knownMostDerivedAsBase().whenCompleted(
                        cb.response, cb.exception_knownMostDerivedAsBase);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("non-slicing of known intermediate as intermediate... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.knownIntermediateAsKnownIntermediate();
                test(false);
            }
            catch(KnownIntermediate ki)
            {
                test(ki.b.Equals("KnownIntermediate.b"));
                test(ki.ki.Equals("KnownIntermediate.ki"));
                test(ki.GetType().FullName.Equals("Test.KnownIntermediate"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("non-slicing of known intermediate as intermediate (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_knownIntermediateAsKnownIntermediate().whenCompleted(
                        cb.response, cb.exception_knownIntermediateAsKnownIntermediate);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("non-slicing of known most derived as intermediate... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.knownMostDerivedAsKnownIntermediate();
                test(false);
            }
            catch(KnownMostDerived kmd)
            {
                test(kmd.b.Equals("KnownMostDerived.b"));
                test(kmd.ki.Equals("KnownMostDerived.ki"));
                test(kmd.kmd.Equals("KnownMostDerived.kmd"));
                test(kmd.GetType().FullName.Equals("Test.KnownMostDerived"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("non-slicing of known most derived as intermediate (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_knownMostDerivedAsKnownIntermediate().whenCompleted(
                        cb.response, cb.exception_knownMostDerivedAsKnownIntermediate);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("non-slicing of known most derived as most derived... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.knownMostDerivedAsKnownMostDerived();
                test(false);
            }
            catch(KnownMostDerived kmd)
            {
                test(kmd.b.Equals("KnownMostDerived.b"));
                test(kmd.ki.Equals("KnownMostDerived.ki"));
                test(kmd.kmd.Equals("KnownMostDerived.kmd"));
                test(kmd.GetType().FullName.Equals("Test.KnownMostDerived"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("non-slicing of known most derived as most derived (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_knownMostDerivedAsKnownMostDerived().whenCompleted(
                        cb.response, cb.exception_knownMostDerivedAsKnownMostDerived);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of unknown most derived, known intermediate as base... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.unknownMostDerived1AsBase();
                test(false);
            }
            catch(KnownIntermediate ki)
            {
                test(ki.b.Equals("UnknownMostDerived1.b"));
                test(ki.ki.Equals("UnknownMostDerived1.ki"));
                test(ki.GetType().FullName.Equals("Test.KnownIntermediate"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of unknown most derived, known intermediate as base (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_unknownMostDerived1AsBase().whenCompleted(
                        cb.response, cb.exception_unknownMostDerived1AsBase);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of unknown most derived, known intermediate as intermediate... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.unknownMostDerived1AsKnownIntermediate();
                test(false);
            }
            catch(KnownIntermediate ki)
            {
                test(ki.b.Equals("UnknownMostDerived1.b"));
                test(ki.ki.Equals("UnknownMostDerived1.ki"));
                test(ki.GetType().FullName.Equals("Test.KnownIntermediate"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of unknown most derived, known intermediate as intermediate (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_unknownMostDerived1AsKnownIntermediate().whenCompleted(
                        cb.response, cb.exception_unknownMostDerived1AsKnownIntermediate);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of unknown most derived, unknown intermediate thrown as base... ");
        Console.Out.Flush();
        {
            try
            {
                testPrx.unknownMostDerived2AsBase();
                test(false);
            }
            catch(Base b)
            {
                test(b.b.Equals("UnknownMostDerived2.b"));
                test(b.GetType().FullName.Equals("Test.Base"));
            }
            catch(Exception)
            {
                test(false);
            }
        }
        Console.Out.WriteLine("ok");

        Console.Out.Write("slicing of unknown most derived, unknown intermediate thrown as base (AMI)... ");
        Console.Out.Flush();
        {
            AsyncCallback cb = new AsyncCallback();
            testPrx.begin_unknownMostDerived2AsBase().whenCompleted(
                        cb.response, cb.exception_unknownMostDerived2AsBase);
            cb.check();
        }
        Console.Out.WriteLine("ok");

        return testPrx;
    }
}
