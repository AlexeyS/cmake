/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile: cmIncludeExternalMSProjectCommand.cxx,v $
  Language:  C++
  Date:      $Date: 2008-01-28 13:38:35 $
  Version:   $Revision: 1.23 $

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "cmIncludeExternalMSProjectCommand.h"

// cmIncludeExternalMSProjectCommand
bool cmIncludeExternalMSProjectCommand
::InitialPass(std::vector<std::string> const& args, cmExecutionStatus &)
{
  if(args.size() < 2) 
  {
  this->SetError("INCLUDE_EXTERNAL_MSPROJECT called with incorrect "
                 "number of arguments");
  return false;
  }
// only compile this for win32 to avoid coverage errors
#ifdef _WIN32
  std::string customType;
  std::string customGuid;
  std::string platformMapping;

  if(this->Makefile->GetDefinition("WIN32"))
    {
    std::string location = args[1];
    bool gotTypeKeyword = false;
    bool gotPlatformKeyword = false;
    bool gotGuidKeyword = false;
    
    std::vector<std::string> depends;
    if (args.size() > 2)
      {
      for (unsigned int i=2; i<args.size(); ++i) 
        {
        if (gotTypeKeyword)
          {
          customType = args[i];
          gotTypeKeyword = false;
          }
        else if (gotGuidKeyword)
          {
          customGuid = args[i];
          gotGuidKeyword = false;
          }
        else if (gotPlatformKeyword)
          {
          platformMapping = args[i];
          gotPlatformKeyword = false;
          }
        else if (args[i] == "TYPE")
          gotTypeKeyword = true;
        else if (args[i] == "GUID")
          gotGuidKeyword = true;
        else if (args[i] == "PLATFORM")
          gotPlatformKeyword = true;
        else
          depends.push_back(args[i]); 
        }
      }

    // Hack together a utility target storing enough information
    // to reproduce the target inclusion.
    std::string utility_name("INCLUDE_EXTERNAL_MSPROJECT");
    utility_name += "_";
    utility_name += args[0];
    std::string path = args[1];
    cmSystemTools::ConvertToUnixSlashes(path);

    if (!customGuid.empty())
      {
      std::string guidStoreName = args[0] + "_GUID_CMAKE";
      this->Makefile->GetCMakeInstance()->AddCacheEntry(guidStoreName.c_str(),
                                                        customGuid.c_str(),
                                                        "Stored GUID",
                                                        cmCacheManager::INTERNAL);
      }

    // Create a target instance for this utility.
    cmTarget* target=this->Makefile->AddNewTarget(cmTarget::UTILITY, 
                                                  utility_name.c_str());
    target->SetProperty("EXCLUDE_FROM_ALL","FALSE");
    if (!customType.empty())
      target->SetProperty("MSVS_PROJECT_TYPE",customType.c_str());
    if (!platformMapping.empty())
      target->SetProperty("MSVS_PLATFORM_MAPPING",platformMapping.c_str());

    std::vector<std::string> no_outputs;
    cmCustomCommandLines commandLines;
    cmCustomCommandLine commandLine;
    commandLine.push_back(args[0]);
    commandLine.push_back(path);
    commandLines.push_back(commandLine);
    cmCustomCommand cc(no_outputs, depends, commandLines, 0, 0);
    target->GetPostBuildCommands().push_back(cc);
    }
#endif
  return true;
}
