#pragma once

#include <windows.h>
#include "Dialog.hpp"

namespace WinApi
{

class ChatterClientDialog : public Dialog
{
public:
    ChatterClientDialog(InstanceHandle p_hInstance, Handle p_parentWindow);
private:
    void onInit() override;
};

}
