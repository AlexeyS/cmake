#ifndef cmGlobalVisualStudio8WMGenerator_h
#define cmGlobalVisualStudio8WMGenerator_h

#include "cmGlobalVisualStudio8Generator.h"


/** \class cmGlobalVisualStudio8WMGenerator
 * \brief Write a Visual Studio solution for Windows Mobile platform.
 *
 * cmGlobalVisualStudio8WMGenerator manages MSVS build process for a tree
 */
class cmGlobalVisualStudio8WMGenerator : 
  public cmGlobalVisualStudio8Generator
{
public:
  cmGlobalVisualStudio8WMGenerator();
  static cmGlobalGenerator* New() { 
    return new cmGlobalVisualStudio8WMGenerator; }
  
  ///! Get the name for the generator.
  virtual const char* GetName() const {
    return cmGlobalVisualStudio8WMGenerator::GetActualName();}
  static const char* GetActualName() {return "Visual Studio 8 2005 for Windows Mobile";}

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

  class cmWMConfigParser;
  void getInstalledSDKs();

private:
  typedef std::map<std::string, SDKInfo> InstalledSDKsMap;
  InstalledSDKsMap InstalledSDKs;
};
#endif
