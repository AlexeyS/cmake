#ifndef cmLocalSymbianMmpGenerator_h
#define cmLocalSymbianMmpGenerator_h

#include "cmLocalGenerator.h"
#include "cmTarget.h"

class cmLocalSymbianMmpGenerator : public cmLocalGenerator
{
public:
  void Generate();

private:
  void WriteMmp(cmTarget& target);
  void WriteHelperMakefile(cmTarget& target);
  void WriteTargetType(cmTarget& target, std::ostream& mmp);

  std::string AddGenericOption(cmTarget& target,
                               const std::string& option,
                               std::ostream& mmp);

  void AddIncludes(std::ostream& mmp);
  void AddResources(cmTarget& target, std::ostream& mmp);
  void AddSources(cmTarget& target, std::ostream& mmp);
  void AddLibraries(cmTarget& target, std::ostream& mmp);
  void AddDefinitions(cmTarget& target, std::ostream& mmp);
  void AddRawData(cmTarget& target, std::ostream& mmp);

  bool WriteMacros(std::ostream& mmp, const char* macros);
  void WriteGenericResource(const cmSymbianResource& res,
                            std::ostream& mmp);

  void WriteBitmap(const cmSymbianResource& res, std::ostream& mmp);
  void WriteCustomCommand(cmCustomCommand& cmd, std::ostream& mk);
  void WriteMakefile(cmTarget& target);

  static std::string KeywordWithParam(const std::string& option);
  static void ReplaceSemicolons(std::string& s, char newSeparator);
};

#endif
