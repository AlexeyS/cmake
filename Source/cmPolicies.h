/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile: cmPolicies.h,v $
  Language:  C++
  Date:      $Date: 2008-09-12 14:56:21 $
  Version:   $Revision: 1.10.2.6 $

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef cmPolicies_h
#define cmPolicies_h

#include "cmCustomCommand.h"

class cmake;
class cmMakefile;
class cmPolicy;

/** \class cmPolicies
 * \brief Handles changes in CMake behavior and policies
 *
 * See the cmake wiki section on policies for an overview of this class's
 * purpose
 */
class cmPolicies
{
public:
  cmPolicies();
  ~cmPolicies();

  enum PolicyStatus { OLD, WARN, NEW, REQUIRED_IF_USED, REQUIRED_ALWAYS };
  static const char* PolicyStatusNames[];

  enum PolicyID
  {
    CMP0000, // Policy version specification
    CMP0001, // Ignore old compatibility variable
    CMP0002, // Target names must be unique
    CMP0003, // Linking does not include extra -L paths
    CMP0004, // Libraries linked may not have leading or trailing whitespace
    CMP0005, // Definition value escaping
    CMP0006, // BUNDLE install rules needed for MACOSX_BUNDLE targets
    CMP0007, // list command handling of empty elements
    CMP0008, // Full-path libraries must be a valid library file name
    CMP0009, // GLOB_RECURSE should not follow symlinks by default

    // Always the last entry.  Useful mostly to avoid adding a comma
    // the last policy when adding a new one.
    CMPCOUNT
  };

  ///! convert a string policy ID into a number
  bool GetPolicyID(const char *id, /* out */ cmPolicies::PolicyID &pid);
  std::string GetPolicyIDString(cmPolicies::PolicyID pid);
  
  ///! Get the default status for a policy
  cmPolicies::PolicyStatus GetPolicyStatus(cmPolicies::PolicyID id);
  
  ///! Define a Policy for CMake
  void DefinePolicy(cmPolicies::PolicyID id,
                    const char *stringID,
                    const char *shortDescription,
                    const char *longDescription,
                    unsigned int majorVersionIntroduced,
                    unsigned int minorVersionIntroduced,
                    unsigned int patchVersionIntroduced,
                    cmPolicies::PolicyStatus status);

  ///! Set a policy level for this listfile
  bool ApplyPolicyVersion(cmMakefile *mf, const char *version);

  ///! return a warning string for a given policy
  std::string GetPolicyWarning(cmPolicies::PolicyID id);
  
  ///! return an error string for when a required policy is unspecified
  std::string GetRequiredPolicyError(cmPolicies::PolicyID id);

  ///! return an error string for when a required policy is unspecified
  std::string GetRequiredAlwaysPolicyError(cmPolicies::PolicyID id);

  ///! Get docs for policies
  void GetDocumentation(std::vector<cmDocumentationEntry>& v);

  private:
  // might have to make these internal for VS6 not sure yet
  std::map<PolicyID,cmPolicy *> Policies;
  std::map<std::string,PolicyID> PolicyStringMap;

  void DiagnoseAncientPolicies(std::vector<PolicyID> const& ancient,
                               unsigned int majorVer, unsigned int minorVer,
                               unsigned int patchVer, cmMakefile* mf);
};

#endif
