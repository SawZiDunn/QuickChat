#include "groupchatwidget.h"
#include <QDateTime>
#include <QScrollBar>

GroupChatWidget::GroupChatWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void GroupChatWidget::setupUI()
{
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // Create header
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 10);

    // Add back button
    backButton = new QPushButton();
    backButton->setIcon(QIcon::fromTheme("go-previous", QIcon(":/icons/back.png")));
    backButton->setToolTip("Go back to chat list");
    backButton->setFixedSize(32, 32);
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setStyleSheet("QPushButton { background-color: #2979ff; border-radius: 16px; } "
                              "QPushButton:hover { background-color: #448aff; }");

    groupNameLabel = new QLabel("Group Chat");
    groupNameLabel->setFont(QFont("Arial", 14, QFont::Bold));
    groupNameLabel->setStyleSheet("color: #e0e0e0;");

    leaveChatButton = new QPushButton("Leave Chat");
    leaveChatButton->setFixedWidth(100);
    leaveChatButton->setCursor(Qt::PointingHandCursor);
    leaveChatButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; "
                                   "border-radius: 4px; padding: 6px; } "
                                   "QPushButton:hover { background-color: #e53935; }");

    headerLayout->addWidget(backButton);
    headerLayout->addSpacing(10);
    headerLayout->addWidget(groupNameLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(leaveChatButton);

    // Add a horizontal line under the header
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #3a3a3a;");

    // Create splitter for chat area and member list
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setHandleWidth(1);
    splitter->setStyleSheet("QSplitter::handle { background-color: #3a3a3a; }");

    // Chat area widget
    QWidget *chatWidget = new QWidget();
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->setSpacing(10);

    // Chat history display
    chatHistoryDisplay = new QTextEdit();
    chatHistoryDisplay->setReadOnly(true);
    chatHistoryDisplay->setStyleSheet("QTextEdit { background-color: #2d2d2d; border: 1px solid #3a3a3a; "
                                      "border-radius: 6px; padding: 8px; color: #e0e0e0; } "
                                      "QScrollBar:vertical { width: 12px; background: #232323; } "
                                      "QScrollBar::handle:vertical { background: #444; border-radius: 6px; min-height: 20px; }");
    chatHistoryDisplay->document()->setDefaultStyleSheet("body { color: #e0e0e0; }");

    // Message input area
    QWidget *messageWidget = new QWidget();
    QHBoxLayout *messageLayout = new QHBoxLayout(messageWidget);
    messageLayout->setContentsMargins(0, 0, 0, 0);
    messageLayout->setSpacing(8);

    messageInputField = new QLineEdit();
    messageInputField->setPlaceholderText("Type your message here...");
    messageInputField->setStyleSheet("QLineEdit { background-color: #333333; color: #e0e0e0; "
                                     "border: 1px solid #444; border-radius: 18px; padding: 8px 16px; } "
                                     "QLineEdit:focus { border: 1px solid #2979ff; }");
    messageInputField->setMinimumHeight(36);

    sendMessageButton = new QPushButton("Send");
    sendMessageButton->setFixedSize(80, 36);
    sendMessageButton->setCursor(Qt::PointingHandCursor);
    sendMessageButton->setStyleSheet("QPushButton { background-color: #2979ff; color: white; "
                                     "border-radius: 18px; font-weight: bold; } "
                                     "QPushButton:hover { background-color: #448aff; }");

    messageLayout->addWidget(messageInputField);
    messageLayout->addWidget(sendMessageButton);

    // Add widgets to chat layout
    chatLayout->addWidget(chatHistoryDisplay);
    chatLayout->addWidget(messageWidget);

    // Members list widget
    QWidget *membersWidget = new QWidget();
    QVBoxLayout *membersLayout = new QVBoxLayout(membersWidget);
    membersLayout->setContentsMargins(10, 0, 0, 0);
    membersLayout->setSpacing(10);

    QLabel *membersLabel = new QLabel("Members");
    membersLabel->setFont(QFont("Arial", 12, QFont::Bold));
    membersLabel->setStyleSheet("color: #e0e0e0;");

    membersListWidget = new QListWidget();
    membersListWidget->setStyleSheet("QListWidget { background-color: #2d2d2d; border: 1px solid #3a3a3a; "
                                     "border-radius: 6px; padding: 4px; color: #e0e0e0; } "
                                     "QListWidget::item { padding: 6px; border-radius: 3px; } "
                                     "QListWidget::item:hover { background-color: #3a3a3a; } "
                                     "QScrollBar:vertical { width: 12px; background: #232323; } "
                                     "QScrollBar::handle:vertical { background: #444; border-radius: 6px; min-height: 20px; }");

    membersLayout->addWidget(membersLabel);
    membersLayout->addWidget(membersListWidget);

    // Add widgets to splitter
    splitter->addWidget(chatWidget);
    splitter->addWidget(membersWidget);
    splitter->setStretchFactor(0, 4);  // Chat area gets more space
    splitter->setStretchFactor(1, 1);  // Members list gets less space

    // Add all to main layout
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(line);
    mainLayout->addWidget(splitter);

    // Connect signals
    connect(backButton, &QPushButton::clicked, this, &GroupChatWidget::backRequested);
    connect(leaveChatButton, &QPushButton::clicked, this, &GroupChatWidget::leaveChatRequested);
    connect(sendMessageButton, &QPushButton::clicked, this, &GroupChatWidget::sendMessage);
    connect(messageInputField, &QLineEdit::returnPressed, this, &GroupChatWidget::sendMessage);

    // Set entire widget background
    this->setStyleSheet("background-color: #1e1e1e;");
}

void GroupChatWidget::setGroupName(const QString &name)
{
    currentGroupName = name;
    groupNameLabel->setText("Group: " + name);
}

QString GroupChatWidget::getGroupName() const
{
    return currentGroupName;
}

void GroupChatWidget::setMembersList(const QStringList &members)
{
    membersListWidget->clear();
    for (const QString &member : members) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(":/icons/user.png"), member);
        membersListWidget->addItem(item);
    }
}

void GroupChatWidget::addMember(const QString &username)
{
    QListWidgetItem *item = new QListWidgetItem(QIcon(":/icons/user.png"), username);
    membersListWidget->addItem(item);
    addSystemMessage(username + " joined the group");
}

void GroupChatWidget::removeMember(const QString &username)
{
    QList<QListWidgetItem*> items = membersListWidget->findItems(username, Qt::MatchExactly);
    for (QListWidgetItem* item : items) {
        delete membersListWidget->takeItem(membersListWidget->row(item));
    }
    addSystemMessage(username + " left the group");
}

void GroupChatWidget::clearChatHistory()
{
    chatHistoryDisplay->clear();
}

void GroupChatWidget::loadChatHistory(QList<std::tuple<QString, QString, QDateTime>> messages)
{
    clearChatHistory();

    // Fetch messages


    QString chatText;

    for (const auto& msg : messages) {
        QString sender = std::get<0>(msg);
        QString content = std::get<1>(msg);
        QDateTime timestamp = std::get<2>(msg);

        // Append formatted message
        chatText += QString("<b>%1</b> [%2]: %3<br>")
                        .arg(sender)
                        .arg(timestamp.toString("yyyy-MM-dd HH:mm:ss"))
                        .arg(content);
    }

    chatHistoryDisplay->setHtml(chatText);  // Display messages

    // Scroll to the bottom
    QScrollBar *scrollbar = chatHistoryDisplay->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());

    addSystemMessage("Loaded chat history for general");
}



void GroupChatWidget::addSystemMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss]");
    chatHistoryDisplay->append("<center><span style='color:#9e9e9e;'>" + timestamp +
                               " --- " + message + " ---</span></center>");

    // Auto-scroll to bottom
    QScrollBar *scrollbar = chatHistoryDisplay->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void GroupChatWidget::addMessage(const QString &sender, const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss]");
    chatHistoryDisplay->append("<span style='color:#aaaaaa;'>" + timestamp + "</span> "
                                                                             "<span style='color:#66bb6a;'><b>" +
                               sender + ":</b></span> " + message);

    // Auto-scroll to bottom
    QScrollBar *scrollbar = chatHistoryDisplay->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void GroupChatWidget::addHistoryMessage(const QString &sender, const QString &message, const QDateTime &timestamp)
{
    QString timeStr = timestamp.toString("[hh:mm:ss]");
    QString dateStr = timestamp.toString("yyyy-MM-dd");

    // If this is the first message of a day or the first message overall, add a date separator
    static QString lastDate;
    if (lastDate != dateStr) {
        chatHistoryDisplay->append("<center><span style='color:#777777;'>--- " +
                                   dateStr + " ---</span></center>");
        lastDate = dateStr;
    }

    // Format history messages with a different color from current session messages
    chatHistoryDisplay->append("<span style='color:#aaaaaa;'>" + timeStr + "</span> "
                                                                           "<span style='color:#4db6ac;'><b>" +
                               sender + ":</b></span> " + message);
}

void GroupChatWidget::sendMessage()
{
    if (!messageInputField->text().isEmpty()) {
        QString message = messageInputField->text();

        // Clear the input field before emitting the signal
        messageInputField->clear();

        // Format current user messages differently
        QString timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss]");
        chatHistoryDisplay->append("<span style='color:#aaaaaa;'>" + timestamp + "</span> "
                                                                                 "<span style='color:#42a5f5;'><b>You:</b></span> " + message);

        // Auto-scroll to bottom
        QScrollBar *scrollbar = chatHistoryDisplay->verticalScrollBar();
        scrollbar->setValue(scrollbar->maximum());

        // Emit the message for processing and saving to DB
        emit messageSubmitted(message);
    }
}
