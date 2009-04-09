#include "cmLocalSymbianMmpGenerator.h"
#include "cmGlobalGenerator.h"
#include "cmMakefile.h"
#include "cmTarget.h"
#include "cmSourceFile.h"
#include "cmake.h"

#include <cctype>
#include <string>
#include <fstream>
#include <algorithm>

#define MMP_COLUMN_WIDTH 20
#define MMP_INDENT "  "

void cmLocalSymbianMmpGenerator::Generate()
{
  cmTargets::iterator i = Makefile->GetTargets().begin();
  for (; i != Makefile->GetTargets().end(); ++i)
    {
    if (i->second.GetType() == cmTarget::UTILITY)
      {
      this->WriteMakefile(i->second);
      }
    else
      {
      this->WriteMmp(i->second);
      }
    }
}

void cmLocalSymbianMmpGenerator::WriteMmp(cmTarget& target)
{
  std::string targetName = target.GetName();
  std::string filename = Makefile->GetStartOutputDirectory();
  filename += "/" + targetName + ".mmp";
  std::ofstream mmp(filename.c_str());

  this->WriteTargetType(target, mmp);
  this->AddGenericOption(target, "UID", mmp);
  this->AddGenericOption(target, "SECUREID", mmp);
  this->AddGenericOption(target, "VENDORID", mmp);
  this->AddGenericOption(target, "EPOCSTACKSIZE", mmp);
  this->AddGenericOption(target, "EPOCHEAPSIZE", mmp);
  this->AddGenericOption(target, "CAPABILITY", mmp);
  mmp << std::endl;

  this->AddDefinitions(target, mmp);
  this->AddRawData(target, mmp);

  this->AddResources(target, mmp);
  this->AddIncludes(mmp);
  this->AddSources(target, mmp);

  if (target.GetType() != cmTarget::STATIC_LIBRARY)
    {
    this->AddLibraries(target, mmp);
    }

  size_t customCommandsCount = target.GetPreBuildCommands().size() +
                               target.GetPreLinkCommands().size()  +
                               target.GetPostBuildCommands().size();

  if (customCommandsCount > 0)
    {
    this->WriteHelperMakefile(target);
    }
}

void cmLocalSymbianMmpGenerator::WriteHelperMakefile(cmTarget& target)
{
  std::string filename = Makefile->GetStartOutputDirectory();
  filename += "/";
  filename += target.GetName();
  filename += ".mk";
  std::ofstream mk(filename.c_str());
  
  mk << "bld:" << std::endl;
  for (size_t i = 0; i < target.GetPreBuildCommands().size(); ++i)
    {
    this->WriteCustomCommand(target.GetPreBuildCommands()[i], mk);
    }

  for (size_t i = 0; i < target.GetPreLinkCommands().size(); ++i)
    {
    this->WriteCustomCommand(target.GetPreBuildCommands()[i], mk);
    }
  mk << std::endl;

  mk << "final:" << std::endl;
  for (size_t i = 0; i < target.GetPreLinkCommands().size(); ++i)
    {
    this->WriteCustomCommand(target.GetPreBuildCommands()[i], mk);
    }
  mk << std::endl;

  mk << "makmake freeze lib cleanlib clean resource"
     << " savespace releaseables:" << std::endl;
}

void cmLocalSymbianMmpGenerator::WriteCustomCommand(cmCustomCommand& cmd,
                                                    std::ostream& mk)
{
  if (cmd.GetComment() != NULL)
    {
    mk << "\t@echo " << cmd.GetComment();
    }

  const cmCustomCommandLines& cmdlines = cmd.GetCommandLines();
  cmCustomCommandLines::const_iterator i;
  for (i = cmdlines.begin(); i != cmdlines.end(); ++i)
    {
    const cmCustomCommandLine line = *i;
    cmCustomCommandLine::const_iterator j;
    mk << "\t";
    for (j = line.begin(); j != line.end(); ++j)
      {
      mk << (j != line.begin() ? " " : "") << *j;
      }
    mk << std::endl;
    }
}

void cmLocalSymbianMmpGenerator::WriteTargetType(cmTarget& target,
                                                 std::ostream& mmp)
{
  std::string varName = target.GetName();
  varName += "_SYMBIAN_TARGETTYPE";
  const char* targetType = Makefile->GetDefinition(varName.c_str());

  if (!targetType)
    {
    switch (target.GetType())
      {
      case cmTarget::EXECUTABLE:
        targetType = "exe";
        break;
      case cmTarget::SHARED_LIBRARY:
        targetType = "dll";
        break;
      case cmTarget::STATIC_LIBRARY:
        targetType = "lib";
        break;
      }
    }

  mmp << KeywordWithParam("TARGET") << target.GetName()
      << "." << targetType << std::endl;

  mmp << KeywordWithParam("TARGETTYPE") << targetType << std::endl;
}

std::string cmLocalSymbianMmpGenerator
::AddGenericOption(cmTarget& target,
                   const std::string& option,
                   std::ostream& mmp)
{
  std::string propName = "SYMBIAN_" + option;
  const char* rawValue = target.GetProperty(propName.c_str());

  if (rawValue)
    {
    std::string value = rawValue;
    this->ReplaceSemicolons(value, ' ');
    mmp << KeywordWithParam(option) << value << std::endl;
    }

  return rawValue ? rawValue : "";
}

void cmLocalSymbianMmpGenerator::AddIncludes(std::ostream& mmp)
{
  std::vector<std::string>& includes = Makefile->GetIncludeDirectories();
  std::vector<std::string>::iterator inc = includes.begin();
  for (; inc != includes.end(); ++inc)
    {
    std::string path = this->ConvertToRelativePath(
                                  StartOutputDirectoryComponents,
                                  inc->c_str());
    mmp << KeywordWithParam("SYSTEMINCLUDE") << path << std::endl;
    }

  if (includes.size() > 0)
    {
    mmp << std::endl;
    }
}

void cmLocalSymbianMmpGenerator::AddDefinitions(cmTarget& target,
                                                std::ostream& mmp)
{
  bool needNewline = false;

  if (this->WriteMacros(mmp, Makefile->GetProperty("COMPILE_DEFINITIONS")))
    needNewline = true;

  if (this->WriteMacros(mmp, target.GetProperty("COMPILE_DEFINITIONS")))
    needNewline = true;
  
  if (needNewline)
    mmp << std::endl;
}

void cmLocalSymbianMmpGenerator::AddResources(cmTarget& target,
                                              std::ostream& mmp)
{
  const std::vector<cmSymbianResource>& resources =
                                            target.GetSymbianResources();
  std::vector<cmSymbianResource>::const_iterator res = resources.begin();
  for (; res != resources.end(); ++res)
    {
    if (res->type == cmSymbianResource::GENERIC)
      {
      this->WriteGenericResource(*res, mmp);
      }
    else if (res->type == cmSymbianResource::BITMAP)
      {
      this->WriteBitmap(*res, mmp);
      }
    }
}

void cmLocalSymbianMmpGenerator::AddSources(cmTarget& target,
                                            std::ostream& mmp)
{
  std::vector<cmSourceFile*> const& sources = target.GetSourceFiles();
  std::vector<cmSourceFile*>::const_iterator src = sources.begin();

  for (; src != sources.end(); ++src)
    {
    std::vector<std::string>::const_iterator ext;
    ext = std::find(Makefile->GetHeaderExtensions().begin(),
                    Makefile->GetHeaderExtensions().end(),
                    (*src)->GetExtension());

    // skip headers
    if (ext != Makefile->GetHeaderExtensions().end())
      {
      continue;
      }
    
    mmp << KeywordWithParam("SOURCE");
    mmp << this->ConvertToRelativePath(StartOutputDirectoryComponents,
                                       (*src)->GetFullPath().c_str());
    mmp << std::endl;
    }

  if (sources.size() > 0)
    {
    mmp << std::endl;
    }
}

void cmLocalSymbianMmpGenerator::AddLibraries(cmTarget& target,
                                              std::ostream& mmp)
{
  cmTarget::LinkLibraryVectorType libraries = target.GetLinkLibraries();
  cmTarget::LinkLibraryVectorType::iterator lib = libraries.begin();
  for (; lib != libraries.end(); ++lib)
    {
    if (lib->second != cmTarget::IMPORT)
      {
      mmp << KeywordWithParam("STATICLIBRARY");
      }
    else
      {
      mmp << KeywordWithParam("LIBRARY");
      }
    mmp << lib->first;
    if (!cmSystemTools::StringEndsWith(lib->first.c_str(), ".lib"))
      {
      mmp << ".lib";
      }
    mmp << std::endl;
    }

  if (libraries.size() > 0)
    {
    mmp << std::endl;
    }
}

void cmLocalSymbianMmpGenerator::AddRawData(cmTarget& target,
                                            std::ostream& mmp)
{
  std::string varName = target.GetName();
  varName += "_SYMBIAN_RAW_DATA";
  const char* rawValue = Makefile->GetDefinition(varName.c_str());

  if (rawValue)
    {
    std::string value = rawValue;
    ReplaceSemicolons(value, '\n');
    mmp << value << std::endl;
    mmp << std::endl;
    }
}

bool cmLocalSymbianMmpGenerator::WriteMacros(std::ostream& mmp,
                                             const char* macros)
{
  if (! macros)
    {
    return false;
    }

  std::string values = macros;
  ReplaceSemicolons(values, ' ');
  mmp << KeywordWithParam("MACRO") << values << std::endl;
  return true;
}

void cmLocalSymbianMmpGenerator
::WriteGenericResource(const cmSymbianResource& res, std::ostream& mmp)
{
  mmp << KeywordWithParam("START RESOURCE");
  mmp << this->ConvertToRelativePath(StartOutputDirectoryComponents,
                                     res.filename.c_str());
  mmp << std::endl;

  if (!res.target.empty())
    {
    mmp << MMP_INDENT << KeywordWithParam("TARGET")
        << res.target << std::endl;
    }

  if (!res.targetpath.empty())
    {
    mmp << MMP_INDENT << KeywordWithParam("TARGETPATH")
        << res.targetpath << std::endl;
    }

  if (res.header)
    {
    mmp << MMP_INDENT << "HEADER" << std::endl;
    }

  if (!res.lang.empty())
    {
    mmp << MMP_INDENT << KeywordWithParam("LANG")
        << res.lang << std::endl;
    }

  if (!res.uid.empty())
    {
    mmp << MMP_INDENT << KeywordWithParam("UID") << res.uid << std::endl;
    }

  mmp << "END" << std::endl << std::endl;
}

void cmLocalSymbianMmpGenerator::WriteBitmap(const cmSymbianResource& res,
                                             std::ostream& mmp)
{
  mmp << KeywordWithParam("START BITMAP") << res.target << std::endl;

  if (!res.targetpath.empty())
    {
    mmp << MMP_INDENT << KeywordWithParam("TARGETPATH")
        << res.targetpath << std::endl;
    }

  if (res.header)
    {
    mmp << MMP_INDENT << "HEADER" << std::endl;
    }

  std::list<cmSymbianBitmapSource>::const_iterator img = res.images.begin();
  for (; img != res.images.end(); ++img)
    {
    mmp << MMP_INDENT << KeywordWithParam("SOURCE");
    mmp << img->depth << " ";
    mmp << this->ConvertToRelativePath(StartOutputDirectoryComponents,
                                       img->filename.c_str());
    mmp << std::endl;
    }

  mmp << "END" << std::endl << std::endl;
}

std::string
cmLocalSymbianMmpGenerator::KeywordWithParam(const std::string& option)
{
  std::string keyword = option;
  for (unsigned int i = 0; i < MMP_COLUMN_WIDTH - option.length() + 1; ++i)
    {
    keyword += ' ';
    }

  return keyword;
}

void cmLocalSymbianMmpGenerator::WriteMakefile(cmTarget& target)
{
  std::string targetName = target.GetName();
  std::string filename = Makefile->GetStartOutputDirectory();
  filename += "/" + targetName + ".mk";
  std::ofstream mk(filename.c_str());

  mk << "bld:" << std::endl;

  cmCustomCommand* cmd = target.GetSourceFiles()[0]->GetCustomCommand();
  if (cmd->GetWorkingDirectory())
    {
    std::string dir = cmd->GetWorkingDirectory();
#ifdef _WIN32
    for (unsigned int i = 0; i < dir.size(); ++i)
      {
      if (dir[i] == '/')
        {
        dir[i] = '\\';
        }
      }
#endif
      mk << "\t\tcd " <<  dir;
    }

  cmCustomCommandLines::const_iterator cmdline;
  cmdline = cmd->GetCommandLines().begin();
  for (; cmdline != cmd->GetCommandLines().end(); ++cmdline)
    {
    mk << ";";
    std::vector<std::string>::const_iterator i = cmdline->begin();
    mk << *(i++);
    for (; i != cmdline->end(); ++i)
      {
      mk << " " << *i;
      }
    }

  mk << std::endl;
  mk << "makmake freeze lib cleanlib clean final"
     << " resource savespace releaseables:";
  mk << std::endl << std::endl;
}

void cmLocalSymbianMmpGenerator::ReplaceSemicolons(std::string& s,
                                                   char newSeparator)
{
  std::string::size_type semicolon_pos = s.find(';');
  while (semicolon_pos != std::string::npos)
    {
    s[semicolon_pos] = newSeparator;
    semicolon_pos = s.find(';', semicolon_pos);
    }
}
