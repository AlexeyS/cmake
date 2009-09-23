#include "cmGlobalSymbianMmpGenerator.h"
#include "cmLocalSymbianMmpGenerator.h"
#include "cmCacheManager.h"
#include "cmMakefile.h"
#include "cmSystemTools.h"
#include "cmake.h"

#include <fstream>

cmGlobalSymbianMmpGenerator::cmGlobalSymbianMmpGenerator()
{
  this->FindMakeProgramFile = "CMakeSymbianFindMake.cmake";
}

cmLocalGenerator *cmGlobalSymbianMmpGenerator::CreateLocalGenerator()
{
    cmLocalSymbianMmpGenerator* lg = new cmLocalSymbianMmpGenerator;
    lg->SetGlobalGenerator(this);
    return lg;
}

void cmGlobalSymbianMmpGenerator::Generate()
{
  cmGlobalGenerator::Generate();

  std::string filename = GetCMakeInstance()->GetStartOutputDirectory();
  filename += "/bld.inf";
  std::ofstream bld_inf(filename.c_str());
  bld_inf << "prj_mmpfiles" << std::endl;

  std::vector<std::string> basedir;
  cmSystemTools::SplitPath(GetCMakeInstance()->GetStartOutputDirectory(),
                           basedir);

  for (size_t i = 0; i < LocalGenerators.size(); ++i)
    {
    cmTargets targets = LocalGenerators[i]->GetMakefile()->GetTargets();
    
    for (cmTargets::iterator t = targets.begin(); t != targets.end(); ++t)
      {
      cmLocalGenerator* local = LocalGenerators[i];

      if (t->second.GetType() == cmTarget::UTILITY)
          bld_inf << "gnumakefile ";

      std::string prj = local->GetMakefile()->GetStartOutputDirectory();
      prj += (prj.length() > 0 ? "/" : "");
      prj += t->first;
      prj += (t->second.GetType() == cmTarget::UTILITY ? ".mk" : ".mmp");
      bld_inf << local->ConvertToRelativePath(basedir, prj.c_str());
      bld_inf << std::endl;

      if (t->second.GetType() != cmTarget::UTILITY)
        {
        size_t customStepsCount = t->second.GetPreBuildCommands().size() +
                                  t->second.GetPreLinkCommands().size() +
                                  t->second.GetPostBuildCommands().size();
        if (customStepsCount > 0)
          {
              std::string helperMk = t->first;
              helperMk += ".mk";
              bld_inf << "gnumakefile ";
              bld_inf << local->ConvertToRelativePath(basedir,
                                                      helperMk.c_str());
              bld_inf << std::endl;
          }
        }
      }
    }
}

void cmGlobalSymbianMmpGenerator
::GetDocumentation(cmDocumentationEntry& entry) const
{
  entry.Name  = this->GetName();
  entry.Brief = "Generates Symbian SDK .mmp project files.";
  entry.Full  =
      "The .mmp files can be used to build a project in usual Symbian "
      "way using SDK tools.";
}
