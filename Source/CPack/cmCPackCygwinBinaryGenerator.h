/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile: cmCPackCygwinBinaryGenerator.h,v $
  Language:  C++
  Date:      $Date: 2007-10-31 12:50:17 $
  Version:   $Revision: 1.3 $

  Copyright (c) 2002 Kitware, Inc. All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef cmCPackCygwinBinaryGenerator_h
#define cmCPackCygwinBinaryGenerator_h

#include "cmCPackTarBZip2Generator.h"

/** \class cmCPackCygwinBinaryGenerator
 * \brief A generator for TarBZip2 files
 */
class cmCPackCygwinBinaryGenerator : public cmCPackTarBZip2Generator
{
public:
  cmCPackTypeMacro(cmCPackCygwinBinaryGenerator, cmCPackTarBZip2Generator);

  /**
   * Construct generator
   */
  cmCPackCygwinBinaryGenerator();
  virtual ~cmCPackCygwinBinaryGenerator();
protected:
  virtual int InitializeInternal();
  int CompressFiles(const char* outFileName, const char* toplevel,
    const std::vector<std::string>& files);
  virtual const char* GetOutputExtension();
  std::string OutputExtension;
};

#endif
