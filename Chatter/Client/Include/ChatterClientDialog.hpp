#pragma once

#include <windows.h>
#include "Dialog.hpp"
#include "ControlEdit.hpp"
#include "ControlList.hpp"
#include "ChatterClientApp.hpp"

namespace WinApi
{

class ChatterClientDialog : public Dialog
{
public:
    ChatterClientDialog(InstanceHandle p_hInstance, Handle p_parentWindow, const std::string& p_userName);
private:
    void onInit() override;
    void onSendClick();
    void onAddChatClick();
    void onRemoveChatClick();
    void onChatSelected();
    void updateChatList();
    void updateCurrentChat();
    void goOnLine();

    Control::Edit curretnChat;
    Control::Edit message;
    Control::Edit userName;
    Control::Edit receiverAddr;
    Control::List chats;
    Chatter::ClientApp chatter;
    std::string name;
};

}
