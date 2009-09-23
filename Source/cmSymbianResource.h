#ifndef cmSymbianResource_h
#define cmSymbianResource_h

#include "cmStandardIncludes.h"

struct cmSymbianBitmapSource
{
  std::string filename;
  std::string depth;
};

struct cmSymbianResource
{
  enum Type {GENERIC, BITMAP};

  Type type;
  bool header;
  std::string lang;
  std::string filename;
  std::list<cmSymbianBitmapSource> images;
  std::string target;
  std::string targetpath;
  std::string uid;

  cmSymbianResource() : header(false), type(GENERIC) {}
};

#endif
