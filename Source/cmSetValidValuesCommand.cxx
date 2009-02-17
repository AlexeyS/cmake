#include "cmSetValidValuesCommand.h"

bool cmSetValidValuesCommand::InitialPass(std::vector<std::string> const& args, cmExecutionStatus &)
{
  if(args.size() < 1 )
    {
    this->SetError("called with incorrect number of arguments");
    return false;
    }
  
  const char* variable = args[0].c_str(); // VAR is always first
  cmCacheManager::CacheIterator it =
    this->Makefile->GetCacheManager()->GetCacheIterator(variable);

  if(it.IsAtEnd())
    {
    this->SetError((": variable " + args[0] + " is not found").c_str());
    return false;
    }

  bool has_valid_value = args.size() > 1 ? false : true;

  for(std::vector<std::string>::size_type i = 1; i < args.size(); ++i)
    if(args[i] == it.GetValue())
    {
    has_valid_value = true;
    break;
    }

  if(!has_valid_value)
    {
    this->SetError((": variable " + args[0] + " has invalid value").c_str());
    return false;
    }

  std::string values;
  for(std::vector<std::string>::size_type i = 1; i < args.size(); ++i)
    {
    values += args[i];
    values += i < (args.size() - 1) ? ";" : "";
    }

  it.SetProperty("VALID_VALUES", values.c_str());

  return true;
}

