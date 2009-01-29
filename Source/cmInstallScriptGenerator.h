/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile: cmInstallScriptGenerator.h,v $
  Language:  C++
  Date:      $Date: 2007-10-15 11:08:10 $
  Version:   $Revision: 1.3 $

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef cmInstallScriptGenerator_h
#define cmInstallScriptGenerator_h

#include "cmInstallGenerator.h"

/** \class cmInstallScriptGenerator
 * \brief Generate target installation rules.
 */
class cmInstallScriptGenerator: public cmInstallGenerator
{
public:
  cmInstallScriptGenerator(const char* script, bool code,
    const char* component);
  virtual ~cmInstallScriptGenerator();

protected:
  virtual void GenerateScript(std::ostream& os);
  std::string Script;
  bool Code;
};

#endif
