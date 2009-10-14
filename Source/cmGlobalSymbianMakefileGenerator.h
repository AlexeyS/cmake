#ifndef cmGlobalSymbianMakefileGenerator_h
#define cmGlobalSymbianMakefileGenerator_h

#include "cmGlobalUnixMakefileGenerator3.h"

class cmGlobalSymbianMakefileGenerator : public cmGlobalUnixMakefileGenerator3
{
public:
  cmGlobalSymbianMakefileGenerator();
  static cmGlobalGenerator* New() { return new cmGlobalSymbianMakefileGenerator; }
  ///! Get the name for the generator.
  virtual const char* GetName() const {
    return cmGlobalSymbianMakefileGenerator::GetActualName();}
  static const char* GetActualName() {return "Symbian Makefiles";}

  /** Get the documentation entry for this generator.  */
  virtual void GetDocumentation(cmDocumentationEntry& entry) const;
  
  ///! Create a local generator appropriate to this Global Generator
  virtual cmLocalGenerator *CreateLocalGenerator();

  /**
   * Try to determine system infomation such as shared library
   * extension, pthreads, byte order etc.  
   */
  virtual void EnableLanguage(std::vector<std::string>const& languages, 
                              cmMakefile *, bool optional);

private:
  bool SelectToolchain(cmMakefile* mf, const std::string& name);

private:
  std::string SDKPath;
};

#endif