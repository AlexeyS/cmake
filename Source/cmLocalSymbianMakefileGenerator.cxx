#include "cmLocalSymbianMakefileGenerator.h"
#include "cmMakefile.h"
#include "cmTarget.h"
#include "cmCustomCommand.h"

void cmLocalSymbianMakefileGenerator::Generate()
{
  cmTargets& targets = this->Makefile->GetTargets();
  for(cmTargets::iterator t = targets.begin(); t != targets.end(); ++t)
    {
    HandleSymbianProperties(&t->second);
    }

  this->cmLocalUnixMakefileGenerator3::Generate();
}

void cmLocalSymbianMakefileGenerator
::HandleSymbianProperties(cmTarget* t)
{
    if(t->GetType() != cmTarget::EXECUTABLE &&
       t->GetType() != cmTarget::SHARED_LIBRARY)
      {
      return;
      }

  const char* sid = t->GetProperty("SYMBIAN_SECUREID");
  const char* vid = t->GetProperty("SYMBIAN_VENDORID");
  const char* stack = t->GetProperty("SYMBIAN_EPOCSTACKSIZE");
  const char* uid1 = t->GetProperty("SYMBIAN_UID1");
  const char* uid2 = t->GetProperty("SYMBIAN_UID2");
  const char* uid3 = t->GetProperty("SYMBIAN_UID3");
  const char* targettype = t->GetProperty("SYMBIAN_TARGETTYPE");
  const char* capability = t->GetProperty("SYMBIAN_CAPABILITY");
  const bool dlldata = t->GetPropertyAsBool("SYMBIAN_EPOCALLOWDLLDATA");

  if(!sid)
    {
    sid = "0xa000017f";
    }

  if(!vid)
    {
    vid = "0x00000000";
    }

  if(!uid1)
    {
    uid1 = t->GetType() == cmTarget::EXECUTABLE ? "0x1000007A" : "0x10000079";
    }

  if(!uid2)
    {
    uid2 = "0x00000000";
    }

  if(!uid3)
    {
    uid3 = "0x00000000";
    }

  if(!targettype)
    {  
    targettype = t->GetType() == cmTarget::EXECUTABLE ? "EXE" : "DLL";
    }

  if(!capability)
    {
    capability = "NONE";
    }

  cmCustomCommandLine cmdLine;
  cmdLine.push_back("elf2e32");

  cmdLine.push_back(CombineArg("sid", sid));

  if(stack)
    {
    cmdLine.push_back(CombineArg("stack", stack));
    }

  cmdLine.push_back(CombineArg("uid1", uid1));
  cmdLine.push_back(CombineArg("uid2", uid2));
  cmdLine.push_back(CombineArg("uid3", uid3));
  cmdLine.push_back(CombineArg("vid", vid));
  cmdLine.push_back(CombineArg("capability", capability));
  cmdLine.push_back(CombineArg("targettype", targettype));

  if(dlldata)
    {
    cmdLine.push_back("--dlldata");
    }

  std::string binPath = "\"" + t->GetFullPath() + "\"";
  cmdLine.push_back(CombineArg("elfinput", binPath.c_str()));
  cmdLine.push_back(CombineArg("output", binPath.c_str()));
  
  cmCustomCommandLines cmdLines;
  cmdLines.push_back(cmdLine);

  std::vector<std::string> no_depends;
  std::vector<std::string> outputs;
  cmCustomCommand cmd(outputs, no_depends, cmdLines, "", NULL);

  // we have strong reasons to make this POST_BUILD step first
  // as the following steps may depend on the resulting binary
  std::vector<cmCustomCommand>& postBuildCmds = t->GetPostBuildCommands();
  postBuildCmds.insert(postBuildCmds.begin(), 1, cmd);
}

std::string cmLocalSymbianMakefileGenerator
::CombineArg(const char* name, const char* value)
{
  std::string result = "--";
  result += name;
  result += "=";
  result += value;
  return result;
}
