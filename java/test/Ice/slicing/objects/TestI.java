// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package test.Ice.slicing.objects;

import test.Ice.slicing.objects.server.Test.B;
import test.Ice.slicing.objects.server.Test.BDictHolder;
import test.Ice.slicing.objects.server.Test.BHolder;
import test.Ice.slicing.objects.server.Test.BaseException;
import test.Ice.slicing.objects.server.Test.D1;
import test.Ice.slicing.objects.server.Test.D2;
import test.Ice.slicing.objects.server.Test.D4;
import test.Ice.slicing.objects.server.Test.DerivedException;
import test.Ice.slicing.objects.server.Test.Forward;
import test.Ice.slicing.objects.server.Test.ForwardHolder;
import test.Ice.slicing.objects.server.Test.Hidden;
import test.Ice.slicing.objects.server.Test.SBSKnownDerived;
import test.Ice.slicing.objects.server.Test.SBSUnknownDerived;
import test.Ice.slicing.objects.server.Test.SBase;
import test.Ice.slicing.objects.server.Test.SS;
import test.Ice.slicing.objects.server.Test.SS1;
import test.Ice.slicing.objects.server.Test.SS2;
import test.Ice.slicing.objects.server.Test.SUnknown;
import test.Ice.slicing.objects.server.Test.UnknownDerivedException;
import test.Ice.slicing.objects.server.Test._TestIntfDisp;

public final class TestI extends _TestIntfDisp
{
    public
    TestI()
    {
    }

    public void
    shutdown(Ice.Current current)
    {
        current.adapter.getCommunicator().shutdown();
    }

    public Ice.Object
    SBaseAsObject(Ice.Current current)
    {
        SBase sb = new SBase();
        sb.sb = "SBase.sb";
        return sb;
    }

    public SBase
    SBaseAsSBase(Ice.Current current)
    {
        SBase sb = new SBase();
        sb.sb = "SBase.sb";
        return sb;
    }

    public SBase
    SBSKnownDerivedAsSBase(Ice.Current current)
    {
        SBSKnownDerived sbskd = new SBSKnownDerived();
        sbskd.sb = "SBSKnownDerived.sb";
        sbskd.sbskd = "SBSKnownDerived.sbskd";
        return sbskd;
    }

    public SBSKnownDerived
    SBSKnownDerivedAsSBSKnownDerived(Ice.Current current)
    {
        SBSKnownDerived sbskd = new SBSKnownDerived();
        sbskd.sb = "SBSKnownDerived.sb";
        sbskd.sbskd = "SBSKnownDerived.sbskd";
        return sbskd;
    }

    public SBase
    SBSUnknownDerivedAsSBase(Ice.Current current)
    {
        SBSUnknownDerived sbsud = new SBSUnknownDerived();
        sbsud.sb = "SBSUnknownDerived.sb";
        sbsud.sbsud = "SBSUnknownDerived.sbsud";
        return sbsud;
    }

    public Ice.Object
    SUnknownAsObject(Ice.Current current)
    {
        SUnknown su = new SUnknown();
        su.su = "SUnknown.su";
        return su;
    }

    public B
    oneElementCycle(Ice.Current current)
    {
        B b = new B();
        b.sb = "B1.sb";
        b.pb = b;
        return b;
    }

    public B
    twoElementCycle(Ice.Current current)
    {
        B b1 = new B();
        b1.sb = "B1.sb";
        B b2 = new B();
        b2.sb = "B2.sb";
        b2.pb = b1;
        b1.pb = b2;
        return b1;
    }

    public B
    D1AsB(Ice.Current current)
    {
        D1 d1 = new D1();
        d1.sb = "D1.sb";
        d1.sd1 = "D1.sd1";
        D2 d2 = new D2();
        d2.pb = d1;
        d2.sb = "D2.sb";
        d2.sd2 = "D2.sd2";
        d2.pd2 = d1;
        d1.pb = d2;
        d1.pd1 = d2;
        return d1;
    }

    public D1
    D1AsD1(Ice.Current current)
    {
        D1 d1 = new D1();
        d1.sb = "D1.sb";
        d1.sd1 = "D1.sd1";
        D2 d2 = new D2();
        d2.pb = d1;
        d2.sb = "D2.sb";
        d2.sd2 = "D2.sd2";
        d2.pd2 = d1;
        d1.pb = d2;
        d1.pd1 = d2;
        return d1;
    }

    public B
    D2AsB(Ice.Current current)
    {
        D2 d2 = new D2();
        d2.sb = "D2.sb";
        d2.sd2 = "D2.sd2";
        D1 d1 = new D1();
        d1.pb = d2;
        d1.sb = "D1.sb";
        d1.sd1 = "D1.sd1";
        d1.pd1 = d2;
        d2.pb = d1;
        d2.pd2 = d1;
        return d2;
    }

    public void
    paramTest1(BHolder p1, BHolder p2, Ice.Current current)
    {
        D1 d1 = new D1();
        d1.sb = "D1.sb";
        d1.sd1 = "D1.sd1";
        D2 d2 = new D2();
        d2.pb = d1;
        d2.sb = "D2.sb";
        d2.sd2 = "D2.sd2";
        d2.pd2 = d1;
        d1.pb = d2;
        d1.pd1 = d2;
        p1.value = d1;
        p2.value = d2;
    }

    public void
    paramTest2(BHolder p1, BHolder p2, Ice.Current current)
    {
        paramTest1(p2, p1, current);
    }

    public B
    paramTest3(BHolder p1, BHolder p2, Ice.Current current)
    {
        D2 d2 = new D2();
        d2.sb = "D2.sb (p1 1)";
        d2.pb = null;
        d2.sd2 = "D2.sd2 (p1 1)";
        p1.value = d2;

        D1 d1 = new D1();
        d1.sb = "D1.sb (p1 2)";
        d1.pb = null;
        d1.sd1 = "D1.sd2 (p1 2)";
        d1.pd1 = null;
        d2.pd2 = d1;

        D2 d4 = new D2();
        d4.sb = "D2.sb (p2 1)";
        d4.pb = null;
        d4.sd2 = "D2.sd2 (p2 1)";
        p2.value = d4;

        D1 d3 = new D1();
        d3.sb = "D1.sb (p2 2)";
        d3.pb = null;
        d3.sd1 = "D1.sd2 (p2 2)";
        d3.pd1 = null;
        d4.pd2 = d3;

        return d3;
    }

    public B
    paramTest4(BHolder p1, Ice.Current current)
    {
        D4 d4 = new D4();
        d4.sb = "D4.sb (1)";
        d4.pb = null;
        d4.p1 = new B();
        d4.p1.sb = "B.sb (1)";
        d4.p2 = new B();
        d4.p2.sb = "B.sb (2)";
        p1.value = d4;
        return d4.p2;
    }

    public B
    returnTest1(BHolder p1, BHolder p2, Ice.Current current)
    {
        Ice.Current c;
        paramTest1(p1, p2, current);
        return p1.value;
    }

    public B
    returnTest2(BHolder p1, BHolder p2, Ice.Current current)
    {
        Ice.Current c;
        paramTest1(p2, p1, current);
        return p1.value;
    }

    public B
    returnTest3(B p1, B p2, Ice.Current current)
    {
        return p1;
    }

    public SS
    sequenceTest(SS1 p1, SS2 p2, Ice.Current current)
    {
        SS ss = new SS();
        ss.c1 = p1;
        ss.c2 = p2;
        return ss;
    }

    public java.util.Map<Integer, B>
    dictionaryTest(java.util.Map<Integer, B> bin, BDictHolder bout, Ice.Current current)
    {
        bout.value = new java.util.HashMap<Integer, B>();
        int i;
        for(i = 0; i < 10; ++i)
        {
            B b = bin.get(i);
            D2 d2 = new D2();
            d2.sb = b.sb;
            d2.pb = b.pb;
            d2.sd2 = "D2";
            d2.pd2 = d2;
            bout.value.put(i * 10, d2);
        }
        java.util.Map<Integer, B> r = new java.util.HashMap<Integer, B>();
        for(i = 0; i < 10; ++i)
        {
            String s = "D1." + new Integer(i * 20).toString();
            D1 d1 = new D1();
            d1.sb = s;
            d1.pb = (i == 0 ? null : r.get((i - 1) * 20));
            d1.sd1 = s;
            d1.pd1 = d1;
            r.put(i * 20, d1);
        }
        return r;
    }

    public void
    throwBaseAsBase(Ice.Current current)
        throws BaseException
    {
        BaseException be = new BaseException();
        be.sbe = "sbe";
        be.pb = new B();
        be.pb.sb = "sb";
        be.pb.pb = be.pb;
        throw be;
    }

    public void
    throwDerivedAsBase(Ice.Current current)
        throws BaseException
    {
        DerivedException de = new DerivedException();
        de.sbe = "sbe";
        de.pb = new B();
        de.pb.sb = "sb1";
        de.pb.pb = de.pb;
        de.sde = "sde1";
        de.pd1 = new D1();
        de.pd1.sb = "sb2";
        de.pd1.pb = de.pd1;
        de.pd1.sd1 = "sd2";
        de.pd1.pd1 = de.pd1;
        throw de;
    }

    public void
    throwDerivedAsDerived(Ice.Current current)
        throws DerivedException
    {
        DerivedException de = new DerivedException();
        de.sbe = "sbe";
        de.pb = new B();
        de.pb.sb = "sb1";
        de.pb.pb = de.pb;
        de.sde = "sde1";
        de.pd1 = new D1();
        de.pd1.sb = "sb2";
        de.pd1.pb = de.pd1;
        de.pd1.sd1 = "sd2";
        de.pd1.pd1 = de.pd1;
        throw de;
    }

    public void
    throwUnknownDerivedAsBase(Ice.Current current)
        throws BaseException
    {
        D2 d2 = new D2();
        d2.sb = "sb d2";
        d2.pb = d2;
        d2.sd2 = "sd2 d2";
        d2.pd2 = d2;

        UnknownDerivedException ude = new UnknownDerivedException();
        ude.sbe = "sbe";
        ude.pb = d2;
        ude.sude = "sude";
        ude.pd2 = d2;
        throw ude;
    }

    public void
    useForward(ForwardHolder f, Ice.Current current)
    {
        f.value = new Forward();
        f.value.h = new Hidden();
        f.value.h.f = f.value;
    }
}
