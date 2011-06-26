/* CFUUID.c
   
   Copyright (C) 2011 Free Software Foundation, Inc.
   
   Written by: Stefan Bidigaray
   Date: May, 2011
   
   This file is part of GNUstep CoreBase Library.
   
   This library is free software; you can redisibute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is disibuted in the hope that it will be useful,
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
#include "CoreFoundation/CFBase.h"
#include "CoreFoundation/CFString.h"
#include "CoreFoundation/CFUUID.h"

/* Some of the code in CFUUID is based on Etoile's ETUUID class.
   Copyright (C) 2007 Yen-Ju Check <yjchenx AT gmail DOT com> */

#include <stdlib.h>
#include <unistd.h>
// On *BSD and Linux we have a srandomdev() function which seeds the random 
// number generator with entropy collected from a variety of sources. On other
// platforms we don't, so we use some less random data based on the current 
// time and pid to seed the random number generator.
#if defined(__FreeBSD__) \
    || defined(__OpenBSD) \
    || defined(__DragonFly__) \
    || defined(__APPLE__)
  #define INITRANDOM() srandomdev()
#else
  #include <sys/time.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <errno.h>
  #define INITRANDOM() CFsrandomdev()
    #if defined(__linux__)
static void CFsrandomdev(void)
{
	int fd;
	unsigned int seed = 0;
	size_t len = sizeof(seed);
	BOOL hasSeed = NO;
  
	fd = open("/dev/random", O_RDONLY | O_NONBLOCK, 0);
	if (fd >= 0) 
    {
      if (errno != EWOULDBLOCK)
        {
          if (read(fd, &seed, len) == (ssize_t)len)
            hasSeed = YES;
        }
      close(fd);
    }
  
	if (hasSeed == NO) 
    {
      struct timeval tv;
      unsigned long junk;
      
      gettimeofday(&tv, NULL);
      seed = ((getpid() << 16) ^ tv.tv_sec ^ tv.tv_usec ^ junk);
    }
	
	srandom(seed);
}
    #else
static void CFsrandomdev(void)
{
	struct timeval tv;
	unsigned long junk;
	unsigned int seed = 0;
  
	/* Within a process, junk is always initialized to the same value (on Linux),
	   gettimeofday is microsecond-based and pid is fixed. This leads to many
	   collisions if you call ETSRandomDev() in a loop, as -testString does
	   in TestUUID.m. */
	gettimeofday(&tv, NULL);
	seed = ((getpid() << 16) ^ tv.tv_sec ^ tv.tv_usec ^ junk);
  
	srandom(seed);
}
    #endif
#endif

static CFTypeID _kCFUUIDTypeID = 0;

struct __CFUUID
{
  CFRuntimeBase _parent;
  CFUUIDBytes   _bytes;
};

static Boolean
CFUUIDEqual (CFTypeRef cf1, CFTypeRef cf2)
{
  CFUUIDBytes bytes1 = ((CFUUIDRef)cf1)->_bytes;
  CFUUIDBytes bytes2 = ((CFUUIDRef)cf2)->_bytes;
  
  if (bytes1.byte0 == bytes2.byte0
      && bytes1.byte1 == bytes2.byte1
      && bytes1.byte2 == bytes2.byte2
      && bytes1.byte3 == bytes2.byte3
      && bytes1.byte4 == bytes2.byte4
      && bytes1.byte5 == bytes2.byte5
      && bytes1.byte6 == bytes2.byte6
      && bytes1.byte7 == bytes2.byte7
      && bytes1.byte8 == bytes2.byte8
      && bytes1.byte9 == bytes2.byte9
      && bytes1.byte10 == bytes2.byte10
      && bytes1.byte11 == bytes2.byte11
      && bytes1.byte12 == bytes2.byte12
      && bytes1.byte13 == bytes2.byte13
      && bytes1.byte14 == bytes2.byte14
      && bytes1.byte15 == bytes2.byte15)
    return true;
  
  return false;
}

static CFStringRef
CFUUIDCopyFormattingDescription (CFTypeRef cf, CFDictionaryRef formatOptions)
{
  return CFUUIDCreateString (CFAllocatorGetDefault(), cf);
}

const CFRuntimeClass CFUUIDClass =
{
  0,
  "CFUUID",
  NULL, // init
  NULL, // copy
  NULL, // dealloc
  CFUUIDEqual, // equal
  NULL, // hash
  CFUUIDCopyFormattingDescription, // copyFormattingDesc
  NULL  // copyDebugDesc
};

void CFUUIDInitialize (void)
{
  INITRANDOM();
  _kCFUUIDTypeID = _CFRuntimeRegisterClass (&CFUUIDClass);
}



CFUUIDRef
CFUUIDCreate (CFAllocatorRef alloc)
{
  CFIndex i;
  CFUUIDBytes bytes;
  
  for (i = 0 ; i < 16 ; ++i)
    {
      long r = random ();
      ((UInt8*)&bytes)[i] = (UInt8)r;
    }
  // Set version
  bytes.byte6 &= 0x0F;
  bytes.byte6 |= 0x40;
  // Bit 6 must be 0 and 7 must be 1
  bytes.byte8 &= 0xBF;
  
  return CFUUIDCreateFromUUIDBytes (alloc, bytes);
}

CFUUIDRef
CFUUIDCreateFromString (CFAllocatorRef alloc, CFStringRef uuidStr)
{
  CFUUIDBytes bytes;
  char data[36]; // 36 chars
  if (!CFStringGetCString (uuidStr, data, 36, kCFStringEncodingASCII))
	sscanf (data, "%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-"
  "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
    &bytes.byte0, &bytes.byte1, &bytes.byte2, &bytes.byte3,
    &bytes.byte4, &bytes.byte5, &bytes.byte6, &bytes.byte7,
    &bytes.byte8, &bytes.byte9, &bytes.byte10, &bytes.byte11,
    &bytes.byte12, &bytes.byte13, &bytes.byte14, &bytes.byte15);
  
  return CFUUIDCreateFromUUIDBytes (alloc, bytes);
}

CFUUIDRef
CFUUIDCreateFromUUIDBytes (CFAllocatorRef alloc, CFUUIDBytes bytes)
{
  struct __CFUUID *new;
  
  new = (struct __CFUUID *)_CFRuntimeCreateInstance (alloc, _kCFUUIDTypeID,
    sizeof(struct __CFUUID) - sizeof(CFRuntimeBase), NULL);
  if (new)
    new->_bytes = bytes;
  
  return (CFUUIDRef)new;
}

CFUUIDRef
CFUUIDCreateWithBytes (CFAllocatorRef alloc, UInt8 byte0, UInt8 byte1,
  UInt8 byte2, UInt8 byte3, UInt8 byte4, UInt8 byte5, UInt8 byte6, UInt8 byte7,
  UInt8 byte8, UInt8 byte9, UInt8 byte10, UInt8 byte11, UInt8 byte12,
  UInt8 byte13, UInt8 byte14, UInt8 byte15)
{
  CFUUIDBytes uuidBytes;
  uuidBytes.byte0 = byte0;
  uuidBytes.byte1 = byte1;
  uuidBytes.byte2 = byte2;
  uuidBytes.byte3 = byte3;
  uuidBytes.byte4 = byte4;
  uuidBytes.byte5 = byte5;
  uuidBytes.byte6 = byte6;
  uuidBytes.byte7 = byte7;
  uuidBytes.byte8 = byte8;
  uuidBytes.byte9 = byte9;
  uuidBytes.byte10 = byte10;
  uuidBytes.byte11 = byte11;
  uuidBytes.byte12 = byte12;
  uuidBytes.byte13 = byte13;
  uuidBytes.byte14 = byte14;
  uuidBytes.byte15 = byte15;
  
  return CFUUIDCreateFromUUIDBytes (alloc, uuidBytes);
}

CFStringRef
CFUUIDCreateString (CFAllocatorRef alloc, CFUUIDRef uuid)
{
  CFStringRef ret;
  CFUUIDBytes bytes = uuid->_bytes;
  
  ret = CFStringCreateWithFormat (alloc, NULL, CFSTR("%2hhx%2hhx%2hhx%2hhx-"
    "%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx"),
      bytes.byte0, bytes.byte1, bytes.byte2, bytes.byte3,
      bytes.byte4, bytes.byte5, bytes.byte6, bytes.byte7,
      bytes.byte8, bytes.byte9, bytes.byte10, bytes.byte11,
      bytes.byte12, bytes.byte13, bytes.byte14, bytes.byte15);
  
  return ret;
}

CFUUIDRef
CFUUIDGetConstantUUIDWithBytes (CFAllocatorRef alloc, UInt8 byte0, UInt8 byte1,
  UInt8 byte2, UInt8 byte3, UInt8 byte4, UInt8 byte5, UInt8 byte6, UInt8 byte7,
  UInt8 byte8, UInt8 byte9, UInt8 byte10, UInt8 byte11, UInt8 byte12,
  UInt8 byte13, UInt8 byte14, UInt8 byte15)
{
  return NULL; // FIXME
}

CFUUIDBytes
CFUUIDGetUUIDBytes (CFUUIDRef uuid)
{
  return uuid->_bytes;
}

CFTypeID
CFUUIDGetTypeID (void)
{
  return _kCFUUIDTypeID;
}