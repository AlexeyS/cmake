#include "cmAddSymbianResourceCommand.h"

bool cmAddSymbianResourceCommand::InitialPass(std::vector<std::string> const &args,
                                              cmExecutionStatus&)
{
  if (args.size() < 3)
    return false;

  std::vector<std::string>::const_iterator i = args.begin();
  cmTarget* target = Makefile->FindTarget(i->c_str());

  if (!target)
    {
    std::string msg = "Target " + *i + " not found";
    SetError(msg.c_str());
    return false;
    }

  cmSymbianResource res;
  res.filename = Makefile->GetStartDirectory();
  res.filename += "/" + *(++i);
  ++i;

  for (; i != args.end(); ++i)
    {
    if (*i == "TARGET")
      {
      if (++i == args.end())
        {
        SetError("The TARGET argument must be followed by symbian target file name");
        return false;
        }
      res.target = *i;
      }
    else if (*i == "TARGETPATH")
      {
      if (++i == args.end())
        {
            SetError("The TARGETPATH argument must be followed by symbian target path");
            return false;
        }
      res.targetpath = *i;
      }
    else if (*i == "HEADER")
      {
        res.header = true;
      }
    else if (*i == "LANG")
      {
      if (++i == args.end())
        {
        SetError("The LANG argument must be followed by langueage codes "
                  "for the symbian resource");
        return false;
        }
      res.lang = *i;
      }
    else if (*i == "UID")
      {
      if (++i == args.end())
        {
        SetError("The UID argument must be followed by second and "
                 "optionally third uid for symbian resource");
        return false;
        }
      res.uid = *i;
      }
    else
      {
      std::string msg = "Unknown extra parameter \"" + *i +"\"";
      SetError(msg.c_str());
      return false;
      }
    }

  target->AddSymbianResource(res);
  return true;
}
