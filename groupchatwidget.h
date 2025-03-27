#ifndef GROUPCHATWIDGET_H
#define GROUPCHATWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>

class GroupChatWidget : public QWidget
{
    Q_OBJECT

public:
    GroupChatWidget(QWidget *parent = nullptr);
    ~GroupChatWidget() = default;

    void setGroupName(const QString &name);
    void setMembersList(const QStringList &members);
    void addMember(const QString &username);
    void removeMember(const QString &username);
    void clearChatHistory();
    void addSystemMessage(const QString &message);
    void addMessage(const QString &sender, const QString &message);

signals:
    void leaveChatRequested();
    void messageSubmitted(const QString &message);

private slots:
    void sendMessage();

private:
    void setupUI();

    // UI components
    QLabel *groupNameLabel;
    QPushButton *leaveChatButton;
    QTextEdit *chatHistoryDisplay;
    QLineEdit *messageInputField;
    QPushButton *sendMessageButton;
    QListWidget *membersListWidget;
};

#endif // GROUPCHATWIDGET_H
