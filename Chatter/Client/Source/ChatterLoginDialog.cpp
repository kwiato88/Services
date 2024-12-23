#include "ChatterLoginDialog.hpp"
#include "ChatterLoginDialogDef.h"
#include "DialogMsgMatchers.hpp"

namespace WinApi
{

LoginDialog::LoginDialog(InstanceHandle p_hInstance, Handle p_parentWindow)
    : Dialog(p_hInstance, p_parentWindow, ResourceId(ID_CHATTER_LOGIN_DIALOG))
{
    registerHandler(MsgMatchers::ButtonClick(IDOK), std::bind(&LoginDialog::onOk, this));
    registerHandler(MsgMatchers::ButtonClick(IDCANCEL), std::bind(&LoginDialog::onCancel, this));
}

void LoginDialog::onInit()
{
    name.init(getItem(ResourceId(ID_EDIT_NAME)));
    pass.init(getItem(ResourceId(ID_EDIT_PASS)));
    name.setFocus();
}

void LoginDialog::onOk()
{
    userName = name.getContent();
    userPass = pass.getContent();
    close(Dialog::RESULT_OK);
}

void LoginDialog::onCancel()
{
    userName.clear();
    userPass.clear();
    close(Dialog::RESULT_CANCEL);
}

std::string LoginDialog::getName() const
{
    return userName;
}

std::string LoginDialog::getPass() const
{
    return userPass;
}

} // namespace WinApi
