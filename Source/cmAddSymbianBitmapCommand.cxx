#include "cmAddSymbianBitmapCommand.h"

bool cmAddSymbianBitmapCommand::InitialPass(std::vector<std::string> const& args,
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
  res.type = cmSymbianResource::BITMAP;
  res.target = *(++i);
  ++i;

  for (; i != args.end(); ++i)
    {
    if (*i == "TARGETPATH")
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
    else if (*i == "SOURCE")
      {
      if (++i == args.end() || i+1 == args.end())
        {
        SetError("The SOURCE argument must be followed by image depth "
                 "and relative path to the image");
        return false;
        }

      cmSymbianBitmapSource image;
      image.depth = *i;
      image.filename = Makefile->GetStartDirectory();
      image.filename += "/" + *(++i);
      res.images.push_back(image);
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
