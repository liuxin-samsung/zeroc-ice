#!/usr/bin/env python
# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

import os, sys, time, re

path = [ ".", "..", "../..", "../../..", "../../../.." ]
head = os.path.dirname(sys.argv[0])
if len(head) > 0:
    path = [os.path.join(head, p) for p in path]
path = [os.path.abspath(p) for p in path if os.path.exists(os.path.join(p, "scripts", "TestUtil.py")) ]
if len(path) == 0:
    raise "can't find toplevel directory!"
sys.path.append(os.path.join(path[0]))
from scripts import *

publisher = os.path.join(os.getcwd(), "publisher")
subscriber = os.path.join(os.getcwd(), "subscriber")
subscriber2 = os.path.join(os.getcwd(), "sub")

targets = []
if TestUtil.appverifier:
    targets = [TestUtil.getIceBox(), publisher, subscriber, subscriber2, TestUtil.getIceBoxAdmin(), \
               TestUtil.getIceStormAdmin()]
    TestUtil.setAppVerifierSettings(targets, cwd = os.getcwd())

def runsub(opt, ref, arg = "", echo=False):
    qos = ""
    if opt == "twoway":
        qos = " --twoway"
    if opt == "ordered":
        qos = " --ordered"
    return TestUtil.startServer(subscriber, ref + arg + qos)

def runpub(ref, arg = "", echo=False):
    return TestUtil.startClient(publisher, ref + arg)

def runtest(opt, ref, subopt="", pubopt=""):
    subscriberProc = runsub(opt, ref, subopt)
    publisherProc = runpub(ref, pubopt)
    publisherProc.waitTestSuccess()
    subscriberProc.waitTestSuccess(timeout=30)

def runsub2(replica = -1, expect = None):
    proc = TestUtil.startServer(subscriber2, icestorm.reference(replica) + ' --id foo', count=0, echo = False)
    if expect:
        proc.expect(expect)
        proc.wait()
    else:
        proc.waitTestSuccess()

def rununsub2(replica = -1, expect = None):
    if replica == -1:
        proc = TestUtil.startServer(subscriber2, icestorm.reference(replica) + ' --id foo --unsub', count=0)
        proc.waitTestSuccess()
    # Else we first subscribe to this replica, then unsub. We
    # shouldn't get an AlreadySubscribedException.
    proc = TestUtil.startServer(subscriber2, icestorm.reference(replica) + ' --id foo', count=0, echo = False)
    if expect:
        proc.expect(expect)
        proc.wait()
        return
    else:
        proc.waitTestSuccess()
    proc = TestUtil.startServer(subscriber2, icestorm.reference(replica) + ' --id foo --unsub', count=0)
    proc.waitTestSuccess()

icestorm = IceStormUtil.init(TestUtil.toplevel, os.getcwd(), "replicated", replicatedPublisher=True, additional =
                             ' --IceStorm.Election.MasterTimeout=2' +
                             ' --IceStorm.Election.ElectionTimeout=2' +
                             ' --IceStorm.Election.ResponseTimeout=2')
icestorm.start()

print "testing topic creation across replicas...",
sys.stdout.flush()
icestorm.admin("create single")

for replica in range(0, 3):
    icestorm.adminForReplica(replica, "create single", "error: topic `single' exists")
print "ok"

print "testing topic destruction across replicas...",
sys.stdout.flush()
icestorm.admin("destroy single")

for replica in range(0, 3):
    icestorm.adminForReplica(replica, "destroy single", "error: couldn't find topic `single'")
print "ok"

print "testing topic creation without replica...",
sys.stdout.flush()

icestorm.stopReplica(0)
icestorm.admin("create single")

for replica in range(1, 3):
    icestorm.adminForReplica(replica, "create single", "error: topic `single' exists")

icestorm.adminForReplica(0, "create single", "ConnectionRefused")

icestorm.startReplica(0, echo=False)

icestorm.adminForReplica(0, "create single", "error: topic `single' exists")
print "ok"

icestorm.admin("destroy single")

print "testing topic creation without master...",
sys.stdout.flush()
icestorm.stopReplica(2)

icestorm.admin("create single")

for replica in range(0, 2):
    icestorm.adminForReplica(replica, "create single", "error: topic `single' exists")

icestorm.adminForReplica(2, "create single", "ConnectionRefused")

icestorm.startReplica(2, echo=False)

icestorm.adminForReplica(2, "create single", "error: topic `single' exists")
print "ok"

# All replicas are running

print "testing topic destruction without replica...",
sys.stdout.flush()
icestorm.stopReplica(0)

icestorm.admin("destroy single")

for replica in range(1, 3):
    icestorm.adminForReplica(replica, "destroy single", "error: couldn't find topic `single'")

icestorm.adminForReplica(0, "destroy single", "ConnectionRefused")

icestorm.startReplica(0, echo=False)

icestorm.adminForReplica(0, "destroy single", "error: couldn't find topic `single'")
print "ok"

print "testing topic destruction without master...",
sys.stdout.flush()

icestorm.admin("create single")
icestorm.stopReplica(2)

icestorm.admin("destroy single")

for replica in range(0, 2):
    icestorm.adminForReplica(replica, "destroy single", "error: couldn't find topic `single'")

icestorm.adminForReplica(2, "destroy single", "ConnectionRefused")

icestorm.startReplica(2, echo=False)

icestorm.adminForReplica(2, "destroy single", "error: couldn't find topic `single'")
print "ok"

# Now test subscription/unsubscription on all replicas.

icestorm.admin("create single")

print "testing subscription across replicas...",
sys.stdout.flush()
runsub2()

for replica in range(0, 3):
    runsub2(replica, "IceStorm::AlreadySubscribed")
print "ok"

print "testing unsubscription across replicas...",
sys.stdout.flush()
rununsub2()

for replica in range(0, 3):
    rununsub2(replica)
print "ok"

print "testing subscription without master...",
sys.stdout.flush()
icestorm.stopReplica(2)

runsub2()

for replica in range(0, 2):
    runsub2(replica, "IceStorm::AlreadySubscribed")

runsub2(2, "ConnectionRefused")

icestorm.startReplica(2, echo=False)

runsub2(2, "IceStorm::AlreadySubscribed")
print "ok"

print "testing unsubscription without master...",
sys.stdout.flush()
icestorm.stopReplica(2)

rununsub2()

for replica in range(0, 2):
    rununsub2(replica)

rununsub2(2, "ConnectionRefused")

icestorm.startReplica(2, echo=False)

rununsub2(2)
print "ok"

print "testing subscription without replica...",
sys.stdout.flush()
icestorm.stopReplica(0)

runsub2()

for replica in range(1, 3):
    runsub2(replica, "IceStorm::AlreadySubscribed")

runsub2(0, "ConnectionRefused")

icestorm.startReplica(0, echo=False)

runsub2(0, "IceStorm::AlreadySubscribed")
print "ok"

print "testing unsubscription without replica...",
sys.stdout.flush()
icestorm.stopReplica(0)

rununsub2()

for replica in range(1, 3):
    rununsub2(replica)

rununsub2(0, "ConnectionRefused")

icestorm.startReplica(0, echo=False)

rununsub2(0)
print "ok"

# All replicas are running

print "running twoway subscription test...",
sys.stdout.flush()
runtest("twoway", icestorm.reference())
print "ok"

print "running ordered subscription test...",
sys.stdout.flush()
runtest("ordered", icestorm.reference())
print "ok"

icestorm.stopReplica(2)

print "running twoway, ordered subscription test without master...",
sys.stdout.flush()
runtest("twoway", icestorm.reference())
runtest("ordered", icestorm.reference())
print "ok"

icestorm.startReplica(2, echo = False)
icestorm.stopReplica(0)

print "running twoway, ordered subscription test without replica...",
sys.stdout.flush()
runtest("twoway", icestorm.reference())
runtest("ordered", icestorm.reference())
print "ok"

icestorm.startReplica(0, echo = False)

print "running cycle publishing test...",
sys.stdout.flush()
runtest("twoway", icestorm.reference(), pubopt=" --cycle")
print "ok"

print "stopping replicas...",
sys.stdout.flush()
icestorm.stop()
print "ok"

if TestUtil.appverifier:
    TestUtil.appVerifierAfterTestEnd(targets, cwd = os.getcwd())
