#ifndef PRIVATECHATWIDGET_H
#define PRIVATECHATWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QDateTime>
#include <tuple>
#include "chatdbhandler.h"

class PrivateChatWidget : public QWidget
{
    Q_OBJECT

public:
    PrivateChatWidget(const QString &currentUserEmail, const QString &recipientEmail, const QString &recipientName, ChatDatabaseHandler &dbHandler, QWidget *parent=nullptr);
    ~PrivateChatWidget() = default;

    void clearChatHistory();
    void addSystemMessage(const QString &message);
    void addIncomingMessage(const QString &sender, const QString &email, const QString &message);
    void addOutgoingMessage(const QString &message);
    void loadChatHistory();

signals:
    void backToMenuRequested();

private slots:
    void sendMessage();
    void scrollToBottom();

private:
    void setupUI();
    QString formatTimestamp(const QDateTime &timestamp);

    // UI components
    QLabel *chatPartnerLabel;
    QLabel *partnerNameLabel;
    QLabel *partnerEmailLabel;
    QPushButton *leaveChatButton;
    QTextEdit *chatHistoryDisplay;
    QLineEdit *messageInputField;
    QPushButton *sendMessageButton;

    // Data members
    QString userEmail;
    QString recipientEmail;
    QString recipientName;
    ChatDatabaseHandler &dbHandler;
    QTimer *refreshTimer;
};

#endif // PRIVATECHATWIDGET_H
