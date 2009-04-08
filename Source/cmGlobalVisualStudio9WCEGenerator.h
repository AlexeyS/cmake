#ifndef cmGlobalVisualStudio9WCEGenerator_h
#define cmGlobalVisualStudio9WCEGenerator_h

#include "cmGlobalVisualStudio9Generator.h"


/** \class cmGlobalVisualStudio9WCEGenerator
 * \brief Write a Visual Studio solution for Windows Mobile platform.
 *
 * cmGlobalVisualStudio9WCEGenerator manages MSVS build process for a tree
 */
class cmGlobalVisualStudio9WCEGenerator : 
  public cmGlobalVisualStudio9Generator
{
public:
  cmGlobalVisualStudio9WCEGenerator();
  static cmGlobalGenerator* New() { 
    return new cmGlobalVisualStudio9WCEGenerator; }
  
  ///! Get the name for the generator.
  virtual const char* GetName() const {
    return cmGlobalVisualStudio9WCEGenerator::GetActualName();}
  static const char* GetActualName() {return "Visual Studio 9 2008 for Windows CE";}

  /** Get the documentation entry for this generator.  */
  virtual void GetDocumentation(cmDocumentationEntry& entry) const;

  ///! create the correct local generator
  virtual cmLocalGenerator *CreateLocalGenerator();

  /**
   * Try to determine system infomation such as shared library
   * extension, pthreads, byte order etc.  
   */
  virtual void EnableLanguage(std::vector<std::string>const& languages, 
                              cmMakefile *, bool optional);

private:
  struct SDKInfo
    {
    std::string CEVer;
    std::string TargetMachine;
    };

  class cmWCEConfigParser;
  void getInstalledSDKs();

private:
  typedef std::map<std::string, SDKInfo> InstalledSDKsMap;
  InstalledSDKsMap InstalledSDKs;
};
#endif
