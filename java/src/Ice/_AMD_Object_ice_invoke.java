// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

package Ice;

final class _AMD_Object_ice_invoke extends IceInternal.IncomingAsync implements AMD_Object_ice_invoke
{
    public
    _AMD_Object_ice_invoke(IceInternal.Incoming in)
    {
        super(in);
    }

    public void
    ice_response(boolean ok, byte[] outParams)
    {
        if(__validateResponse(ok))
        {
            try
            {
                __os().writeBlob(outParams);
            }
            catch(Ice.LocalException ex)
            {
                __exception(ex);
                return;
            }
            __response(ok);
        }
    }
}
