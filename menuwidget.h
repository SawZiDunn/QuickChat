#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QStackedWidget>
#include <QMap>
#include <QPair>
#include <QStringList>

#include "chatdbhandler.h"
#include "groupchatwidget.h"
#include "privatechatwidget.h"
#include "groupchatlistwidget.h"

class MenuWidget : public QWidget
{
    Q_OBJECT
public:
    MenuWidget(ChatDatabaseHandler& dbHandler, QStackedWidget *stackedWidget, QWidget *parent = nullptr);
    ~MenuWidget() = default;

    // void setUsername(const QString &username);
    void setUsers(const QMap<QString, QPair<QString, QString>> &userMap) { users = userMap; }
    void setCurrentUser(const QString &username, const QString &email) { currentUser = qMakePair(username, email); }
    void setGroupChats(const QStringList &chats) { groupChats = chats; }

    // Chat action methods
    void startPrivateChat();
    void viewGroupChats();
    void createGroupChat();
    void joinGroupChat();
    void logoutRequested();

signals:
    void startPrivateChatRequested();
    void viewGroupChatsRequested();
    void createGroupChatRequested();
    void joinGroupChatRequested();

private:
    void setupUI();

    // UI components
    QLabel *titleLabel;
    QLabel *welcomeLabel;
    QPushButton *startPrivateChatButton;
    QPushButton *viewGroupChatsButton;
    QPushButton *createGroupChatButton;
    QPushButton *joinGroupChatButton;
    QPushButton *logoutButton;

    // Data members
    QStackedWidget *stackedWidget;
    QMap<QString, QPair<QString, QString>> users; // Map of email -> (username, password)
    QPair<QString, QString> currentUser; // Currently logged in user, (name, email)
    QStringList groupChats;

    GroupChatWidget *groupChat;
    GroupChatListWidget *groupChatListWidget;

    // Add the database handler
    ChatDatabaseHandler &dbHandler;
};

#endif // MENUWIDGET_H
