#ifndef GROUPCHATWIDGET_H
#define GROUPCHATWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QSplitter>
#include <QFrame>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <tuple>

#include "chatdbhandler.h"

class GroupChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GroupChatWidget(QWidget *parent = nullptr);

    void setGroupName(const QString &name);
    QString getGroupName() const;
    void setMembersList(const QStringList &members);
    void addMember(const QString &username);
    void removeMember(const QString &username);
    void clearChatHistory();
    void loadChatHistory(QList<std::tuple<QString, QString, QDateTime>> messages);
    void addSystemMessage(const QString &message);
    void addMessage(const QString &sender, const QString &message);

signals:
    void backRequested();
    void leaveChatRequested();
    void messageSubmitted(const QString &message);

private slots:
    void sendMessage();

private:
    void setupUI();
    void addHistoryMessage(const QString &sender, const QString &message, const QDateTime &timestamp);

    QLabel *groupNameLabel;
    QPushButton *backButton;
    QPushButton *leaveChatButton;
    QTextEdit *chatHistoryDisplay;
    QLineEdit *messageInputField;
    QPushButton *sendMessageButton;
    QListWidget *membersListWidget;
    QString currentGroupName;

    ChatDatabaseHandler dbHandler;
};

#endif // GROUPCHATWIDGET_H
