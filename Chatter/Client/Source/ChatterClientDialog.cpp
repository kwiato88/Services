#include "ChatterClientDialog.hpp"
#include "ChatterClientDialogDef.h"
#include "DialogMsgMatchers.hpp"
#include "MessageDialog.hpp"
#include "QueryDialog.hpp"

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
    : Dialog(p_hInstance, p_parentWindow, ResourceId(ID_CHATTER_CLIENT_DIALOG)), chatter(p_userName), name(p_userName)
{
    registerHandler(MsgMatchers::ButtonClick(ID_BUTTON_SEND), std::bind(&ChatterClientDialog::onSendClick, this));
    registerHandler(MsgMatchers::ButtonClick(ID_BUTTON_ADD), std::bind(&ChatterClientDialog::onAddChatClick, this));
    registerHandler(MsgMatchers::ButtonClick(ID_BUTTON_REMOVE), std::bind(&ChatterClientDialog::onRemoveChatClick, this));
    registerHandler(MsgMatchers::MsgCodeAndValue(ID_LIST_CHATS, LBN_DBLCLK), std::bind(&ChatterClientDialog::onChatSelected, this));
    //TODO: enter key on message
    // dialog receive message WM_KEYDOWN
    // with code ith code VK_RETURN
    // and current focus is on edit control (GetFocus() == ownHandle in Control)
    //   handle enter key
    //   return TRUE from dialoc func
    // else return FALSE from dialog func (let system deal with message)
    //TODO: enter/delete key on chats VK_RETURN/VK_DELETE
    //  same as key on message
    //TODO: context menu
    //  on chats: copy, add, remove
    //TODO: update on msg receive
}

void ChatterClientDialog::onInit()
{
    curretnChat.init(getItem(ResourceId(ID_EDIT_CHAT)));
    message.init(getItem(ResourceId(ID_EDIT_MESSAGE)));
    userName.init(getItem(ResourceId(ID_EDIT_USER_NAME)));
    receiverAddr.init(getItem(ResourceId(ID_EDIT_ADDRESS)));
    chats.init(getItem(ResourceId(ID_LIST_CHATS)));
    userName.setContent(name);
    receiverAddr.setContent("off-line");
    goOnLine();
}

void ChatterClientDialog::goOnLine()
try
{
    receiverAddr.setContent(chatter.goOnLine());
}
catch(std::exception& e)
{
    errorMessage(m_self, std::string("Can't go on-line: ") + e.what());
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
    updateCurrentChat();
}

void ChatterClientDialog::updateChatList()
{
    chats.clear();
    chats.addItems(chatter.getChats());
    chats.selectIndex(chatter.getCurrentChatIdx());
}

void ChatterClientDialog::updateCurrentChat()
{
    curretnChat.setContent(chatter.getCurrentChat());
    //TODO: scroll to end
    //TODO: scroll bar
}

}
