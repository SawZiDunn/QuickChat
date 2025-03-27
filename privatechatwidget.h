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

class PrivateChatWidget : public QWidget
{
    Q_OBJECT

public:
    PrivateChatWidget(QWidget *parent = nullptr);
    ~PrivateChatWidget() = default;

    void setChatPartner(const QString &partnerName);
    void clearChatHistory();
    void addSystemMessage(const QString &message);
    void addIncomingMessage(const QString &sender, const QString &message);
    void addOutgoingMessage(const QString &message);

signals:
    void leaveChatRequested();
    void messageSubmitted(const QString &message);

private slots:
    void sendMessage();

private:
    void setupUI();

    // UI components
    QLabel *chatPartnerLabel;
    QLabel *partnerNameLabel;
    QPushButton *leaveChatButton;
    QTextEdit *chatHistoryDisplay;
    QLineEdit *messageInputField;
    QPushButton *sendMessageButton;
};

#endif // PRIVATECHATWIDGET_H
