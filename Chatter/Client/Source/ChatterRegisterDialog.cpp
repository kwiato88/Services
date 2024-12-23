#include "ChatterRegisterDialog.hpp"
#include "ChatterRegisterDialogDef.h"
#include "MessageDialog.hpp"
#include "DialogMsgMatchers.hpp"

namespace WinApi
{

RegisterDialog::RegisterDialog(InstanceHandle p_hInstance, Handle p_parentWindow)
    : Dialog(p_hInstance, p_parentWindow, ResourceId(ID_CHATTER_REGISTER_DIALOG))
{
    registerHandler(MsgMatchers::ButtonClick(IDOK), std::bind(&RegisterDialog::onOk, this));
    registerHandler(MsgMatchers::ButtonClick(IDCANCEL), std::bind(&RegisterDialog::onCancel, this));
}

void RegisterDialog::onInit()
{
    name.init(getItem(ResourceId(ID_EDIT_NAME)));
    pass1.init(getItem(ResourceId(ID_EDIT_PASS_1)));
    pass2.init(getItem(ResourceId(ID_EDIT_PASS_2)));
    name.setFocus();
}

void RegisterDialog::onOk()
{
    if(pass1.getContent() == pass2.getContent())
    {
        userName = name.getContent();
        userPass = pass1.getContent();
        close(Dialog::RESULT_OK);
    }
    else
    {
        MessageDialog{m_self}.withTitle("Register")
            .withContent("Passwords do not match").show();
    }
}

void RegisterDialog::onCancel()
{
    userName.clear();
    userPass.clear();
    close(Dialog::RESULT_CANCEL);
}

std::string RegisterDialog::getName() const
{
    return userName;
}

std::string RegisterDialog::getPass() const
{
    return userPass;
}

}
