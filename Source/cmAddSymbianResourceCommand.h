#ifndef cmAddSymbianResourceCommand_h
#define cmAddSymbianResourceCommand_h

#include "cmCommand.h"

class cmAddSymbianResourceCommand : public cmCommand
{
public:
  virtual cmCommand* Clone() 
    {
    return new cmAddSymbianResourceCommand;
    }

  virtual bool InitialPass(std::vector<std::string> const &args,
                           cmExecutionStatus &status);

  virtual const char* GetName() { return "add_symbian_resource"; }

  virtual const char* GetTerseDocumentation() 
    {
    return "Add symbian resource.";
    }
  
  virtual const char* GetFullDocumentation()
    {
    return "";
    }
  
  cmTypeMacro(cmAddSymbianResourceCommand, cmCommand);
};


#endif
