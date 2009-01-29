/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile: cmCursesBoolWidget.cxx,v $
  Language:  C++
  Date:      $Date: 2006-03-16 15:44:55 $
  Version:   $Revision: 1.8 $

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "cmCursesBoolWidget.h"
#include "cmCursesMainForm.h"

cmCursesBoolWidget::cmCursesBoolWidget(int width, int height, 
                                       int left, int top) :
  cmCursesWidget(width, height, left, top)
{
  this->Type = cmCacheManager::BOOL;
  set_field_fore(this->Field,  A_NORMAL);
  set_field_back(this->Field,  A_STANDOUT);
  field_opts_off(this->Field,  O_STATIC);
  this->SetValueAsBool(false);
}

bool cmCursesBoolWidget::HandleInput(int& key, cmCursesMainForm*, WINDOW* w)
{

  // 10 == enter
  if (key == 10 || key == KEY_ENTER)
    {
    if (this->GetValueAsBool())
      {
      this->SetValueAsBool(false);
      }
    else
      {
      this->SetValueAsBool(true);
      }

    touchwin(w); 
    wrefresh(w); 
    return true;
    }
  else
    {
    return false;
    }
  
}

void cmCursesBoolWidget::SetValueAsBool(bool value)
{
  if (value)
    {
    this->SetValue("ON");
    }
  else
    { 
    this->SetValue("OFF");
    }
}

bool cmCursesBoolWidget::GetValueAsBool()
{
  if (this->Value == "ON")
    {
    return true;
    }
  else
    { 
    return false;
    }
}
