#include "cmGlobalSymbianMakefileGenerator.h"
#include "cmLocalUnixMakefileGenerator3.h"
#include "cmMakefile.h"
#include "cmXMLParser.h"

#include <list>
#include <memory>
#include <iostream>

class InstalledSDKParser : public cmXMLParser
{
public:
  virtual void StartElement(const char* name, const char** atts)
    {
    if(strcmp(name, "sdk-path") == 0)
      {
      this->ParsingSDKPath = true;
      }
    }

  virtual void CharacterDataHandler(const char* data, int length)
    {
    if(this->ParsingSDKPath)
      {
      this->Current.append(data, length);
      }
    }

  virtual void EndElement(const char* name)
    {
    if(strcmp(name, "sdk-path") == 0)
      {
      this->ParsingSDKPath = false;
      this->SDKs.push_back(this->Current);
      this->Current.clear();
      }
    }

  std::list<std::string> SDKs;

  virtual int InitializeParser()
    {
    this->ParsingSDKPath = false;
    return cmXMLParser::InitializeParser();
    }

private:
  std::string Current;
  bool ParsingSDKPath;
};

cmGlobalSymbianMakefileGenerator::cmGlobalSymbianMakefileGenerator()
{
  this->FindMakeProgramFile = "CMakeFindSymbianMake.cmake";
  this->ForceUnixPaths = false;
  this->ToolSupportsColor = true;
  this->NeedSymbolicMark = true;
  this->EmptyRuleHackCommand = "@cd .";

  const char* symbianCommonReg = "HKEY_LOCAL_MACHINE\\Software\\Symbian"
                                 "\\EPOC SDKs;CommonPath";
  std::string symbianCommonPath;
  if(!cmSystemTools::ReadRegistryValue(symbianCommonReg, symbianCommonPath))
    {
    cmSystemTools::Error("Could not find Symbian SDK installed");
    return;
    }

  const std::string installedFile = symbianCommonPath + "\\installed2.xml";
  InstalledSDKParser parser;
  parser.ParseFile(installedFile.c_str());

  if(parser.SDKs.size() == 0)
    {
    cmSystemTools::Error("Could not find Symbian SDK installed");
    return;
    }

  this->SDKPath = *parser.SDKs.begin();
}

bool cmGlobalSymbianMakefileGenerator
::SelectToolchain(cmMakefile* mf, const std::string& name)
{
  const std::string path = SDKPath +
              "\\Epoc32\\tools\\compilation_config\\" + name + ".mk";

  std::ifstream f(path.c_str());
  if(!f)
    {
    cmSystemTools::Error("Could not find toolchain ", name.c_str());
    return false;
    }

  char c;
  bool commentLine;
  std::string line;
  while((c = f.get()) && f)
    {
    if (line.empty() && c == '#')
      {
      commentLine = true;
      }

    if (commentLine)
      {
      if (c == '\n')
        {
        commentLine = false;
        }
      continue;
      }

      if (c == '\n')
        {
        std::string::size_type eqPos = line.find('=');
        if (eqPos != std::string::npos)
          {
          std::string varName = "SYMBIAN_" + line.substr(0, eqPos);
          std::string varValue = line.substr(eqPos+1);
          mf->AddDefinition(varName.c_str(), varValue.c_str());
          }
        line.clear();
        commentLine = false;
        }
      else
        {
        line += c;
        }
    }

  return true;
}

void cmGlobalSymbianMakefileGenerator
::EnableLanguage(std::vector<std::string>const& l, 
                 cmMakefile *mf, 
                 bool optional)
{
  mf->AddDefinition("SYMBIAN", "1");
  mf->AddDefinition("CMAKE_SYSTEM", "Symbian");
  mf->AddDefinition("CMAKE_SYSTEM_NAME", "Symbian");

  mf->AddDefinition("SYMBIAN_SDK_PATH", SDKPath.c_str());

  if(!mf->GetDefinition("SYMBIAN_TOOLCHAIN"))
    {
    mf->AddCacheDefinition("SYMBIAN_TOOLCHAIN", "GCCE", "",
                           cmCacheManager::STRING);
    }

  this->SelectToolchain(mf, mf->GetDefinition("SYMBIAN_TOOLCHAIN"));

  mf->AddDefinition("CMAKE_GENERATOR_CC", mf->GetDefinition("SYMBIAN_CC"));
  mf->AddDefinition("CMAKE_GENERATOR_CXX", mf->GetDefinition("SYMBIAN_CC"));

  this->cmGlobalUnixMakefileGenerator3::EnableLanguage(l, mf, optional);
}

///! Create a local generator appropriate to this Global Generator
cmLocalGenerator *cmGlobalSymbianMakefileGenerator::CreateLocalGenerator()
{
  cmLocalUnixMakefileGenerator3* lg = new cmLocalUnixMakefileGenerator3;
  lg->SetWindowsShell(true);
  lg->SetMakeSilentFlag("-s");
  lg->SetGlobalGenerator(this);
  lg->SetIgnoreLibPrefix(true);
  lg->SetPassMakeflags(false);
  lg->SetUnixCD(false);
  return lg;
}

//----------------------------------------------------------------------------
void cmGlobalSymbianMakefileGenerator
::GetDocumentation(cmDocumentationEntry& entry) const
{
  entry.Name = this->GetName();
  entry.Brief = "Generates makefiles for Symbian SDK.";
  entry.Full = "";
}
