#include "ChatterClientDialog.hpp"
#include "ChatterClientDialogDef.h"
#include "DialogMsgMatchers.hpp"
#include "MessageDialog.hpp"
#include "QueryDialog.hpp"
#include "ContextMenu.hpp"
#include "Clipboard.hpp"

namespace WinApi
{

void errorMessage(Handle p_parent, const std::string& p_msg)
{
    MessageDialog(p_parent)
    .withTitle("Error")
    .withContent(p_msg)
    .with(MessageDialog::Icon::Error)
    .with(MessageDialog::Buttons::Ok)
    .show();
}

ChatterClientDialog::ChatterClientDialog(InstanceHandle p_hInstance, Handle p_parentWindow, const std::string& p_userName)
    : Dialog(p_hInstance, p_parentWindow, ResourceId(ID_CHATTER_CLIENT_DIALOG)),
      chatter(p_userName, std::bind(&ChatterClientDialog::notifyMessageReceived, this)), name(p_userName)
{
    registerHandler(MsgMatchers::ButtonClick(ID_BUTTON_SEND), std::bind(&ChatterClientDialog::onSendClick, this));
    registerHandler(MsgMatchers::ButtonClick(ID_BUTTON_ADD), std::bind(&ChatterClientDialog::onAddChatClick, this));
    registerHandler(MsgMatchers::ButtonClick(ID_BUTTON_REMOVE), std::bind(&ChatterClientDialog::onRemoveChatClick, this));
    registerHandler(MsgMatchers::MsgCodeAndValue(ID_LIST_CHATS, LBN_DBLCLK), std::bind(&ChatterClientDialog::onChatSelected, this));
    registerHandler(MsgMatchers::MsgCodeAndValue(1015, 1015), std::bind(&ChatterClientDialog::onMessageReceived, this)); //TODO: remove
    registerHandler(MsgMatchers::MsgCodeAndValue(ID_CHECK_ON_LINE, BN_CLICKED), std::bind(&ChatterClientDialog::onLineChanged, this));
    //TODO: enter key on message (*)
    // dialog receive message WM_KEYDOWN
    // with code ith code VK_RETURN
    // and current focus is on edit control (GetFocus() == ownHandle in Control)
    //   handle enter key
    //   return TRUE from dialoc func
    // else return FALSE from dialog func (let system deal with message)
    //TODO: enter/delete key on chats VK_RETURN/VK_DELETE (*)
    //  same as key on message
    //TODO: update on msg receive (*)
    //  register handler (onMessageReceived) for message WN_CHATTER_MESSAGE_RECEIVED
    //TODO: online/offline button

    // WinApi extension
    //TODO: dialog to allow register handler for msg *
    //  matcher(msgCode, lParam, wParam(hi, lo)), handler
    //TODO: refactor registering for control events
}

void ChatterClientDialog::onInit()
{
    curretnChat.init(getItem(ResourceId(ID_EDIT_CHAT)));
    message.init(getItem(ResourceId(ID_EDIT_MESSAGE)));
    userName.init(getItem(ResourceId(ID_EDIT_USER_NAME)));
    receiverAddr.init(getItem(ResourceId(ID_EDIT_ADDRESS)));
    chats.init(getItem(ResourceId(ID_LIST_CHATS)));
    isOnline.init(getItem(ResourceId(ID_CHECK_ON_LINE)));
    shouldSendOnEnter.init(getItem(ResourceId(ID_CHECK_SEND_ON_ENTER)));
    shouldSendOnEnter.check();
    isOnline.uncheck();
    userName.setContent(name);
    receiverAddr.setContent("off-line");
    goOnLine();
    chats.setFocus();;
}

void ChatterClientDialog::goOnLine()
try
{
    receiverAddr.setContent(chatter.goOnLine());
    isOnline.check();
}
catch(std::exception& e)
{
    isOnline.uncheck();
    errorMessage(m_self, std::string("Can't go on-line: ") + e.what());
}

void ChatterClientDialog::onLineChanged()
{
    if(isOnline.isChecked())
        goOnLine();
    else
    {
       chatter.goOffLine();
       receiverAddr.setContent("off-line");
    }
}

void ChatterClientDialog::onSendClick()
try
{
    chatter.sendMessage(message.getContent());
    updateCurrentChat();
    message.setContent("");
}
catch(std::exception& e)
{
    errorMessage(m_self, std::string("Can't send message: ") + e.what());
    updateCurrentChat();
    message.setContent("");
}

void ChatterClientDialog::onAddChatClick()
try
{
    QueryDialog dlg(m_hInstance, m_self);
    dlg.setQuestion("Start chat with");
    dlg.setInitialResponse("userName");
    if(dlg.show() != Dialog::RESULT_OK)
    {
        errorMessage(m_self, "ret notOK");
        return;
    }
    auto chatWith = dlg.getResponse();
    if(chatWith.empty())
    {
        errorMessage(m_self, "empty name");
        return;
    }
    chatter.newChat(chatWith);
    updateChatList();
    updateCurrentChat();
}
catch(std::exception& e)
{
    errorMessage(m_self, std::string("Can't start new chat: ") + e.what());
}

void ChatterClientDialog::onRemoveChatClick()
{
    auto selected = chats.selectedIndex();
    if(selected == -1)
        return;
    chatter.removeChat(selected);
    updateChatList();
    updateCurrentChat();
}

void ChatterClientDialog::onChatSelected()
{
    auto selected = chats.selectedIndex();
    if(selected == -1)
        return;
    chatter.chatWith(selected);
    updateChatList();
    updateCurrentChat();
}

void ChatterClientDialog::onMessageReceived()
{
    updateChatList();
    updateCurrentChat();
}

void  ChatterClientDialog::notifyMessageReceived()
{
    //PostMessage(m_self, WM_CHATTER_MESSAGE_RECEIVED, 0, 0);
    PostMessage(m_self, WM_COMMAND, MAKELONG(1015,1015), 0); //TODO: remove
}

void ChatterClientDialog::updateChatList()
{
    chats.clear();
    chats.addItems(chatter.getChats());
    chats.selectIndex(chatter.getCurrentChatIdx());
    //TODO: scroll bar
}

void ChatterClientDialog::updateCurrentChat()
{
    curretnChat.setContent(chatter.getCurrentChat());
    //TODO: scroll to end
}

bool ChatterClientDialog::showContextMenu(int p_xPos, int p_yPos)
{
    if (chats.isWithin(p_xPos, p_yPos))
    {
        ContextMenu menu(m_self);
        menu.add(ContextMenu::Item{ "Start new chat" , std::bind(&ChatterClientDialog::onAddChatClick, this) });
        menu.add(ContextMenu::Item{ "Remove chat" , std::bind(&ChatterClientDialog::onRemoveChatClick, this) });
        menu.add(ContextMenu::Item{ "Copy user name" , std::bind(&ChatterClientDialog::copySelectetUserName, this) });
        menu.add(ContextMenu::Item{ "Copy all user names" , std::bind(&ChatterClientDialog::copyAllUserNames, this) });
        menu.add(ContextMenu::Item{ "Copy chat" , std::bind(&ChatterClientDialog::copySelectedChat, this) });
        menu.show(p_xPos, p_yPos);
        return true;
    }
    return false;
}

void ChatterClientDialog::copySelectetUserName()
{
    Clipboard::set(Clipboard::String(chats.selectedItem()));
}

void ChatterClientDialog::copyAllUserNames()
{
    std::string names;
    for(const auto& name : chatter.getChats())
        names += name + "\n";
    Clipboard::set(Clipboard::String(names));
}

void ChatterClientDialog::copySelectedChat()
{
    auto selected = chats.selectedIndex();
    if(selected != -1)
        Clipboard::set(Clipboard::String(chatter.getChatWith(selected)));
}

}