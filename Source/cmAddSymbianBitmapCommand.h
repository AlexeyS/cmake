#ifndef cmAddSymbianBitmapCommand_h
#define cmAddSymbianBitmapCommand_h

#include "cmCommand.h"

class cmAddSymbianBitmapCommand : public cmCommand
{
public:
  virtual cmCommand* Clone() 
    {
    return new cmAddSymbianBitmapCommand;
    }

  virtual bool InitialPass(std::vector<std::string> const &args,
                           cmExecutionStatus &status);

  virtual const char* GetName() { return "add_symbian_bitmap"; }

  virtual const char* GetTerseDocumentation() 
    {
    return "Add symbian bitmap.";
    }
  
  virtual const char* GetFullDocumentation()
    {
    return "";
    }
  
  cmTypeMacro(cmAddSymbianBitmapCommand, cmCommand);
};


#endif
