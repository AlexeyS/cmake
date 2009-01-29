/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: FLTKDialog.cxx,v $
  Language:  C++
  Date:      $Date: 2007-12-13 22:56:50 $
  Version:   $Revision: 1.8 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "CMakeSetupGUIImplementation.h"
#include "FL/Fl.H"
#include "FL/fl_ask.H"

#include "cmSystemTools.h"

int main(int argc, char * argv[] ) 
{
  cmSystemTools::FindExecutableDirectory(argv[0]);

  fl_message_font(FL_HELVETICA,11);

  CMakeSetupGUIImplementation * gui 
       = new CMakeSetupGUIImplementation;

  gui->SetPathToExecutable( argv[0] );
  gui->Show();
  gui->LoadRecentDirectories();
  gui->LoadCacheFromDiskToGUI();

  Fl::run();

  delete gui;
  
  return 0;
  
}
