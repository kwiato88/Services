#pragma once

#include <windows.h>
#include "Dialog.hpp"
#include "ControlEdit.hpp"
#include "ControlList.hpp"
#include "ControlCheckBox.hpp"
#include "ChatterClientApp.hpp"

namespace WinApi
{

class ChatterClientDialog : public Dialog
{
public:
    ChatterClientDialog(InstanceHandle p_hInstance, Handle p_parentWindow,
        const std::string& p_userName, const std::string& p_cookie);
private:
    void onInit() override;
    void onSendClick();
    void onAddChatClick();
    void onRemoveChatClick();
    void onChatSelected();
    void onMessageReceived();
    void notifyMessageReceived();
    void updateChatList();
    void updateCurrentChat();
    void onLineChanged();
    void goOnLine();
    bool showContextMenu(int p_xPos, int p_yPos) override;
    void copySelectetUserName();
    void copyAllUserNames();
    void copySelectedChat();
    void changeEnterBahaviour();

    static const UINT WM_CHATTER_MESSAGE_RECEIVED = WM_APP + 1;
    Control::Edit curretnChat;
    Control::Edit message;
    Control::Edit userName;
    Control::Edit receiverAddr;
    Control::List chats;
    Control::CheckBox isOnline;
    Control::CheckBox shouldSendOnEnter;
    Chatter::ClientApp chatter;
    std::string name;
};

}
