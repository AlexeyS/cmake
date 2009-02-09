/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile: cmTargetLinkLibrariesCommand.cxx,v $
  Language:  C++
  Date:      $Date: 2008-09-03 13:43:18 $
  Version:   $Revision: 1.25.2.1 $

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "cmTargetLinkLibrariesCommand.h"

const char* cmTargetLinkLibrariesCommand::LinkLibraryTypeNames[4] =
{
  "general",
  "debug",
  "optimized",
  "import"
};

// cmTargetLinkLibrariesCommand
bool cmTargetLinkLibrariesCommand
::InitialPass(std::vector<std::string> const& args, cmExecutionStatus &)
{
  // must have one argument
  if(args.size() < 1)
    {
    this->SetError("called with incorrect number of arguments");
    return false;
    }

  // but we might not have any libs after variable expansion
  if(args.size() < 2)
    {
    return true;
    }

  // Lookup the target for which libraries are specified.
  this->Target =
    this->Makefile->GetCMakeInstance()
    ->GetGlobalGenerator()->FindTarget(0, args[0].c_str());
  if(!this->Target)
    {
    cmOStringStream e;
    e << "Cannot specify link libraries for target \"" << args[0] << "\" "
      << "which is not built by this project.";
    this->Makefile->IssueMessage(cmake::FATAL_ERROR, e.str());
    cmSystemTools::SetFatalErrorOccured();
    return true;
    }

  // Keep track of link configuration specifiers.
  cmTarget::LinkLibraryType llt = cmTarget::GENERAL;
  bool haveLLT = false;

  // Start with primary linking and switch to link interface
  // specification when the keyword is encountered.
  this->DoingInterface = false;

  // add libraries, nothe that there is an optional prefix 
  // of debug and optimized than can be used
  for(unsigned int i=1; i < args.size(); ++i)
    {
    if(args[i] == "LINK_INTERFACE_LIBRARIES")
      {
      this->DoingInterface = true;
      if(i != 1)
        {
        this->Makefile->IssueMessage(
          cmake::FATAL_ERROR,
          "The LINK_INTERFACE_LIBRARIES option must appear as the second "
          "argument, just after the target name."
          );
        return true;
        }
      }
    else if(args[i] == "debug")
      {
      if(haveLLT)
        {
        this->LinkLibraryTypeSpecifierWarning(llt, cmTarget::DEBUG);
        }
      llt = cmTarget::DEBUG;
      haveLLT = true;
      }
    else if(args[i] == "optimized")
      {
      if(haveLLT)
        {
        this->LinkLibraryTypeSpecifierWarning(llt, cmTarget::OPTIMIZED);
        }
      llt = cmTarget::OPTIMIZED;
      haveLLT = true;
      }
    else if(args[i] == "general")
      {
      if(haveLLT)
        {
        this->LinkLibraryTypeSpecifierWarning(llt, cmTarget::GENERAL);
        }
      llt = cmTarget::GENERAL;
      haveLLT = true;
      }
    else if(args[i] == "import")
      {
      if(haveLLT)
        {
        this->LinkLibraryTypeSpecifierWarning(llt, cmTarget::IMPORT);
        }
      llt = cmTarget::IMPORT;
      haveLLT = true;
      }
    else if(haveLLT)
      {
      // The link type was specified by the previous argument.
      haveLLT = false;
      this->HandleLibrary(args[i].c_str(), llt);
      }
    else
      {
      // Lookup old-style cache entry if type is unspecified.  So if you
      // do a target_link_libraries(foo optimized bar) it will stay optimized
      // and not use the lookup.  As there maybe the case where someone has
      // specifed that a library is both debug and optimized.  (this check is
      // only there for backwards compatibility when mixing projects built
      // with old versions of CMake and new)
      llt = cmTarget::GENERAL;
      std::string linkType = args[0];
      linkType += "_LINK_TYPE";
      const char* linkTypeString = 
        this->Makefile->GetDefinition( linkType.c_str() );
      if(linkTypeString)
        {
        if(strcmp(linkTypeString, "debug") == 0)
          {
          llt = cmTarget::DEBUG;
          }
        if(strcmp(linkTypeString, "optimized") == 0)
          {
          llt = cmTarget::OPTIMIZED;
          }
        }
      this->HandleLibrary(args[i].c_str(), llt);
      }
    } 

  // Make sure the last argument was not a library type specifier.
  if(haveLLT)
    {
    cmOStringStream e;
    e << "The \"" << this->LinkLibraryTypeNames[llt]
      << "\" argument must be followed by a library.";
    this->Makefile->IssueMessage(cmake::FATAL_ERROR, e.str());
    cmSystemTools::SetFatalErrorOccured();
    }

  // If the INTERFACE option was given, make sure the
  // LINK_INTERFACE_LIBRARIES property exists.  This allows the
  // command to be used to specify an empty link interface.
  if(this->DoingInterface &&
     !this->Target->GetProperty("LINK_INTERFACE_LIBRARIES"))
    {
    this->Target->SetProperty("LINK_INTERFACE_LIBRARIES", "");
    }

  return true;
}

//----------------------------------------------------------------------------
void
cmTargetLinkLibrariesCommand
::LinkLibraryTypeSpecifierWarning(int left, int right)
{
  cmOStringStream w;
  w << "Link library type specifier \""
    << this->LinkLibraryTypeNames[left] << "\" is followed by specifier \""
    << this->LinkLibraryTypeNames[right] << "\" instead of a library name.  "
    << "The first specifier will be ignored.";
  this->Makefile->IssueMessage(cmake::AUTHOR_WARNING, w.str());
}

//----------------------------------------------------------------------------
void
cmTargetLinkLibrariesCommand::HandleLibrary(const char* lib,
                                            cmTarget::LinkLibraryType llt)
{
  // Handle normal case first.
  if(!this->DoingInterface)
    {
    this->Makefile
      ->AddLinkLibraryForTarget(this->Target->GetName(), lib, llt);
    return;
    }

  // Include this library in the link interface for the target.
  if(llt == cmTarget::DEBUG)
    {
    // Put in only the DEBUG configuration interface.
    this->Target->AppendProperty("LINK_INTERFACE_LIBRARIES_DEBUG", lib);
    }
  else if(llt == cmTarget::OPTIMIZED)
    {
    // Put in only the non-DEBUG configuration interface.
    this->Target->AppendProperty("LINK_INTERFACE_LIBRARIES", lib);

    // Make sure the DEBUG configuration interface exists so that this
    // one will not be used as a fall-back.
    if(!this->Target->GetProperty("LINK_INTERFACE_LIBRARIES_DEBUG"))
      {
      this->Target->SetProperty("LINK_INTERFACE_LIBRARIES_DEBUG", "");
      }
    }
  else
    {
    // Put in both the DEBUG and non-DEBUG configuration interfaces.
    this->Target->AppendProperty("LINK_INTERFACE_LIBRARIES", lib);
    this->Target->AppendProperty("LINK_INTERFACE_LIBRARIES_DEBUG", lib);
    }
}
