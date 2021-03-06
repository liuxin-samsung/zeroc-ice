// **********************************************************************
//
// Copyright (c) 2003-2011 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

namespace Ice
{
    using System.Collections;
    using System.Collections.Generic;
    using System.Threading;

    //
    // The base class for all ImplicitContext implementations
    //
    public abstract class ImplicitContextI : ImplicitContext
    {
        public static ImplicitContextI create(string kind)
        {
            if(kind.Equals("None") || kind.Length == 0)
            {
                return null;
            }
            else if(kind.Equals("Shared"))
            {
                return new SharedImplicitContext();
            }
            else if(kind.Equals("PerThread"))
            {
                return new PerThreadImplicitContext();
            }
            else
            {
                throw new Ice.InitializationException(
                    "'" + kind + "' is not a valid value for Ice.ImplicitContext"); 
            }
        }
        
        public abstract Dictionary<string, string> getContext();
        public abstract void setContext(Dictionary<string, string> newContext);
        public abstract bool containsKey(string key);    
        public abstract string get(string key);
        public abstract string put(string key, string value);
        public abstract string remove(string key);

        abstract public void write(Dictionary<string, string> prxContext,
                                   IceInternal.BasicStream os);
        abstract internal Dictionary<string, string> combine(Dictionary<string, string> prxContext);
    }
        
        
    internal class SharedImplicitContext : ImplicitContextI
    {
        public override Dictionary<string, string> getContext()
        {
            lock(this)
            {
                return new Dictionary<string, string>(_context);
            }
        }
            
        public override void setContext(Dictionary<string, string> context)
        {
            lock(this)
            {
                if(context != null && context.Count != 0)
                {
                    _context = new Dictionary<string, string>(context);
                }
                else
                {
                    _context.Clear();
                }
            }
        }
            
        public override bool containsKey(string key)
        {
            lock(this)
            {
                if(key == null)
                {
                    key = "";
                }
                
                return _context.ContainsKey(key);
            }
        }

        public override string get(string key)
        {
            lock(this)
            {
                if(key == null)
                {
                    key = "";
                }
                
                string val = _context[key];
                if(val == null)
                {
                    val = "";
                }
                return val;
            }
        }
            
            
        public override string put(string key, string value)
        {
            lock(this)
            {
                if(key == null)
                {
                    key = "";
                }
                if(value == null)
                {
                    value = "";
                }

                string oldVal;
                _context.TryGetValue(key, out oldVal);
                if(oldVal == null)
                {
                    oldVal = "";
                }
                _context[key] = value;
                
                return oldVal;
            }
        }
            
        public override string remove(string key)
        {
            lock(this)
            {
                if(key == null)
                {
                    key = "";
                }

                string val = _context[key];

                if(val == null)
                {
                    val = "";
                }
                else
                {
                    _context.Remove(key);
                }

                return val;
            }
        }
            
        public override void write(Dictionary<string, string> prxContext, IceInternal.BasicStream os)
        {
            if(prxContext.Count == 0)
            {
                lock(this)
                {
                    ContextHelper.write(os, _context);
                }
            }
            else 
            {
                Dictionary<string, string> ctx = null;
                lock(this)
                {
                    ctx = _context.Count == 0 ? prxContext :combine(prxContext); 
                }
                ContextHelper.write(os, ctx);
            }
        }

        internal override Dictionary<string, string> combine(Dictionary<string, string> prxContext)
        {
            lock(this)
            {
                Dictionary<string, string> combined = new Dictionary<string, string>(prxContext);
                foreach(KeyValuePair<string, string> e in _context)
                {
                    try
                    {
                        combined.Add(e.Key, e.Value);
                    }
                    catch(System.ArgumentException)
                    {
                        // Ignore.
                    }
                }
                return combined;
            }
        }

        private Dictionary<string, string> _context = new Dictionary<string, string>();
    }

    internal class PerThreadImplicitContext : ImplicitContextI
    {
        public override Dictionary<string, string> getContext()
        {
            Dictionary<string, string> threadContext = null;
            lock(this)
            {
                threadContext = (Dictionary<string, string>)_map[Thread.CurrentThread];
            }

            if(threadContext == null)
            {
                threadContext = new Dictionary<string, string>();
            }
            return threadContext;
        }

        public override void setContext(Dictionary<string, string> context)
        {
            if(context == null || context.Count == 0)
            {
                lock(this)
                {
                    _map.Remove(Thread.CurrentThread);
                }
            }
            else
            {
                Dictionary<string, string> threadContext = new Dictionary<string, string>(context);
                
                lock(this)
                {
                    _map.Add(Thread.CurrentThread, threadContext);
                }
            }
        }

        public override bool containsKey(string key)
        {
            if(key == null)
            {
                key = "";
            }

            Dictionary<string, string> threadContext = null;
            lock(this)
            {
                threadContext = (Dictionary<string, string>)_map[Thread.CurrentThread];
            }

            if(threadContext == null)
            {
                return false;
            }

            return threadContext.ContainsKey(key);
        }

        public override string get(string key)
        {
            if(key == null)
            {
                key = "";
            }

            Dictionary<string, string> threadContext = null;
            lock(this)
            {
                threadContext = (Dictionary<string, string>)_map[Thread.CurrentThread];
            }

            if(threadContext == null)
            {
                return "";
            }
            string val = threadContext[key];
            if(val == null)
            {
                val = "";
            }
            return val;
        }

        public override string put(string key, string value)
        {
            if(key == null)
            {
                key = "";
            }
            if(value == null)
            {
                value = "";
            }

            Thread currentThread = Thread.CurrentThread;
            
            Dictionary<string, string> threadContext = null;
            lock(this)
            {
                threadContext = (Dictionary<string, string>)_map[currentThread];
            }
           
            if(threadContext == null)
            {
                threadContext = new Dictionary<string, string>();
                lock(this)
                {
                    _map.Add(currentThread, threadContext);
                }
            }
            
            string oldVal;
            threadContext.TryGetValue(key, out oldVal);
            if(oldVal == null)
            {
                oldVal = "";
            }

            threadContext[key] = value;
            return oldVal;
        }

        public override string remove(string key)
        {
            if(key == null)
            {
                key = "";
            }

            Dictionary<string, string> threadContext = null;
            lock(this)
            {
                threadContext = (Dictionary<string, string>)_map[Thread.CurrentThread];
            }

            if(threadContext == null)
            {
                return "";
            }

            string val = threadContext[key];

            if(val == null)
            {
                val = "";
            }
            else
            {
                threadContext.Remove(key);
            }
            return val;
        }

        public override void write(Dictionary<string, string> prxContext,
                                   IceInternal.BasicStream os)
        {
            Dictionary<string, string> threadContext = null;
            lock(this)
            {
                threadContext = (Dictionary<string, string>)_map[Thread.CurrentThread];
            }
            
            if(threadContext == null || threadContext.Count == 0)
            {
                ContextHelper.write(os, prxContext);
            }
            else if(prxContext.Count == 0)
            {
                ContextHelper.write(os, threadContext);
            }
            else
            {
                Dictionary<string, string> combined = new Dictionary<string, string>(prxContext);
                foreach(KeyValuePair<string, string> e in threadContext)
                {
                    try
                    {
                        combined.Add(e.Key, e.Value);
                    }
                    catch(System.ArgumentException)
                    {
                        // Ignore.
                    }
                }
                ContextHelper.write(os, combined);
            }
        }

        internal override Dictionary<string, string> combine(Dictionary<string, string> prxContext)
        {
            Dictionary<string, string> threadContext = null;
            lock(this)
            {
                threadContext = (Dictionary<string, string>)_map[Thread.CurrentThread];
            }

            Dictionary<string, string> combined = new Dictionary<string, string>(prxContext);
            foreach(KeyValuePair<string, string> e in threadContext)
            {
                combined.Add(e.Key, e.Value);
            }
            return combined;
        }

        //
        //  map Thread -> Context
        //
        private Hashtable _map = new Hashtable();
    } 
}


