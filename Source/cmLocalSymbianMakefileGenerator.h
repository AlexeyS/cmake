#include "cmLocalUnixMakefileGenerator3.h"

class cmTarget;

class cmLocalSymbianMakefileGenerator : public cmLocalUnixMakefileGenerator3
{
public:
  virtual void Generate();

private:
  void HandleSymbianProperties(cmTarget* t);
  static std::string CombineArg(const char* name, const char* value);
};
