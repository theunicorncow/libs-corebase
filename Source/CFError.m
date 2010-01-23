/* CFError.m
   
   Copyright (C) 2010 Free Software Foundation, Inc.
   
   Written by: Stefan Bidigaray
   Date: January, 2010
   
   This file is part of GNUstep CoreBase Library.
   
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

#import <Foundation/NSString.h> // For NSConstantString
#import <Foundation/NSError.h>

#include "CoreFoundation/CFError.h"

const CFStringRef kCFErrorDomainPOSIX = 
  (CFStringRef)@"NSPOSIXErrorDomain";
const CFStringRef kCFErrorDomainOSStatus = 
  (CFStringRef)@"NSOSStatusErrorDomain";
const CFStringRef kCFErrorDomainMach = 
  (CFStringRef)@"NSMACHErrorDomain";
const CFStringRef kCFErrorDomainCocoa = 
  (CFStringRef)@"NSCocoaErrorDomain";

const CFStringRef kCFErrorLocalizedDescriptionKey = 
  (CFStringRef)@"NSLocalizedDescriptionKey";
const CFStringRef kCFErrorLocalizedFailureReasonKey = 
  (CFStringRef)@"NSLocalizedFailureReasonErrorKey";
const CFStringRef kCFErrorLocalizedRecoverySuggestionKey = 
  (CFStringRef)@"NSLocalizedRecoverySuggestionErrorKey";
const CFStringRef kCFErrorDescriptionKey = 
  (CFStringRef)@"kCFErrorDescriptionKey"; // FIXME
const CFStringRef kCFErrorUnderlyingErrorKey = 
  (CFStringRef)@"NSUnderlyingErrorKey";



CFStringRef
CFErrorCopyDescription (CFErrorRef err)
{
  return CFRetain([(NSError *)err localizedDescription]);
}

CFStringRef
CFErrorCopyFailureReason (CFErrorRef err)
{
  return CFRetain([(NSError *)err localizedFailureReason]);
}

CFStringRef
CFErrorCopyRecoverySuggestion (CFErrorRef err)
{
  return CFRetain([(NSError *)err localizedRecoverySuggestion]);
}

CFDictionaryRef
CFErrorCopyUserInfo (CFErrorRef err)
{
  return CFRetain([(NSError *)err userInfo]);
}

CFErrorRef
CFErrorCreate (CFAllocatorRef allocator, CFStringRef domain,
               CFIndex code, CFDictionaryRef userInfo)
{
  return (CFErrorRef)[[NSError allocWithZone: (NSZone *)allocator]
                       initWithDomain: (NSString *)domain
                                 code: code
                             userInfo: (NSDictionary *)userInfo];
}

CFErrorRef
CFErrorCreateWithUserInfoKeysAndValues (CFAllocatorRef allocator,
                                        CFStringRef domain, CFIndex code,
                                        const void *const *userInfoKeys,
                                        const void *const *userInfoValues,
                                        CFIndex numUserInfoValues)
{
  CFDictionaryRef userInfo = CFDictionaryCreate (allocator, 
    (const void **)userInfoKeys, (const void **)userInfoValues, 
    numUserInfoValues, NULL, NULL);
  CFErrorRef ret = CFErrorCreate (allocator, domain, code, userInfo);
  
  CFRelease (userInfo);
  return ret;
}

CFIndex
CFErrorGetCode (CFErrorRef err)
{
  return [(NSError *)err code];
}

CFStringRef
CFErrorGetDomain (CFErrorRef err)
{
  return (CFStringRef)[(NSError *)err domain];
}

CFTypeID
CFErrorGetTypeID (void)
{
  return (CFTypeID)[NSError class];
}