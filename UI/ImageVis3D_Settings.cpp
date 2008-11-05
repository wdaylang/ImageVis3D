/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2008 Scientific Computing and Imaging Institute,
   University of Utah.

   
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/


//!    File   : ImageVis3D_Settings.cpp
//!    Author : Jens Krueger
//!             SCI Institute
//!             University of Utah
//!    Date   : September 2008
//
//!    Copyright (C) 2008 SCI Institute

#include "ImageVis3D.h"
#include <Basics/SysTools.h>
#include <QtCore/QSettings>
#include <QtGui/QMessageBox>
#include <QtGui/QMdiSubWindow>

using namespace std;

void MainWindow::CheckSettings() {
  QSettings settings;

  // if memory isn't set this must be the first time we run this app
  if (UINT64_INVALID == settings.value("Memory/MaxGPUMem", UINT64_INVALID).toULongLong()) {
    do {
      QMessageBox::information(this, "Initial Setup", "As this is the first "
                               "time you've started ImageVis3D on this system, "
                               "you need to configure the initial settings.  "
                               "In particular, the memory usage settings need "
                               "to be configured according to the hardware "
                               "configuration of the machine.  Note that "
                               "these settings can be changed later in the "
                               "settings screen.");
    } while(!ShowSettings(SettingsDlg::MEM_TAB));
  } else ApplySettings();
}

bool MainWindow::ShowSettings(SettingsDlg::TabID eTabID) {
    QSettings settings;
    SettingsDlg settingsDlg(m_MasterController, eTabID, this);

    // load first setting to see if we allready saved something
    UINT64 iMaxGPU = settings.value("Memory/MaxGPUMem", UINT64_INVALID).toULongLong();

    // if memory is set load other values (otherwise the dialog box will initialize with defaults
    if (iMaxGPU != UINT64_INVALID) {
      // load other settings here
      UINT64 iMaxCPU = std::min<UINT64>(settings.value("Memory/MaxCPUMem", UINT64_INVALID).toULongLong(), m_MasterController.SysInfo()->GetCPUMemSize());
  
      bool bQuickopen = settings.value("Performance/Quickopen", m_bQuickopen).toBool();
      unsigned int iBlendPrecisionMode = settings.value("Performance/BlendPrecisionMode", 0).toUInt();
      UINT64 iMinFramerate = settings.value("Performance/MinFrameRate", m_iMinFramerate).toULongLong();

      settings.beginGroup("Renderer");
      FLOATVECTOR3 vBackColor1(settings.value("Background1R", 0.0f).toULongLong(),
                              settings.value("Background1G", 0.0f).toULongLong(),
                              settings.value("Background1B", 0.0f).toULongLong());

      FLOATVECTOR3 vBackColor2(settings.value("Background2R", 0.0f).toULongLong(),
                              settings.value("Background2G", 0.0f).toULongLong(),
                              settings.value("Background2B", 0.0f).toULongLong());

      FLOATVECTOR4 vTextColor(settings.value("TextR", 1.0f).toULongLong(),
                              settings.value("TextG", 1.0f).toULongLong(),
                              settings.value("TextB", 1.0f).toULongLong(),
                              settings.value("TextA", 1.0f).toULongLong());
      settings.endGroup();

      // hand data to form
      settingsDlg.Data2Form(iMaxCPU, iMaxGPU, bQuickopen, iMinFramerate, iBlendPrecisionMode, vBackColor1, vBackColor2, vTextColor);
    }

    if (settingsDlg.exec() == QDialog::Accepted) {

      // save settings
      settings.beginGroup("Memory");
      settings.setValue("MaxGPUMem", settingsDlg.GetGPUMem());
      settings.setValue("MaxCPUMem", settingsDlg.GetCPUMem());
      settings.endGroup();

      settings.beginGroup("Performance");
      settings.setValue("Quickopen", settingsDlg.GetQuickopen());
      settings.setValue("MinFrameRate", settingsDlg.GetMinFramerate());
      settings.setValue("BlendPrecisionMode", settingsDlg.GetBlendPrecisionMode());
      settings.endGroup();

      settings.beginGroup("Renderer");
      settings.setValue("Background1R", settingsDlg.GetBackgroundColor1().x);
      settings.setValue("Background1G", settingsDlg.GetBackgroundColor1().y);
      settings.setValue("Background1B", settingsDlg.GetBackgroundColor1().z);
      settings.setValue("Background2R", settingsDlg.GetBackgroundColor2().x);
      settings.setValue("Background2G", settingsDlg.GetBackgroundColor2().y);
      settings.setValue("Background2B", settingsDlg.GetBackgroundColor2().z);
      settings.setValue("TextR", settingsDlg.GetTextColor().x);
      settings.setValue("TextG", settingsDlg.GetTextColor().y);
      settings.setValue("TextB", settingsDlg.GetTextColor().z);
      settings.setValue("TextA", settingsDlg.GetTextColor().w);
      settings.endGroup();


      ApplySettings();
      return true;
    } else return false;

}


void MainWindow::ApplySettings() {
    QSettings settings;

    settings.beginGroup("Memory");
    UINT64 iMaxCPU = std::min<UINT64>(settings.value("MaxCPUMem", UINT64_INVALID).toULongLong(), m_MasterController.SysInfo()->GetCPUMemSize());
    UINT64 iMaxGPU = settings.value("MaxGPUMem", UINT64_INVALID).toULongLong();
    settings.endGroup();

    settings.beginGroup("Performance");
    m_bQuickopen = settings.value("Quickopen", m_bQuickopen).toBool();
    m_iMinFramerate = settings.value("MinFrameRate", m_iMinFramerate).toULongLong();
    m_iBlendPrecisionMode = settings.value("BlendPrecisionMode", 0).toUInt();
    settings.endGroup();

    settings.beginGroup("Renderer");
    m_vBackgroundColors[0] = FLOATVECTOR3(settings.value("Background1R", 0.0f).toULongLong(),
                            settings.value("Background1G", 0.0f).toULongLong(),
                            settings.value("Background1B", 0.0f).toULongLong());

    m_vBackgroundColors[1] = FLOATVECTOR3(settings.value("Background2R", 0.0f).toULongLong(),
                            settings.value("Background2G", 0.0f).toULongLong(),
                            settings.value("Background2B", 0.0f).toULongLong());

    m_vTextColor = FLOATVECTOR4(settings.value("TextR", 1.0f).toULongLong(),
                            settings.value("TextG", 1.0f).toULongLong(),
                            settings.value("TextB", 1.0f).toULongLong(),
                            settings.value("TextA", 1.0f).toULongLong());
    settings.endGroup();

    for (int i = 0;i<mdiArea->subWindowList().size();i++) {
      QWidget* w = mdiArea->subWindowList().at(i)->widget();
      RenderWindow* renderWin = qobject_cast<RenderWindow*>(w);
      renderWin->SetColors(m_vBackgroundColors, m_vTextColor);
      renderWin->SetBlendPrecision(AbstrRenderer::EBlendPrecision(m_iBlendPrecisionMode));
    }

    m_MasterController.SysInfo()->SetMaxUsableCPUMem(iMaxCPU);
    m_MasterController.SysInfo()->SetMaxUsableGPUMem(iMaxGPU);
    m_MasterController.MemMan()->MemSizesChanged();

}
