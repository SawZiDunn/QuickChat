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

    void setUsername(const QString &username);
    void setUsers(const QMap<QString, QPair<QString, QString>> &userMap) { users = userMap; }
    void setCurrentUser(const QString &user) { currentUser = user; }
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


    // New signals for communication with MainWindow
    void privateChatSelected(const QString &username);
    void groupChatJoined(const QString &chatName);

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
    QString currentUser;
    QStringList groupChats;

    // Mock database
    QMap<QString, QPair<QString, QString>> users; // email -> (username, password)
    QStringList groupChats;

    // Currently logged in user
    QString currentUser;

    // Add the database handler
    ChatDatabaseHandler dbHandler;
};

#endif // MENUWIDGET_H
