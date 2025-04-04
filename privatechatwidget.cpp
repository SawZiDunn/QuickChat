#include "privatechatwidget.h"
#include <QDateTime>
#include <QTimer>

PrivateChatWidget::PrivateChatWidget(const QString &currentUserEmail, const QString &recipientEmail, const QString &recipientName, ChatDatabaseHandler &dbHandler, QWidget *parent)
    : QWidget(parent), userEmail(currentUserEmail), recipientEmail(recipientEmail), recipientName(recipientName), dbHandler(dbHandler)
{
    setupUI();
    // Set the recipient's email in the UI
    partnerNameLabel->setText(recipientName);
    partnerEmailLabel->setText(recipientEmail);
    loadChatHistory();

    // Setup refresh timer for chat history
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &PrivateChatWidget::loadChatHistory);
    refreshTimer->start(8000);
}

void PrivateChatWidget::setupUI()
{
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);

    // Set widget background
    setStyleSheet("QWidget { background-color: #2d2d2d; }");

    // Create header
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setSpacing(5);

    QHBoxLayout *topHeaderLayout = new QHBoxLayout();

    chatPartnerLabel = new QLabel("Chat with: ");
    chatPartnerLabel->setFont(QFont("Arial", 12, QFont::Bold));
    chatPartnerLabel->setStyleSheet("color: #ffffff;");

    partnerNameLabel = new QLabel();
    partnerNameLabel->setFont(QFont("Arial", 12));
    partnerNameLabel->setStyleSheet("color: #ffffff;");

    partnerEmailLabel = new QLabel();
    partnerEmailLabel->setFont(QFont("Arial", 10));
    partnerEmailLabel->setStyleSheet("color: #9e9e9e;");

    leaveChatButton = new QPushButton("Back to Menu");
    leaveChatButton->setFixedWidth(120);
    leaveChatButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #424242;"
        "   color: white;"
        "   border-radius: 5px;"
        "   padding: 8px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #616161;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #2a82da;"
        "}"
        );

    topHeaderLayout->addWidget(chatPartnerLabel);
    topHeaderLayout->addWidget(partnerNameLabel);
    topHeaderLayout->addStretch();
    topHeaderLayout->addWidget(leaveChatButton);

    headerLayout->addLayout(topHeaderLayout);
    headerLayout->addWidget(partnerEmailLabel);

    // Create chat display
    chatHistoryDisplay = new QTextEdit();
    chatHistoryDisplay->setReadOnly(true);
    chatHistoryDisplay->setStyleSheet(
        "QTextEdit {"
        "   background-color: #1d1d1d;"
        "   border: 1px solid #424242;"
        "   border-radius: 8px;"
        "   padding: 10px;"
        "   color: #ffffff;"
        "}"
        "QScrollBar:vertical {"
        "   background-color: #2d2d2d;"
        "   width: 12px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background-color: #424242;"
        "   min-height: 30px;"
        "   border-radius: 6px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background-color: #616161;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   height: 0px;"
        "}"
        );
    chatHistoryDisplay->setMinimumHeight(400);

    // Create message input area
    QWidget *messageWidget = new QWidget();
    QHBoxLayout *messageLayout = new QHBoxLayout(messageWidget);
    messageLayout->setSpacing(10);

    messageInputField = new QLineEdit();
    messageInputField->setPlaceholderText("Type your message here...");
    messageInputField->setStyleSheet(
        "QLineEdit {"
        "   background-color: #1d1d1d;"
        "   border: 1px solid #424242;"
        "   border-radius: 5px;"
        "   padding: 10px;"
        "   color: #ffffff;"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid #2a82da;"
        "}"
        );

    sendMessageButton = new QPushButton("Send");
    sendMessageButton->setFixedWidth(100);
    sendMessageButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2a82da;"
        "   color: white;"
        "   border-radius: 5px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3994ec;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1e5c99;"
        "}"
        );

    messageLayout->addWidget(messageInputField);
    messageLayout->addWidget(sendMessageButton);

    // Add widgets to main layout
    layout->addWidget(headerWidget);
    layout->addWidget(chatHistoryDisplay);
    layout->addWidget(messageWidget);

    // Connect signals
    connect(leaveChatButton, &QPushButton::clicked, this, &PrivateChatWidget::backToMenuRequested);
    connect(sendMessageButton, &QPushButton::clicked, this, &PrivateChatWidget::sendMessage);
    connect(messageInputField, &QLineEdit::returnPressed, this, &PrivateChatWidget::sendMessage);
    connect(chatHistoryDisplay->verticalScrollBar(), &QScrollBar::rangeChanged, this, &PrivateChatWidget::scrollToBottom);
}

void PrivateChatWidget::clearChatHistory()
{
    chatHistoryDisplay->clear();
}

QString PrivateChatWidget::formatTimestamp(const QDateTime &timestamp)
{
    QDateTime now = QDateTime::currentDateTime();
    if (timestamp.date() == now.date()) {
        return timestamp.toString("hh:mm AP");
    } else if (timestamp.date().year() == now.date().year()) {
        return timestamp.toString("MMM d, hh:mm AP");
    } else {
        return timestamp.toString("MMM d yyyy, hh:mm AP");
    }
}

void PrivateChatWidget::addSystemMessage(QDateTime msgTimestamp)
{
    QString html;
    // Create a text cursor at the end of the document
    QTextCursor cursor = chatHistoryDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);

    // Create a block format with center alignment
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignCenter);

    // Apply the block format
    cursor.insertBlock(blockFormat);

    QString dateStr = msgTimestamp.toString("yyyy-MM-dd");

    // Create the HTML for the system message
    html = "<div style='display: inline-block; background-color: #333; "
           "border-radius: 10px; padding: 3px 10px; margin: 2px 0;'>"
           "<span style='color:#9e9e9e;'>" + QString("---") + dateStr + QString("---") +  "</span></div><br>";

    cursor.insertHtml(html);

}

void PrivateChatWidget::addIncomingMessage(const QString &sender, const QString &email, const QString &message, const QDateTime &timestamp)
{
    QString formattedTimestamp = formatTimestamp(timestamp);
    chatHistoryDisplay->append(QString("<div style='margin: 16px 0; max-width: 70%; clear: both;'>"
                                     "<div style='float: left; background-color: transparent; padding: 8px 12px; border-radius: 12px; border-bottom-left-radius: 4px;'>"
                                     "<div style='margin-bottom: 4px;'>"
                                     "<span style='color:#81c784; font-weight: bold; font-size: 13px;'>%1</span><br>"
                                     "<span style='color:#9e9e9e; font-size: 11px;'>%2</span>"
                                     "</div>"
                                     "<div style='color: #ffffff; font-size: 13px; line-height: 1.4;'>%3</div>"
                                     "<div style='text-align: right; font-size: 10px; color: #888888; margin-top: 4px;'>%4</div>"
                                     "</div>"
                                     "<div style='clear: both;'></div>"
                                     "</div>")
                                 .arg(sender)
                                 .arg(email)
                                 .arg(message)
                                 .arg(formattedTimestamp));
}

void PrivateChatWidget::addOutgoingMessage(const QString &message, const QDateTime &timestamp)
{
    QString formattedTimestamp = formatTimestamp(timestamp);
    chatHistoryDisplay->append(QString("<div style='margin: 16px 0; max-width: 70%; clear: both;'>"
                                     "<div style='float: right; background-color: transparent; padding: 8px 12px; border-radius: 12px; border-bottom-right-radius: 4px;'>"
                                     "<div style='margin-bottom: 4px;'>"
                                     "<span style='color:#90caf9; font-weight: bold; font-size: 13px;'>You</span><br>"
                                     "<span style='color:#bbdefb; font-size: 11px;'>%1</span>"
                                     "</div>"
                                     "<div style='color: #ffffff; font-size: 13px; line-height: 1.4;'>%2</div>"
                                     "<div style='text-align: right; font-size: 10px; color: #bbdefb; margin-top: 4px;'>%3</div>"
                                     "</div>"
                                     "<div style='clear: both;'></div>"
                                     "</div>")
                                 .arg(userEmail)
                                 .arg(message)
                                 .arg(formattedTimestamp));
}

void PrivateChatWidget::sendMessage()
{
    QString message = messageInputField->text().trimmed();
    if (!message.isEmpty()) {
        // Save message to database
        if (dbHandler.sendDirectMessage(userEmail, recipientEmail, message)) {

            addOutgoingMessage(message, QDateTime::currentDateTime());
            messageInputField->clear();
        } else {
            QMessageBox::warning(this, "Error", "Failed to send message. Please try again.");
        }
    }
}

void PrivateChatWidget::loadChatHistory()
{
    // Clear existing chat history
    chatHistoryDisplay->clear();

    // Get chat history
    QList<std::tuple<QString, QString, QString, QDateTime>> messages =
        dbHandler.getDirectMessageHistory(userEmail, recipientEmail, 50);

    QString lastDate;
    // Display messages in UI
    for (const auto &message : messages) {
        const QString &senderName = std::get<0>(message);
        const QString &senderEmail = std::get<1>(message);
        const QString &content = std::get<2>(message);
        const QDateTime &timestamp = std::get<3>(message);

        // Add date separator if it's a new day
        QString dateStr = timestamp.toString("yyyy-MM-dd");
        if (lastDate != dateStr) {
            addSystemMessage(timestamp);

            lastDate = dateStr;
        }


        if (senderEmail == userEmail) {
            addOutgoingMessage(content, timestamp);
        } else {
            addIncomingMessage(senderName, senderEmail, content, timestamp);
        }
    }

    if (messages.isEmpty()) {
        chatHistoryDisplay->append("<center><span style='color:#777777;'>--- No messages yet ---</span></center>");
    }


    // Scroll to bottom to show latest messages
    scrollToBottom();
}

void PrivateChatWidget::scrollToBottom()
{
    QScrollBar *scrollBar = chatHistoryDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
