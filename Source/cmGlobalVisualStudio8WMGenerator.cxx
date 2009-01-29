
#include "windows.h"
#include "cmGlobalVisualStudio8WMGenerator.h"
#include "cmLocalVisualStudio7Generator.h"
#include "cmMakefile.h"
#include "cmake.h"
#include "cmXMLParser.h"

cmGlobalVisualStudio8WMGenerator::cmGlobalVisualStudio8WMGenerator()
{
  // Get map of installed SDKs in system
  // Make default platform for the first one in map
  this->getInstalledSDKs();
  if(!this->InstalledSDKs.empty())
    {
      this->PlatformName = this->InstalledSDKs.begin()->first.c_str();
    }
}

///! Create a local generator appropriate to this Global Generator
cmLocalGenerator *cmGlobalVisualStudio8WMGenerator::CreateLocalGenerator()
{
  const char* platform = GetCMakeInstance()->GetCacheDefinition("CMAKE_WINDOWS_MOBILE_PLATFORM");
  if( platform )
    this->PlatformName = platform;
  else
    {
    GetCMakeInstance()->AddCacheEntry("CMAKE_WINDOWS_MOBILE_PLATFORM",
                                      this->PlatformName.c_str(),
                                      "WinCE platform name",
                                      cmCacheManager::STRING);
    }

  cmLocalVisualStudio7Generator *lg = new cmLocalVisualStudio7Generator;
  lg->SetVersion8();
  lg->SetPlatformName(this->PlatformName.c_str());
  lg->SetExtraFlagTable(this->GetExtraFlagTableVS8());
  lg->SetGlobalGenerator(this);
  return lg;
}

//----------------------------------------------------------------------------
void cmGlobalVisualStudio8WMGenerator
::GetDocumentation(cmDocumentationEntry& entry) const
{
  entry.Name = this->GetName();
  entry.Brief = "Generates Visual Studio project files for Windows Mobile platforms.";
  entry.Full = "";
}

void cmGlobalVisualStudio8WMGenerator
::EnableLanguage(std::vector<std::string>const &  lang, 
                 cmMakefile *mf, bool optional)
{
  InstalledSDKsMap::const_iterator it = this->InstalledSDKs.find(this->PlatformName);
  if(it == this->InstalledSDKs.end())
    {
    cmSystemTools::Error("CMAKE_WINDOWS_MOBILE_PLATFORM must be a valid CE platform!");
    cmSystemTools::SetFatalErrorOccured();
    return;
    }

  mf->AddDefinition("CMAKE_UNDER_CE", "TRUE");
  mf->AddDefinition("CMAKE_CE_VER", it->second.CEVer.c_str());
  
  mf->AddCacheDefinition("CMAKE_C_COMPILER_WORKS", "1", "", cmCacheManager::INTERNAL);
  mf->AddCacheDefinition("CMAKE_CXX_COMPILER_WORKS", "1", "", cmCacheManager::INTERNAL);

  cmGlobalVisualStudio8Generator::EnableLanguage(lang, mf, optional);

  std::string CELinkerFlags = " /SUBSYSTEM:WINDOWSCE,";
  CELinkerFlags += it->second.CEVer.c_str();
  CELinkerFlags += " /MACHINE:";
  CELinkerFlags += it->second.TargetMachine.c_str();

  mf->AddDefinition("CMAKE_EXE_LINKER_FLAGS", CELinkerFlags.c_str());
  mf->AddDefinition("CMAKE_SHARED_LINKER_FLAGS", CELinkerFlags.c_str());
  mf->AddDefinition("CMAKE_MODULE_LINKER_FLAGS", CELinkerFlags.c_str());
}

// This class is used to parse XML with configuration
// of installed SDKs in system
class cmGlobalVisualStudio8WMGenerator::cmWMConfigParser : public cmXMLParser
{
public:
  cmWMConfigParser(InstalledSDKsMap& sdks)
      : SDKs(sdks)
    {
    }

  virtual void StartElement(const char * name, const char ** atts)
    {
    if(strcmp(name, "Macro") == 0 &&
       strcmp(atts[1], "INSTRUCTIONSET") == 0)
      {
      // correspondence between processor type and
      // linker /machine flag
      struct Platform2LinkerMachine
      {
        std::string Platform;
        std::string LinkerMachine;
      };

      static const Platform2LinkerMachine ConvertInfo[] =
        {
          {"ARMV4",     "ARM" },
          {"ARMV4I",    "THUMB"},
          {"MIPSII",    "MIPS"},
          {"MIPSII_FP", "MIPSFPU"},
          {"MIPSIV",    "MIPS16"},
          {"MIPSIV_FP", "MIPSFPU16"},
          {"SH4",       "SH4"},
          {"x86",       "X86"}
        };

      // find corresponding flag value for obtained architecture
      for(int i = 0; i < sizeof(ConvertInfo) / sizeof(ConvertInfo[0]); ++i)
        {
        if(ConvertInfo[i].Platform == atts[3])
          {
          this->CurSDKInfo.TargetMachine = ConvertInfo[i].LinkerMachine;
          this->SDKs[this->CurSDK] = this->CurSDKInfo;
          break;
          }
        }
      }
    }

  void EndElement(const char * name)
    {
    if(strcmp(name, "PlatformName") == 0)
      {
        this->CurSDK = this->CharacterData;
      }
    else if(strcmp(name, "OSMajorVersion") == 0)
      {
        this->CurSDKInfo.CEVer = this->CharacterData + ".";
      }
    else if(strcmp(name, "OSMinorVersion") == 0)
      {
        this->CurSDKInfo.CEVer += this->CharacterData;
      }
    }

   void CharacterDataHandler(const char* data, int length)
   {
     this->CharacterData.assign(data, length);
   }

private:
  std::string CharacterData;
  std::string CurSDK;
  SDKInfo CurSDKInfo;

  InstalledSDKsMap& SDKs;
};

void cmGlobalVisualStudio8WMGenerator::
getInstalledSDKs()
{
  // get Visual Studio 2005 install directory
  static const char* pathKey =
    "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\VisualStudio\\8.0\\Setup\\VS;ProductDir";

  this->InstalledSDKs.clear();

  std::string vsInstallPath;
  if(!cmSystemTools::ReadRegistryValue(pathKey, vsInstallPath))
    {
    return;
    }
  cmSystemTools::ConvertToUnixSlashes(vsInstallPath);

  const std::string configFilename =
    vsInstallPath + std::string("/VC/vcpackages/WCE.VCPlatform.config");

  // parse XML containing all information that we need
  cmWMConfigParser parser(InstalledSDKs);
  parser.ParseFile(configFilename.c_str());
}