#pragma once

#include <windows.h>
#include <string>
#include "Dialog.hpp"
#include "ControlEdit.hpp"


namespace WinApi
{
//TODO: move to WinApi common Dialogs
class LoginDialog : public Dialog
{
public:
    LoginDialog(InstanceHandle p_hInstance, Handle p_parentWindow);

    std::string getName() const;
    std::string getPass() const;
private:
    void onInit() override;
    void onOk();
    void onCancel();

    std::string userName;
    std::string userPass;
    Control::Edit name;
    Control::Edit pass;
};

}
