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


// Structure to store chat request information
struct ChatRequest {
    QString fromUser;
    QString toUser;
    bool pending;
};

class MenuWidget : public QWidget
{
    Q_OBJECT
public:
    MenuWidget(QStackedWidget *stackedWidget, QWidget *parent = nullptr);
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


    // Chat request management
    QList<ChatRequest> chatRequests;
    QTimer *requestCheckTimer;
    void setupPrivateChatPage();
    void checkPendingRequests();
    void sendChatRequest(const QString &toUser);
    void acceptChatRequest(const ChatRequest &request);
    void rejectChatRequest(const ChatRequest &request);
    void openPrivateChat(const QString &otherUrse);

    // Data members
    QStackedWidget *stackedWidget;
    QMap<QString, QPair<QString, QString>> users; // Map of email -> (username, password)
    QPair<QString, QString> currentUser; // Currently logged in user
    QStringList groupChats;

    // Add the database handler
    ChatDatabaseHandler dbHandler;
};

#endif // MENUWIDGET_H
