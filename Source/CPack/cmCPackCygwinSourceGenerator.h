/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile: cmCPackCygwinSourceGenerator.h,v $
  Language:  C++
  Date:      $Date: 2007-10-31 12:50:17 $
  Version:   $Revision: 1.3 $

  Copyright (c) 2002 Kitware, Inc. All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef cmCPackCygwinSourceGenerator_h
#define cmCPackCygwinSourceGenerator_h

#include "cmCPackTarBZip2Generator.h"

/** \class cmCPackCygwinSourceGenerator
 * \brief A generator for cygwin source files
 */
class cmCPackCygwinSourceGenerator : public cmCPackTarBZip2Generator
{
public:
  cmCPackTypeMacro(cmCPackCygwinSourceGenerator, cmCPackTarBZip2Generator);

  /**
   * Construct generator
   */
  cmCPackCygwinSourceGenerator();
  virtual ~cmCPackCygwinSourceGenerator();
protected:
  const char* GetPackagingInstallPrefix();
  virtual int InitializeInternal();
  int CompressFiles(const char* outFileName, const char* toplevel,
    const std::vector<std::string>& files);
  virtual const char* GetOutputExtension();
  std::string InstallPrefix;
  std::string OutputExtension;
};

#endif
