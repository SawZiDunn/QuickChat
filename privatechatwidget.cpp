#include "privatechatwidget.h"

PrivateChatWidget::PrivateChatWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void PrivateChatWidget::setupUI()
{
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Create header
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);

    chatPartnerLabel = new QLabel("Chat with: ");
    chatPartnerLabel->setFont(QFont("Arial", 12, QFont::Bold));

    partnerNameLabel = new QLabel();
    partnerNameLabel->setFont(QFont("Arial", 12));

    leaveChatButton = new QPushButton("Leave Chat");
    leaveChatButton->setFixedWidth(100);

    headerLayout->addWidget(chatPartnerLabel);
    headerLayout->addWidget(partnerNameLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(leaveChatButton);

    // Create chat display
    chatHistoryDisplay = new QTextEdit();
    chatHistoryDisplay->setReadOnly(true);
    chatHistoryDisplay->setStyleSheet("background-color: #232323; border: 1px solid #3a3a3a; border-radius: 5px;");

    // Create message input area
    QWidget *messageWidget = new QWidget();
    QHBoxLayout *messageLayout = new QHBoxLayout(messageWidget);

    messageInputField = new QLineEdit();
    messageInputField->setPlaceholderText("Type your message here...");
    messageInputField->setStyleSheet("padding: 8px;");

    sendMessageButton = new QPushButton("Send");
    sendMessageButton->setFixedWidth(80);

    messageLayout->addWidget(messageInputField);
    messageLayout->addWidget(sendMessageButton);

    // Add widgets to main layout
    layout->addWidget(headerWidget);
    layout->addWidget(chatHistoryDisplay);
    layout->addWidget(messageWidget);

    // Connect signals
    connect(leaveChatButton, &QPushButton::clicked, this, &PrivateChatWidget::leaveChatRequested);
    connect(sendMessageButton, &QPushButton::clicked, this, &PrivateChatWidget::sendMessage);
    connect(messageInputField, &QLineEdit::returnPressed, this, &PrivateChatWidget::sendMessage);
}

void PrivateChatWidget::setChatPartner(const QString &partnerName)
{
    partnerNameLabel->setText(partnerName);
}

void PrivateChatWidget::clearChatHistory()
{
    chatHistoryDisplay->clear();
}

void PrivateChatWidget::addSystemMessage(const QString &message)
{
    chatHistoryDisplay->append("<center><span style='color:#9e9e9e;'>--- " +
                               message + " ---</span></center>");
}

void PrivateChatWidget::addIncomingMessage(const QString &sender, const QString &message)
{
    chatHistoryDisplay->append("<span style='color:#66bb6a;'><b>" +
                               sender + ":</b></span> " + message);
}

void PrivateChatWidget::addOutgoingMessage(const QString &message)
{
    chatHistoryDisplay->append("<span style='color:#42a5f5;'><b>You:</b></span> " +
                               message);
}

void PrivateChatWidget::sendMessage()
{
    if (!messageInputField->text().isEmpty()) {
        emit messageSubmitted(messageInputField->text());
        addOutgoingMessage(messageInputField->text());
        messageInputField->clear();
    }
}
