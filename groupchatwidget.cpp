#include "groupchatwidget.h"

GroupChatWidget::GroupChatWidget(ChatDatabaseHandler &dbHandler, QString groupName, QWidget *parent)
    : QWidget(parent), dbHandler(dbHandler), currentGroupName(groupName)
{
    setupUI();
    // Load chat history
    setupConnections();

    loadChatHistory();

    // Get the list of members for this group
    QStringList members = dbHandler.getGroupChatMembers(currentGroupName);
    setMembersList(members);
}

void GroupChatWidget::setupUI()
{
    // Create main layout with no margins
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ===== RIGHT SIDE: Current Chat Area =====
    QWidget *chatPanel = new QWidget();
    QVBoxLayout *chatLayout = new QVBoxLayout(chatPanel);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->setSpacing(0);

    // Chat header (top bar with back button, group name, members button)
    QWidget *chatHeader = new QWidget();
    chatHeader->setMinimumHeight(60);
    chatHeader->setStyleSheet("background-color: #1e1e1e; border-bottom: 1px solid #333;");

    QHBoxLayout *chatHeaderLayout = new QHBoxLayout(chatHeader);
    chatHeaderLayout->setContentsMargins(15, 10, 15, 10);

    backButton = new QPushButton();
    backButton->setIcon(QIcon::fromTheme("go-previous", QIcon(":/icons/back.png")));
    backButton->setToolTip("Go back to menu");
    backButton->setFixedSize(32, 32);
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setStyleSheet("QPushButton { background-color: #2979ff; border-radius: 16px; } "
                              "QPushButton:hover { background-color: #448aff; }");

    groupNameLabel = new QLabel("Group Chat");
    groupNameLabel->setFont(QFont("Arial", 14, QFont::Bold));
    groupNameLabel->setStyleSheet("color: #e0e0e0;");

    // Members button (to open a popup menu)
    membersButton = new QPushButton("Members");
    membersButton->setStyleSheet("QPushButton { background-color: #333; color: white; "
                                 "border-radius: 4px; padding: 6px 12px; } "
                                 "QPushButton:hover { background-color: #444; }");

    leaveChatButton = new QPushButton("Leave");
    leaveChatButton->setFixedWidth(70);
    leaveChatButton->setCursor(Qt::PointingHandCursor);
    leaveChatButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; "
                                   "border-radius: 4px; padding: 6px; } "
                                   "QPushButton:hover { background-color: #e53935; }");

    chatHeaderLayout->addWidget(backButton);
    chatHeaderLayout->addSpacing(10);
    chatHeaderLayout->addWidget(groupNameLabel);
    chatHeaderLayout->addStretch();
    chatHeaderLayout->addWidget(membersButton);
    chatHeaderLayout->addSpacing(10);
    chatHeaderLayout->addWidget(leaveChatButton);

    // Chat messages area
    chatHistoryDisplay = new QTextEdit();
    chatHistoryDisplay->setReadOnly(true);
    chatHistoryDisplay->setStyleSheet(
        "QTextEdit { background-color: #1a1a1a; border: none; "
        "padding: 15px; color: #e0e0e0; } "
        "QScrollBar:vertical { width: 8px; background: #232323; } "
        "QScrollBar::handle:vertical { background: #444; border-radius: 4px; min-height: 20px; }");
    chatHistoryDisplay->document()->setDefaultStyleSheet("body { color: #e0e0e0; }");

    // Message input area (bottom bar)
    QWidget *messageInputArea = new QWidget();
    messageInputArea->setMinimumHeight(60);
    messageInputArea->setStyleSheet("background-color: #232323;");

    QHBoxLayout *messageInputLayout = new QHBoxLayout(messageInputArea);
    messageInputLayout->setContentsMargins(15, 10, 15, 10);

    messageInputField = new QLineEdit();
    messageInputField->setPlaceholderText("Type your message here...");
    messageInputField->setStyleSheet(
        "QLineEdit { background-color: #333333; color: #e0e0e0; "
        "border: 1px solid #444; border-radius: 18px; padding: 8px 16px; } "
        "QLineEdit:focus { border: 1px solid #2979ff; }");
    messageInputField->setMinimumHeight(36);

    sendMessageButton = new QPushButton("Send");
    sendMessageButton->setFixedSize(70, 36);
    sendMessageButton->setCursor(Qt::PointingHandCursor);
    sendMessageButton->setStyleSheet(
        "QPushButton { background-color: #2979ff; color: white; "
        "border-radius: 18px; font-weight: bold; } "
        "QPushButton:hover { background-color: #448aff; }");

    messageInputLayout->addWidget(messageInputField);
    messageInputLayout->addWidget(sendMessageButton);

    // Add components to chat layout
    chatLayout->addWidget(chatHeader);
    chatLayout->addWidget(chatHistoryDisplay, 1); // Stretch so it fills the space
    chatLayout->addWidget(messageInputArea);

    // Add chat panel to the main layout
    mainLayout->addWidget(chatPanel);

    // Create members popup menu
    membersMenu = new QMenu(this);
    membersMenu->setStyleSheet(
        "QMenu { background-color: #333; color: #e0e0e0; border: 1px solid #444; padding: 5px; }"
        "QMenu::item { padding: 6px 25px 6px 20px; }"
        "QMenu::item:selected { background-color: #2979ff; }");
    membersButton->setMenu(membersMenu);

    // Create the members list widget (shown in menu)
    membersListWidget = new QListWidget(this);
    membersListWidget->setStyleSheet(
        "QListWidget { background-color: #333; color: #e0e0e0; border: none; }"
        "QListWidget::item { padding: 5px; }"
        "QListWidget::item:hover { background-color: #444; }");

    // Add members list to a QWidgetAction to show in the menu
    QWidgetAction *membersAction = new QWidgetAction(this);
    membersAction->setDefaultWidget(membersListWidget);
    membersMenu->addAction(membersAction);

    // Set entire widget background
    this->setStyleSheet("background-color: #1e1e1e;");
}


void GroupChatWidget::setupConnections()
{
    // Connect signals
    connect(backButton, &QPushButton::clicked, this, &GroupChatWidget::backRequested);
    connect(leaveChatButton, &QPushButton::clicked, this, &GroupChatWidget::leaveChatRequested);
    connect(sendMessageButton, &QPushButton::clicked, this, &GroupChatWidget::sendMessage);
    connect(messageInputField, &QLineEdit::returnPressed, this, &GroupChatWidget::sendMessage);
    connect(membersButton, &QPushButton::clicked, this, &GroupChatWidget::showMembersMenu);
}

void GroupChatWidget::showMembersMenu()
{
    if (membersMenu) {
        membersMenu->exec(membersButton->mapToGlobal(QPoint(0, membersButton->height())));
    }
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

void GroupChatWidget::loadChatHistory()
{
    clearChatHistory();

    // Fetch messages
    QList<std::tuple<QString, QString, QDateTime>> messages = dbHandler.getGroupMessageHistory(currentGroupName, 50);

    QString lastDate;
    QString chatText;

    for (const auto& msg : messages) {
        QString sender = std::get<0>(msg);
        QString content = std::get<1>(msg);
        QDateTime timestamp = std::get<2>(msg);

        // Add date separator if it's a new day
        QString dateStr = timestamp.toString("yyyy-MM-dd");
        if (lastDate != dateStr) {
            chatText += "<center><span style='color:#777777;'>--- " + dateStr + " ---</span></center><br>";
            lastDate = dateStr;
        }

        // Format differently based on if it's your message or someone else's
        bool isCurrentUser = (sender == "You" || sender == "CurrentUser"); // Replace with actual logic

        if (isCurrentUser) {
            // Right-aligned for current user's messages
            chatText += "<div style='text-align: right;'>";
            chatText += "<div style='display: inline-block; background-color: #2979ff; color: white; "
                        "border-radius: 15px; padding: 8px 12px; margin: 2px 0; max-width: 80%;'>";
            chatText += "<span style='font-size: 0.8em; color: #e6e6e6;'>"
                        + timestamp.toString("HH:mm") + "</span><br>";
            chatText += content;
            chatText += "</div></div><br>";
        } else {
            // Left-aligned for others' messages
            chatText += "<div style='text-align: left;'>";
            chatText += "<b style='color: #66bb6a;'>" + sender + "</b><br>";
            chatText += "<div style='display: inline-block; background-color: #383838; "
                        "border-radius: 15px; padding: 8px 12px; margin: 2px 0; max-width: 80%;'>";
            chatText += "<span style='font-size: 0.8em; color: #aaaaaa;'>"
                        + timestamp.toString("HH:mm") + "</span><br>";
            chatText += content;
            chatText += "</div></div><br>";
        }
    }

    if (messages.isEmpty()) {
        chatText = "<center><span style='color:#777777;'>--- No messages yet ---</span></center>";
    }

    chatHistoryDisplay->setHtml(chatText);

    // Scroll to the bottom
    QScrollBar *scrollbar = chatHistoryDisplay->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void GroupChatWidget::addSystemMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm");

    QString html = "<center><div style='display: inline-block; background-color: #333; "
                   "border-radius: 10px; padding: 5px 10px; margin: 5px 0;'>"
                   "<span style='color:#9e9e9e;'>" + timestamp +
                   " — " + message + "</span></div></center><br>";

    chatHistoryDisplay->insertHtml(html);

    // Auto-scroll to bottom
    QScrollBar *scrollbar = chatHistoryDisplay->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void GroupChatWidget::addMessage(const QString &sender, const QString &message)
{
    QDateTime timestamp = QDateTime::currentDateTime();

    // Format differently based on if it's your message or someone else's
    bool isCurrentUser = (sender == "You" || sender == "CurrentUser"); // Replace with actual logic

    QString html;
    if (isCurrentUser) {
        // Right-aligned for current user's messages
        html = "<div style='text-align: right;'>";
        html += "<div style='display: inline-block; background-color: #2979ff; color: white; "
                "border-radius: 15px; padding: 8px 12px; margin: 2px 0; max-width: 80%;'>";
        html += "<span style='font-size: 0.8em; color: #e6e6e6;'>"
                + timestamp.toString("HH:mm") + "</span><br>";
        html += message;
        html += "</div></div><br>";
    } else {
        // Left-aligned for others' messages
        html = "<div style='text-align: left;'>";
        html += "<b style='color: #66bb6a;'>" + sender + "</b><br>";
        html += "<div style='display: inline-block; background-color: #383838; "
                "border-radius: 15px; padding: 8px 12px; margin: 2px 0; max-width: 80%;'>";
        html += "<span style='font-size: 0.8em; color: #aaaaaa;'>"
                + timestamp.toString("HH:mm") + "</span><br>";
        html += message;
        html += "</div></div><br>";
    }

    chatHistoryDisplay->insertHtml(html);

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
        messageInputField->clear();

        // Display the message in the chat
        QString html = "<div style='text-align: right;'>";
        html += "<div style='display: inline-block; background-color: #2979ff; color: white; "
                "border-radius: 15px; padding: 8px 12px; margin: 2px 0; max-width: 80%;'>";
        html += "<span style='font-size: 0.8em; color: #e6e6e6;'>"
                + QDateTime::currentDateTime().toString("HH:mm") + "</span><br>";
        html += message;
        html += "</div></div><br>";

        chatHistoryDisplay->insertHtml(html);

        // Auto-scroll to bottom
        QScrollBar *scrollbar = chatHistoryDisplay->verticalScrollBar();
        scrollbar->setValue(scrollbar->maximum());

        // Emit the message for processing and saving to DB
        emit messageSubmitted(message);
    }
}
