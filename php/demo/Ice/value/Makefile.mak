# **********************************************************************
#
# Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
#
# This copy of Ice is licensed to you under the terms described in the
# ICE_LICENSE file included in this distribution.
#
# **********************************************************************

top_srcdir	= ..\..\..

!include $(top_srcdir)\config\Make.rules.mak.php

SRCS		= Value.php

all:: $(SRCS)

$(SRCS): $*.ice
	-"$(SLICE2PHP)" $(SLICE2PHPFLAGS) $*.ice

clean::
	del /q $(SRCS)

include .depend.mak
