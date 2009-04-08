#ifndef cmGlobalVisualStudio8WCEGenerator_h
#define cmGlobalVisualStudio8WCEGenerator_h

#include "cmGlobalVisualStudio8Generator.h"


/** \class cmGlobalVisualStudio8WCEGenerator
 * \brief Write a Visual Studio solution for Windows Mobile platform.
 *
 * cmGlobalVisualStudio8WCEGenerator manages MSVS build process for a tree
 */
class cmGlobalVisualStudio8WCEGenerator : 
  public cmGlobalVisualStudio8Generator
{
public:
  cmGlobalVisualStudio8WCEGenerator();
  static cmGlobalGenerator* New() { 
    return new cmGlobalVisualStudio8WCEGenerator; }
  
  ///! Get the name for the generator.
  virtual const char* GetName() const {
    return cmGlobalVisualStudio8WCEGenerator::GetActualName();}
  static const char* GetActualName() {return "Visual Studio 8 2005 for Windows CE";}

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
