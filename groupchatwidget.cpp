#include "groupchatwidget.h"

GroupChatWidget::GroupChatWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void GroupChatWidget::setupUI()
{
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create header
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);

    groupNameLabel = new QLabel("Group Chat");
    groupNameLabel->setFont(QFont("Arial", 14, QFont::Bold));

    leaveChatButton = new QPushButton("Leave Chat");
    leaveChatButton->setFixedWidth(100);

    headerLayout->addWidget(groupNameLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(leaveChatButton);

    // Create splitter for chat area and member list
    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    // Chat area widget
    QWidget *chatWidget = new QWidget();
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);

    // Chat history display
    chatHistoryDisplay = new QTextEdit();
    chatHistoryDisplay->setReadOnly(true);
    chatHistoryDisplay->setStyleSheet("background-color: #232323; border: 1px solid #3a3a3a; border-radius: 5px;");

    // Message input area
    QWidget *messageWidget = new QWidget();
    QHBoxLayout *messageLayout = new QHBoxLayout(messageWidget);

    messageInputField = new QLineEdit();
    messageInputField->setPlaceholderText("Type your message here...");
    messageInputField->setStyleSheet("padding: 8px;");

    sendMessageButton = new QPushButton("Send");
    sendMessageButton->setFixedWidth(80);

    messageLayout->addWidget(messageInputField);
    messageLayout->addWidget(sendMessageButton);

    // Add widgets to chat layout
    chatLayout->addWidget(chatHistoryDisplay);
    chatLayout->addWidget(messageWidget);

    // Members list widget
    QWidget *membersWidget = new QWidget();
    QVBoxLayout *membersLayout = new QVBoxLayout(membersWidget);

    QLabel *membersLabel = new QLabel("Members");
    membersLabel->setFont(QFont("Arial", 11, QFont::Bold));

    membersListWidget = new QListWidget();
    membersListWidget->setStyleSheet("background-color: #232323; border: 1px solid #3a3a3a; border-radius: 5px;");

    membersLayout->addWidget(membersLabel);
    membersLayout->addWidget(membersListWidget);

    // Add widgets to splitter
    splitter->addWidget(chatWidget);
    splitter->addWidget(membersWidget);
    splitter->setStretchFactor(0, 3);  // Chat area gets more space
    splitter->setStretchFactor(1, 1);  // Members list gets less space

    // Add all to main layout
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(splitter);

    // Connect signals
    connect(leaveChatButton, &QPushButton::clicked, this, &GroupChatWidget::leaveChatRequested);
    connect(sendMessageButton, &QPushButton::clicked, this, &GroupChatWidget::sendMessage);
    connect(messageInputField, &QLineEdit::returnPressed, this, &GroupChatWidget::sendMessage);
}

void GroupChatWidget::setGroupName(const QString &name)
{
    groupNameLabel->setText("Group: " + name);
}

void GroupChatWidget::setMembersList(const QStringList &members)
{
    membersListWidget->clear();
    membersListWidget->addItems(members);
}

void GroupChatWidget::addMember(const QString &username)
{
    membersListWidget->addItem(username);
}

void GroupChatWidget::removeMember(const QString &username)
{
    QList<QListWidgetItem*> items = membersListWidget->findItems(username, Qt::MatchExactly);
    for (QListWidgetItem* item : items) {
        delete membersListWidget->takeItem(membersListWidget->row(item));
    }
}

void GroupChatWidget::clearChatHistory()
{
    chatHistoryDisplay->clear();
}

void GroupChatWidget::addSystemMessage(const QString &message)
{
    chatHistoryDisplay->append("<center><span style='color:#9e9e9e;'>--- " +
                               message + " ---</span></center>");
}

void GroupChatWidget::addMessage(const QString &sender, const QString &message)
{
    chatHistoryDisplay->append("<span style='color:#66bb6a;'><b>" +
                               sender + ":</b></span> " + message);
}

void GroupChatWidget::sendMessage()
{
    if (!messageInputField->text().isEmpty()) {
        emit messageSubmitted(messageInputField->text());
        chatHistoryDisplay->append("<span style='color:#42a5f5;'><b>You:</b></span> " +
                                   messageInputField->text());
        messageInputField->clear();
    }
}
