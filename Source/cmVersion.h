/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile: cmVersion.h,v $
  Language:  C++
  Date:      $Date: 2005-06-17 13:49:06 $
  Version:   $Revision: 1.2 $

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef cmVersion_h
#define cmVersion_h

#include "cmStandardIncludes.h"

/** \class cmVersion
 * \brief Helper class for providing CMake and CTest version information.
 *
 * Finds all version related information.
 */
class cmVersion
{
public:
  /**
   * Return major and minor version numbers for cmake.
   */
  static unsigned int GetMajorVersion() { return CMake_VERSION_MAJOR; }
  static unsigned int GetMinorVersion() { return CMake_VERSION_MINOR; }
  static unsigned int GetPatchVersion() { return CMake_VERSION_PATCH; }
  static std::string GetReleaseVersion();
  static std::string GetCMakeVersion();
};

#endif

