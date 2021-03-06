# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import Ice, threading

Ice.loadSlice('Test.ice')
import Test

def test(b):
    if not b:
        raise RuntimeError('test assertion failed')

class CallbackBase:
    def __init__(self):
        self._called = False
        self._cond = threading.Condition()

    def check(self):
        self._cond.acquire()
        try:
            while not self._called:
                self._cond.wait()
            self._called = False
        finally:
            self._cond.release()

    def called(self):
        self._cond.acquire()
        self._called = True
        self._cond.notify()
        self._cond.release()

class Callback(CallbackBase):
    def response(self):
        test(False)

    def exception(self, ex):
        test(False)

    def opPidI(self, pid):
        self._pid = pid
        self.called()

    def opShutdownI(self):
        self.called()

    def exceptAbortI(self, ex):
        try:
            raise ex
        except Ice.ConnectionLostException:
            pass
        except Ice.ConnectFailedException:
            pass
        except Ice.Exception, ex:
            print ex
            test(False)
        self.called()

    def pid(self):
        return self._pid

def allTests(communicator, ports):
    print "testing stringToProxy... ",
    ref = "test"
    for p in ports:
        ref = ref + ":default -p " + str(p)
    base = communicator.stringToProxy(ref)
    test(base)
    print "ok"

    print "testing checked cast... ",
    obj = Test.TestIntfPrx.checkedCast(base)
    test(obj)
    test(obj == base)
    print "ok"

    oldPid = 0
    ami = False
    i = 1
    j = 0
    while i <= len(ports):
        if j > 3:
            j = 0
            ami = not ami

        if not ami:
            print "testing server #%d... " % i,
            pid = obj.pid()
            test(pid != oldPid)
            print "ok"
            oldPid = pid
        else:
            print "testing server #%d with AMI... " % i,
            cb = Callback()
            obj.begin_pid(cb.opPidI, cb.exception)
            cb.check()
            pid = cb.pid()
            test(pid != oldPid)
            print "ok"
            oldPid = pid

        if j == 0:
            if not ami:
                print "shutting down server #%d... " % i,
                obj.shutdown()
                print "ok"
            else:
                print "shutting down server #%d with AMI... " % i,
                cb = Callback()
                obj.begin_shutdown(cb.opShutdownI, cb.exception)
                cb.check()
                print "ok"
        elif j == 1 or i + 1 > len(ports):
            if not ami:
                print "aborting server #%d... " % i,
                try:
                    obj.abort()
                    test(False)
                except Ice.ConnectionLostException:
                    print "ok"
                except Ice.ConnectFailedException:
                    print "ok"
            else:
                print "aborting server #%d with AMI... " % i,
                cb = Callback()
                obj.begin_abort(cb.response, cb.exceptAbortI)
                cb.check()
                print "ok"
        elif j == 2 or j == 3:
            if not ami:
                print "aborting server #%d and #%d with idempotent call... " % (i, i + 1),
                try:
                    obj.idempotentAbort()
                    test(False)
                except Ice.ConnectionLostException:
                    print "ok"
                except Ice.ConnectFailedException:
                    print "ok"
            else:
                print "aborting server #%d and #%d with idempotent AMI call... " % (i, i + 1),
                cb = Callback()
                obj.begin_idempotentAbort(cb.response, cb.exceptAbortI)
                cb.check()
                print "ok"

            i = i + 1
        else:
            assert(False)

        i = i + 1
        j = j + 1

    print "testing whether all servers are gone... ",
    try:
        obj.ice_ping()
        test(False)
    except Ice.LocalException:
        print "ok"
