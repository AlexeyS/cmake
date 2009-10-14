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
  this->SelectToolchain("gcce");
}

bool cmGlobalSymbianMakefileGenerator
::SelectToolchain(const std::string& name)
{
  this->ToolchainDefs.clear();

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
          std::string varName = line.substr(0, eqPos);
          std::string varValue = line.substr(eqPos+1);
          this->ToolchainDefs.insert(std::make_pair(varName, varValue));
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
  // pick a default 
  mf->AddDefinition("SYMBIAN", "1");
  mf->AddDefinition("CMAKE_QUOTE_INCLUDE_PATHS", "1");

  const std::string cc = this->ToolchainDefs["CC"];
  mf->AddDefinition("CMAKE_GENERATOR_CC", cc.c_str());
  mf->AddDefinition("CMAKE_GENERATOR_CXX", cc.c_str());

  // load GCCE toolchain file
  mf->AddDefinition("CMAKE_C_COMPILER_LOADED", "1");
  mf->AddDefinition("CMAKE_CXX_COMPILER_LOADED", "1");

  mf->AddDefinition("CMAKE_C_COMPILER", cc.c_str());
  mf->AddDefinition("CMAKE_CXX_COMPILER", cc.c_str());

  mf->AddDefinition("CMAKE_SYSTEM", "Symbian");
  mf->AddDefinition("CMAKE_SYSTEM_NAME", "Symbian");

  std::string ld = ToolchainDefs["LD"];
  std::string linkRule = ld + " <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET>";
  mf->AddDefinition("CMAKE_C_LINK_EXECUTABLE", linkRule.c_str());
  mf->AddDefinition("CMAKE_CXX_LINK_EXECUTABLE", linkRule.c_str());

  mf->AddCacheDefinition("CMAKE_C_FLAGS",
                         this->ToolchainDefs["C_LANG_OPTION"].c_str(),
                         "", cmCacheManager::STRING);

  mf->AddCacheDefinition("CMAKE_CXX_FLAGS",
                         this->ToolchainDefs["CPP_LANG_OPTION"].c_str(),
                         "", cmCacheManager::STRING);

  std::string stdLibPath = GetStdLibPath();
  std::string linkFlags = this->ToolchainDefs["LINKER_ENTRY_OPTION"] +
                          " _E32Startup";

  linkFlags += " " + this->ToolchainDefs["STATIC_LIBS_PATH"] + " " +
               "\"" + stdLibPath + "\" " +
               this->ToolchainDefs["STATIC_LIBS_LIST"];

  std::string epocLibPath = this->SDKPath + "\\EPOC32\\RELEASE\\ARMV5\\LIB";
  std::vector<cmsys::String> libs;
  const char* rtLibsList = this->ToolchainDefs["RUNTIME_LIBS_LIST"].c_str();
  libs = cmSystemTools::SplitString(rtLibsList, ' ');
  for (unsigned int i = 0; i < libs.size(); ++i)
    {
    linkFlags += " \"" + epocLibPath + "\\" + libs[i] + "\"";
    }

  mf->AddCacheDefinition("CMAKE_EXE_LINKER_FLAGS",
                         linkFlags.c_str(),
                         "", cmCacheManager::STRING);

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

std::string cmGlobalSymbianMakefileGenerator
::GetStdLibPath() const
{
    std::string output;
    cmSystemTools::RunCommand("arm-none-symbianelf-g++ -print-libgcc-file-name", output);
    return cmSystemTools::GetFilenamePath(output);
}
