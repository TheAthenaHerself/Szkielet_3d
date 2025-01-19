#pragma once
#include "Application3D.h"
void LoadPipeline();
void LoadAssets();
void SetApp(std::shared_ptr<Application_3d>& _app);
void WaitForPreviousFrame();
void PopulateCommandList();