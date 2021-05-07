/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
module tbrpgsca.play;

import tbrpgsca.actor;
import tbrpgsca.ability;
import tbrpgsca.costume;
import tbrpgsca.ailment;
import tbrpgsca.scene;

import core.stdc.stdlib;
import core.stdc.stdint;
import std.container.array : Array;
/*
If the COMPRESS_POINTERS enum is set to a non-zero value, 64bit pointers will be compressed into 32bit integers, according to the following options:
    +3 can compress addresses up to 16GB, at the expense of the 3 lower tag bits, which can no longer be used for other purporses
    +2 can compress addresses up to 8GB, at the expense of the 2 lower tag bits, which can no longer be used for other purporses
    +1 can compress addresses up to 4GB, at the expense of the lower tag bit, which can no longer be used for other purporses
Attempting to compress an address higher than the mentioned limits, will lead however to increased CPU and RAM usage and cannot be shared between threads;
The following negative values can also be used, but they are not safe and will lead to crashes, when the memory limits are exceeded:
    -4 can compress addresses up to 32GB, at the expense of the 3 lower tag bits, which can no longer be used for other purporses
    -3 can compress addresses up to 16GB, at the expense of the 2 lower tag bits, which can no longer be used for other purporses
    -2 can compress addresses up to 8GB, at the expense of the lower tag bit, which can no longer be used for other purporses
    -1 can compress addresses up to 4GB, leaving the 3 lower tag bits to be used for other purporses
*/
enum COMPRESS_POINTERS = (void*).sizeof < 8 ? 0 : -4;
enum USE_BITFIELDS = true;

enum nil = null;
alias Bit = bool;
alias Idx = ubyte;
alias Dec = float;
alias Vct = Array;
alias Txt = char*;
alias Chr = char;
alias Nr = int;

struct CmpsPtr(T, const int own = 0, const int cmpsType = COMPRESS_POINTERS)
{
    static if (own != 0)
    {
        pragma(inline, true):
        protected void clean()
        {
            this.ptr.erase;
            //T* ptr = this.ptr;
            //(&(ptr)).clear;
            static if (cmpsType > 0)
            {
                clearList(this._ptr);
            }
            //GC.removeRange(ptr);
        }
    }
    
    static if (own > 0)
    {
        protected CmpsPtr!(size_t, 0, cmpsType == 0 ? 0 : (cmpsType > 0 ? 3 : -4)) count = void;

        pragma(inline, true):
        protected void reset()
        {
            size_t* countPtr = alloc!size_t;
            (*countPtr) = 0;
            this.count.ptr(countPtr);
        }
        
        pragma(inline, true):
        protected void increase()
        {
            auto cntPtr = this.count.ptr;
            if (cntPtr)
            {
                (*cntPtr) += 1;
            }
        }

        pragma(inline, true):
        protected void decrease()
        {
            auto count = &this.count;
            auto cntPtr = count.ptr;
            if (cntPtr)
            {
                auto cntNr = *cntPtr;
                if (cntNr == 1)
                {
                    this.clean;
                    count.ptr = nil;
                    cntPtr.free;
                }
                else
                {
                    (*cntPtr) = cntNr - 1;
                }
            }
        }
    }
    
    static if (cmpsType == 0)
    {
        public T* ptr = void;
    }
    else static if (cmpsType > 0)
    {
        enum SHIFT_LEN = cmpsType > 2 ? 2 : (cmpsType - 1);
        private static Vct!(void*) _ptrList; //TODO: multiple thread shared access safety and analyze better solutions
        private uint _ptr = 0U;

        private static bool clearList(uint ptr)
        {
            if (ptr == 0U)
            {
                return false;
            }
            auto ptrList = &this._ptrList;
            if ((ptr & 1U) == 1U)
            {
                ptr >>>= 1;
                if (ptr == ptrList.length)
                {
                    size_t ptrListLen = void;
                    do
                    {
                        ptrList.removeBack();
                    }
                    while ((ptrListLen = ptrList.length) > 0 && (--ptr) == ptrListLen && (*ptrList)[ptr - 1U] == nil);
                }
                else
                {
                    (*ptrList)[ptr - 1U] = nil;
                }
            }
            return true;
        }

        pragma(inline, true):
        public bool compressed()
        {
            return (this._ptr & 1U) != 1U;
        }

        pragma(inline, true):
        public T* ptr()
        {
            uint ptr = this._ptr;
            if (ptr == 0U)
            {
                return nil;
            }
            //uintptr_t ptrNr = void;
            else if ((ptr & 1U) == 1U)
            {
                return cast(T*)this._ptrList[(ptr >>> 1) - 1U];
            }
            else
            {
                //ptrNr = (cast(uintptr_t)ptr) << SHIFT_LEN;
                return cast(T*)((cast(uintptr_t)ptr) << SHIFT_LEN);
            }
            //return cast(T*)((ptrNr & ((1UL << 48) - 1UL)) | ~((ptrNr & (1UL << 47)) - 1UL));
        }

        private void listPtr(T* ptr)
        {
            uint oldPtr = this._ptr;
            auto ptrList = &this._ptrList;
            if ((oldPtr & 1U) == 1U)
            {
                oldPtr >>>= 1;
                if (oldPtr > 0U)
                {
                    (*ptrList)[oldPtr - 1U] = ptr;
                    return;
                }
            }
            ulong ptrLength = ptrList.length;
            for (uint i = 0; i < ptrLength; i += 1U)
            {
                if ((*ptrList)[i] == nil)
                {
                    (*ptrList)[i] = ptr;
                    this._ptr = (i << 1U) | 1U;
                    return;
                }
            }
            ptrList.insert(ptr);
            this._ptr = cast(uint)(((ptrLength + 1) << 1) | 1);
        }

        pragma(inline, true):
        public void ptr(T* ptr)
        {
            static if (own == 0)
            {
                if (this.ptr == ptr)
                {
                    return;
                }
                if (ptr == nil)
                {
                    if (clearList(this._ptr)) this._ptr = 0U;
                    return;
                }
            }
            else
            {
                if (ptr && ptr != this.ptr)
                {
                    static if (own > 0)
                    {
                        this.decrease;
                        this.reset;
                    }
                    else
                    {
                        this.clean;
                    }
                }
                else
                {
                    return;
                }
            }
            uintptr_t ptrNr = cast(uintptr_t)ptr; //<< 16) >>> 16;
            if (ptrNr < (4294967296UL << SHIFT_LEN))
            {
                static if (own == 0)
                {
                    clearList(this._ptr);
                }
                this._ptr = cast(uint)(ptrNr >>> SHIFT_LEN);
            }
            else
            {
                this.listPtr(ptr);
            }
        }
        /*pragma(inline, true):
        this(this)
        {
            T* ptr = this.ptr;
            this._ptr = 0U;
            this.ptr = ptr;
        }*/        

        static if (own < 0)
        {
            pragma(inline, true):
            ~this()
            {
                this.clean;
            }
        }
        else
        {
            pragma(inline, true):
            ~this()
            {
                static if (own == 0)
                {
                    //this._ptr.clearList;
                    clearList(this._ptr);
                }
                else
                {
                    this.decrease;
                }
            }
        }
    }
    else
    {
        private uint _ptr = void;
     
        enum SHIFT_LEN = cmpsType < -3 ? 3 : -(cmpsType + 1);

        pragma(inline, true):
        public bool compressed()
        {
            return true;
        }
        
        pragma(inline, true):
        public T* ptr()
        {
            /*uint ptr = this._ptr;
            if (ptr == 0U)
            {
                return nil;
            }
            else
            {
                uintptr_t ptrNr = (cast(uintptr_t)ptr) << SHIFT_LEN;
                return cast(T*)((ptrNr & ((1UL << 48) - 1UL)) | ~((ptrNr & (1UL << 47)) - 1UL));
            }*/
            return cast(T*)((cast(uintptr_t)this._ptr) << SHIFT_LEN);
        }
        
        pragma(inline, true):
        public void ptr(const T* ptr)
        {
            /*if (ptr == nil)
            {
                this._ptr = 0U;
            }
            else
            {*/
                static if (own > 0)
                {
                    if (ptr != this.ptr)
                    {
                        this.decrease;
                        this.reset;
                    }
                }
                uintptr_t ptrNr = cast(uintptr_t)ptr;
                //uintptr_t ptrNr = (cast(uintptr_t)ptr << 16) >>> 16;
                assert(ptrNr < (4294967296UL << SHIFT_LEN)); //TODO: analyze alternative solutions
                this._ptr = cast(uint)(ptrNr >>> SHIFT_LEN);
            //}
        }

        static if (own < 0)
        {
            pragma(inline, true):
            ~this()
            {
                this.clean;
            }
        }
        else static if (own > 0)
        {
            pragma(inline, true):
            ~this()
            {
                this.decrease;
            }
        }
    }
    
    alias ptr this;

    static if (own > 0)
    {
        @disable this(this);
        
        pragma(inline, true):
        this(ref return scope CmpsPtr copy)
        {
            //this.count = nil;
            this.ptr = copy.ptr;
            static if (own > 0)
            {
                this.count = copy.count;
                this.increase;
            }
        }
    }
    else static if (own < 0)
    {
        @disable this(this);
        
        @disable this(ref return scope CmpsPtr copy);
    }
            
    pragma(inline, true):
    this(T* ptr)
    {
        static if (own > 0)
        {
            this.count = nil;
        }
        this.ptr = ptr;
    }
}

pragma(inline, true):
T* alloc(T)(const Nr qty = 1)
{
    return cast(T*)malloc(T.sizeof * qty);
}

pragma(inline, true):
void clear(T, const bool check = false)(T** ptr)
{
    static if (check)
    {
        if (ptr == nil) return;
    }
    (*ptr).erase!(T, check);
    (*ptr) = nil;
}

pragma(inline, true):
void clear(T, const bool check = false)(CmpsPtr!T ptr)
{
    ptr.erase!(T, check);
    ptr.ptr = nil;
}

pragma(inline, true):
void erase(T, const bool check = false)(T* ptr)
{
    static if (check)
    {
        if (ptr == nil) return;
    }
    (*ptr).destroy;
    ptr.free;
}

pragma(inline, true):
void erase(T, const bool check = false)(CmpsPtr!T ptr)
{
    ptr.ptr.erase!(T, check);
}

alias Ptr = CmpsPtr;

enum Element : Nr
{
    spirit = 1,
    wisdom = 2,
    agility = 4,
    strength = 8,
    armor = 16,
    water = 32,
    stone = 64,
    fire = 128,
    bolt = 256,
    wind = 512,
    ice = 1024,
    psychic = 2048,
    light = 4096
}
