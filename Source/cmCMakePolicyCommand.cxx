/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile: cmCMakePolicyCommand.cxx,v $
  Language:  C++
  Date:      $Date: 2008-09-03 13:43:16 $
  Version:   $Revision: 1.2.2.2 $

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "cmCMakePolicyCommand.h"

#include "cmVersion.h"

// cmCMakePolicyCommand
bool cmCMakePolicyCommand
::InitialPass(std::vector<std::string> const& args, cmExecutionStatus &)
{
  if(args.size() < 1)
    {
    this->SetError("requires at least one argument.");
    return false;
    }

  if(args[0] == "SET")
    {
    return this->HandleSetMode(args);
    }
  else if(args[0] == "GET")
    {
    return this->HandleGetMode(args);
    }
  else if(args[0] == "PUSH")
    {
    if(args.size() > 1)
      {
      this->SetError("PUSH may not be given additional arguments.");
      return false;
      }
    return this->Makefile->PushPolicy();
    }
  else if(args[0] == "POP")
    {
    if(args.size() > 1)
      {
      this->SetError("POP may not be given additional arguments.");
      return false;
      }
    if(this->Makefile->PopPolicy(false))
      {
      return true;
      }
    else
      {
      this->SetError("POP without matching PUSH");
      return false;
      }
    }
  else if(args[0] == "VERSION")
    {
    return this->HandleVersionMode(args);
    }

  cmOStringStream e;
  e << "given unknown first argument \"" << args[0] << "\"";
  this->SetError(e.str().c_str());
  return false;
}

//----------------------------------------------------------------------------
bool cmCMakePolicyCommand::HandleSetMode(std::vector<std::string> const& args)
{
  if(args.size() != 3)
    {
    this->SetError("SET must be given exactly 2 additional arguments.");
    return false;
    }

  cmPolicies::PolicyStatus status;
  if(args[2] == "OLD")
    {
    status = cmPolicies::OLD;
    }
  else if(args[2] == "NEW")
    {
    status = cmPolicies::NEW;
    }
  else
    {
    cmOStringStream e;
    e << "SET given unrecognized policy status \"" << args[2] << "\"";
    this->SetError(e.str().c_str());
    return false;
    }

  if(!this->Makefile->SetPolicy(args[1].c_str(), status))
    {
    this->SetError("SET failed to set policy.");
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool cmCMakePolicyCommand::HandleGetMode(std::vector<std::string> const& args)
{
  if(args.size() != 3)
    {
    this->SetError("GET must be given exactly 2 additional arguments.");
    return false;
    }

  // Get arguments.
  std::string const& id = args[1];
  std::string const& var = args[2];

  // Lookup the policy number.
  cmPolicies::PolicyID pid;
  if(!this->Makefile->GetPolicies()->GetPolicyID(id.c_str(), pid))
    {
    cmOStringStream e;
    e << "GET given policy \"" << id << "\" which is not known to this "
      << "version of CMake.";
    this->SetError(e.str().c_str());
    return false;
    }

  // Lookup the policy setting.
  cmPolicies::PolicyStatus status = this->Makefile->GetPolicyStatus(pid);
  switch (status)
    {
    case cmPolicies::OLD:
      // Report that the policy is set to OLD.
      this->Makefile->AddDefinition(var.c_str(), "OLD");
      break;
    case cmPolicies::WARN:
      // Report that the policy is not set.
      this->Makefile->AddDefinition(var.c_str(), "");
      break;
    case cmPolicies::NEW:
      // Report that the policy is set to NEW.
      this->Makefile->AddDefinition(var.c_str(), "NEW");
      break;
    case cmPolicies::REQUIRED_IF_USED:
    case cmPolicies::REQUIRED_ALWAYS:
      // The policy is required to be set before anything needs it.
      {
      cmOStringStream e;
      e << this->Makefile->GetPolicies()->GetRequiredPolicyError(pid)
        << "\n"
        << "The call to cmake_policy(GET " << id << " ...) at which this "
        << "error appears requests the policy, and this version of CMake "
        << "requires that the policy be set to NEW before it is checked.";
      this->Makefile->IssueMessage(cmake::FATAL_ERROR, e.str());
      }
    }

  return true;
}

//----------------------------------------------------------------------------
bool
cmCMakePolicyCommand::HandleVersionMode(std::vector<std::string> const& args)
{
  if(args.size() <= 1)
    {
    this->SetError("VERSION not given an argument");
    return false;
    }
  else if(args.size() >= 3)
    {
    this->SetError("VERSION given too many arguments");
    return false;
    }
  this->Makefile->SetPolicyVersion(args[1].c_str());
  return true;
}
