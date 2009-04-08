#ifndef cmGlobalSymbianMmpGenerator_h
#define cmGlobalSymbianMmpGenerator_h

#include "cmGlobalGenerator.h"

class cmTarget;

class cmGlobalSymbianMmpGenerator : public cmGlobalGenerator
{
public:
  static cmGlobalGenerator* New()
    {return new cmGlobalSymbianMmpGenerator;}

  virtual cmLocalGenerator *CreateLocalGenerator();

  virtual const char* GetName() const
    { return cmGlobalSymbianMmpGenerator::GetActualName(); }

  virtual void GetDocumentation(cmDocumentationEntry& entry) const;

  virtual void Generate();

  static const char* GetActualName()
    {return "Symbian SDK .mmp projects"; }

private:
  cmGlobalSymbianMmpGenerator();
};

#endif
