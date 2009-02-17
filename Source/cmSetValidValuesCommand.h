#ifndef cmSetValidValuesCommand_h
#define cmSetValidValuesCommand_h

#include "cmCommand.h"

/** \class cmSetValidValuesCommand
 * \brief Set a CMAKE variable
 *
 * cmSetValidValuesCommand sets a variable to a value with expansion.  
 */
class cmSetValidValuesCommand : public cmCommand
{
public:
  /**
   * This is a virtual constructor for the command.
   */
  virtual cmCommand* Clone() 
    {
    return new cmSetValidValuesCommand;
    }

  /**
   * This is called when the command is first encountered in
   * the CMakeLists.txt file.
   */
  virtual bool InitialPass(std::vector<std::string> const& args,
                           cmExecutionStatus &status);

  /**
   * This determines if the command is invoked when in script mode.
   */
  virtual bool IsScriptable() { return true; }

  /**
   * The name of the command as specified in CMakeList.txt.
   */
  virtual const char* GetName() {return "set_valid_values";}
  
  /**
   * Succinct documentation.
   */
  virtual const char* GetTerseDocumentation() 
    {
    return "Specify valid values for a CMAKE variable.";
    }
  
  /**
   * More documentation.
   */
  virtual const char* GetFullDocumentation()
    {
    return
      "  SET_VALID_VALUES(VAR [VALUE 1] [VALUE 2] .. [VALUE N]])\n"
      "Set a list of values which can be assigned to the variable. "
      "This command is intended to restrict possible values of a variable "
      "so variable behaves like an instance of enumeration type.\n"
      "The command resets any restrictions when no values are specified.";
    }
  
  cmTypeMacro(cmSetValidValuesCommand, cmCommand);
};



#endif
