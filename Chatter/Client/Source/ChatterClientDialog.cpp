#include "ChatterClientDialog.hpp"
#include "ChatterClientDialogDef.h"

namespace WinApi
{

ChatterClientDialog::ChatterClientDialog(InstanceHandle p_hInstance, Handle p_parentWindow)
    : Dialog(p_hInstance, p_parentWindow, ResourceId(ID_CHATTER_CLIENT_DIALOG))
{
}

void ChatterClientDialog::onInit()
{
}

}
