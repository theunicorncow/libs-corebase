﻿/* CFCharacterSet.c
   
   Copyright (C) 2012 Free Software Foundation, Inc.
   
   Written by: Stefan Bidigaray
   Date: January, 2012
   
   This file is part of the GNUstep CoreBase Library.
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.
   If not, see <http://www.gnu.org/licenses/> or write to the 
   Free Software Foundation, 51 Franklin Street, Fifth Floor, 
   Boston, MA 02110-1301, USA.
*/

#include "CoreFoundation/CFRuntime.h"
#include "CoreFoundation/CFCharacterSet.h"
#include "CoreFoundation/CFDictionary.h"
#include "CoreFoundation/CFString.h"
#include "GSPrivate.h"

#include <unicode/uset.h>

struct __CFCharacterSet
{
  CFRuntimeBase _parent;
  USet         *_uset;
};

static CFTypeID _kCFCharacterSetTypeID = 0;
static CFMutableDictionaryRef _kCFCharacterSetPredefinedSets = NULL;
static GSMutex _kCFCharacterSetLock;

static void
CFCharacterSetFinalize (CFTypeRef cf)
{
  CFCharacterSetRef cs = (CFCharacterSetRef)cf;
  uset_close (cs->_uset);
}

static Boolean
CFCharacterSetEqual (CFTypeRef cf1, CFTypeRef cf2)
{
  return uset_equals (((CFCharacterSetRef)cf1)->_uset,
    ((CFCharacterSetRef)cf2)->_uset);
}

static CFHashCode
CFCharacterSetHash (CFTypeRef cf)
{
  return uset_size (((CFCharacterSetRef)cf)->_uset);
}

static const CFRuntimeClass CFCharacterSetClass =
{
  0,
  "CFCharacterSet",
  NULL,
  (CFTypeRef (*)(CFAllocatorRef, CFTypeRef))CFCharacterSetCreateCopy,
  CFCharacterSetFinalize,
  CFCharacterSetEqual,
  CFCharacterSetHash,
  NULL,
  NULL
};

void CFCharacterSetInitialize (void)
{
  _kCFCharacterSetTypeID = _CFRuntimeRegisterClass (&CFCharacterSetClass);
  GSMutexInitialize (&_kCFCharacterSetLock);
}



CFTypeID
CFCharacterSetGetTypeID (void)
{
  return _kCFCharacterSetTypeID;
}

#define CFCHARACTERSET_SIZE \
  (sizeof(struct __CFCharacterSet) - sizeof(CFRuntimeBase))

CFCharacterSetRef
CFCharacterSetCreateCopy (CFAllocatorRef alloc, CFCharacterSetRef set)
{
  struct __CFCharacterSet *new;
  
  new = (struct __CFCharacterSet*)_CFRuntimeCreateInstance (alloc,
    _kCFCharacterSetTypeID, CFCHARACTERSET_SIZE, 0);
  if (new)
    {
      new->_uset = uset_clone (set->_uset);
      uset_freeze (new->_uset);
    }
  
  return new;
}

CFCharacterSetRef
CFCharacterSetCreateInvertedSet (CFAllocatorRef alloc, CFCharacterSetRef set)
{
  struct __CFCharacterSet *new;
  
  new = (struct __CFCharacterSet*)_CFRuntimeCreateInstance (alloc,
    _kCFCharacterSetTypeID, CFCHARACTERSET_SIZE, 0);
  if (new)
    {
      new->_uset = uset_clone (set->_uset);
      uset_complement (new->_uset);
      uset_freeze (new->_uset);
    }
  
  return new;
}

CFCharacterSetRef
CFCharacterSetCreateWithCharactersInRange (CFAllocatorRef alloc,
  CFRange range)
{
  struct __CFCharacterSet *new;
  
  new = (struct __CFCharacterSet*)_CFRuntimeCreateInstance (alloc,
    _kCFCharacterSetTypeID, CFCHARACTERSET_SIZE, 0);
  if (new)
    {
      new->_uset = uset_open ((UChar32)range.location,
        (UChar32)(range.location + range.length));
      uset_freeze (new->_uset);
    }
  
  return new;
}

static void
USetAddString (USet *set, CFStringRef string)
{
  UniChar *str;
  CFIndex len;
  
  len = CFStringGetLength (string);
  str = CFAllocatorAllocate (NULL, sizeof(UniChar) * len, 0);
  CFStringGetCharacters (string, CFRangeMake(0, len), str);
  
  uset_addString (set, str, len);
  
  CFAllocatorDeallocate (NULL, str);
}

CFCharacterSetRef
CFCharacterSetCreateWithCharactersInString (CFAllocatorRef alloc,
  CFStringRef string)
{
  struct __CFCharacterSet *new;
  
  new = (struct __CFCharacterSet*)_CFRuntimeCreateInstance (alloc,
    _kCFCharacterSetTypeID, CFCHARACTERSET_SIZE, 0);
  if (new)
    {
      new->_uset = uset_openEmpty ();
      USetAddString (new->_uset, string);
      uset_freeze (new->_uset);
    }
  
  return new;
}

CFCharacterSetRef
CFCharacterSetCreateWithBitmapRepresentation (CFAllocatorRef alloc,
  CFDataRef data)
{
  return NULL;
}

CFCharacterSetRef
CFCharacterSetGetPredefined (CFCharacterSetPredefinedSet setIdentifier)
{
  return NULL;
}

CFDataRef
CFCharacterSetCreateBitmapRepresentation (CFAllocatorRef alloc,
  CFCharacterSetRef set)
{
  return NULL;
}

Boolean
CFCharacterSetIsCharacterMember (CFCharacterSetRef set, UniChar c)
{
  return (Boolean)uset_contains (set->_uset, (UChar32)c);
}

Boolean
CFCharacterSetHasMemberInPlane (CFCharacterSetRef set, CFIndex plane)
{
  return false;
}

Boolean
CFCharacterSetIsLongCharacterMember (CFCharacterSetRef set, UTF32Char c)
{
  return (Boolean)uset_contains (set->_uset, (UChar32)c);
}

Boolean
CFCharacterSetIsSupersetOfSet (CFCharacterSetRef set,
  CFCharacterSetRef otherSet)
{
  return uset_containsAll (set->_uset, otherSet->_uset);
}



CFMutableCharacterSetRef
CFCharacterSetCreateMutable (CFAllocatorRef alloc)
{
  struct __CFCharacterSet *new;
  
  new = (struct __CFCharacterSet*)_CFRuntimeCreateInstance (alloc,
    _kCFCharacterSetTypeID, CFCHARACTERSET_SIZE, 0);
  if (new)
    {
      new->_uset = uset_openEmpty ();
    }
  
  return new;
}

CFMutableCharacterSetRef
CFCharacterSetCreateMutableCopy (CFAllocatorRef alloc, CFCharacterSetRef set)
{
  struct __CFCharacterSet *new;
  
  new = (struct __CFCharacterSet*)_CFRuntimeCreateInstance (alloc,
    _kCFCharacterSetTypeID, CFCHARACTERSET_SIZE, 0);
  if (new)
    {
      new->_uset = uset_cloneAsThawed (set->_uset);
    }
  
  return new;
}

void
CFCharacterSetAddCharactersInRange (CFMutableCharacterSetRef set,
  CFRange range)
{
  uset_addRange (set->_uset, (UChar32)range.location,
    (UChar32)(range.location + range.length));
}

void
CFCharacterSetAddCharactersInString (CFMutableCharacterSetRef set,
  CFStringRef string)
{
  USetAddString (set->_uset, string);
}

void
CFCharacterSetRemoveCharactersInRange (CFMutableCharacterSetRef set,
  CFRange range)
{
  uset_removeRange (set->_uset, (UChar32)range.location,
    (UChar32)(range.location + range.length));
}

void
CFCharacterSetRemoveCharactersInString (CFMutableCharacterSetRef set,
  CFStringRef string)
{
  UniChar *str;
  CFIndex len;
  
  len = CFStringGetLength (string);
  str = CFAllocatorAllocate (NULL, sizeof(UniChar) * len, 0);
  CFStringGetCharacters (string, CFRangeMake(0, len), str);
  
  uset_removeString (set->_uset, str, len);
  
  CFAllocatorDeallocate (NULL, str);
}

void
CFCharacterSetIntersect (CFMutableCharacterSetRef set,
  CFCharacterSetRef otherSet)
{
  uset_retainAll (set->_uset, otherSet->_uset);
}

void
CFCharacterSetInvert (CFMutableCharacterSetRef set)
{
  uset_complement (set->_uset);
}

void
CFCharacterSetUnion (CFMutableCharacterSetRef set, CFCharacterSetRef otherSet)
{
  uset_addAll (set->_uset, otherSet->_uset);
}