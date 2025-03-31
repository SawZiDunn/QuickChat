#include "groupchatwidget.h"

GroupChatWidget::GroupChatWidget(ChatDatabaseHandler &dbHandler, QString groupId, QPair<QString, QString> currentUser, QWidget *parent)
    : QWidget(parent), dbHandler(dbHandler), currentUser(currentUser)
{
    setupUI();
    setGroupName(dbHandler.groupChatExists(groupId));

    // Load chat history
    setupConnections();
    loadChatHistory();

    // Setup auto-refresh timer
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &GroupChatWidget::loadChatHistory);
    refreshTimer->start(8000); // Refresh every 8 seconds



    // Get the, list of members for this group
    QStringList members = dbHandler.getGroupChatMembers(currentGroupName);
    setMembersList(members);

    // first - name, second -email
    if (!dbHandler.isGroupMember(currentUser.second, currentGroupName)) {

        // Add a system message about the user joining

        dbHandler.joinGroupChat(currentUser.second, groupId);
        QString systemMessage = QString("%1 has joined the group chat.").arg(currentUser.first);
        // Save system message to database with type 'system'
        dbHandler.sendGroupMessage(currentUser.second, currentGroupName, systemMessage, "system");

    }
}

GroupChatWidget::~GroupChatWidget()
{
    // Stop the timer when the widget is destroyed
    refreshTimer->stop();
    delete refreshTimer;
}

void GroupChatWidget::setupUI()
{
    // Create main layout with no margins
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // chat area
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

    backButton = new QPushButton("Back");
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setStyleSheet("QPushButton { background-color: #2979ff; border-radius: 6px; } "
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
                                   "border-radius: 4px; padding: 6px 12px; } "
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
        "border: 1px solid #444; border-radius: 18px; font-weight: bold; } "
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

QString GroupChatWidget::formatTimestamp(const QDateTime &timestamp)
{
    return timestamp.toString("hh:mm AP");
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

    // Add title/header
    QListWidgetItem *header = new QListWidgetItem("Group Members (" + QString::number(members.size()) + ")");
    header->setFlags(Qt::NoItemFlags);
    header->setForeground(QBrush(QColor("#bbbbbb")));
    header->setBackground(QBrush(QColor("#2a2a2a")));
    header->setTextAlignment(Qt::AlignCenter);
    membersListWidget->addItem(header);

    // Add separator
    QListWidgetItem *separator = new QListWidgetItem();
    separator->setFlags(Qt::NoItemFlags);
    membersListWidget->addItem(separator);
    membersListWidget->setItemWidget(separator, new QFrame());

    // Add members
    for (const QString &member : members) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(":/icons/user.png"), " " + member);
        item->setSizeHint(QSize(item->sizeHint().width(), 30)); // Set consistent height
        membersListWidget->addItem(item);
    }

    // Set width to accommodate content
    membersListWidget->setMinimumWidth(200);
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

    // Fetch messages with type
    QList<std::tuple<QString, QString, QString, QDateTime, QString>> messages =
        dbHandler.getGroupMessageHistory(currentGroupName, 50);

    QString lastDate;

    for (const auto& msg : messages) {
        QString sender = std::get<0>(msg);
        QString senderEmail = std::get<1>(msg);
        QString content = std::get<2>(msg);
        QDateTime timestamp = std::get<3>(msg);
        QString type = std::get<4>(msg);

        // Add date separator if it's a new day
        QString dateStr = timestamp.toString("yyyy-MM-dd");
        if (lastDate != dateStr) {
            // Create a new cursor at the end
            QTextCursor cursor = chatHistoryDisplay->textCursor();
            cursor.movePosition(QTextCursor::End);
            chatHistoryDisplay->setTextCursor(cursor);

            // Create a block format with center alignment
            QTextBlockFormat blockFormat;
            blockFormat.setAlignment(Qt::AlignCenter);

            // Apply the block format
            cursor.insertBlock(blockFormat);

            // Insert the date separator text with styling
            cursor.insertHtml("<span style='color:#777777;'>--- " + dateStr + " ---</span>");

            // Add another block with default alignment to reset formatting
            cursor.insertBlock();

            lastDate = dateStr;
        }

        // Format based on message type
        if (type == "system") {
            addSystemMessage(content, timestamp);
        } else {
            // Check if message is from current user
            bool isCurrentUser = (sender == currentUser.first || sender == currentUser.second);

            if (isCurrentUser) {
                addOutgoingMessage(content, timestamp);
            } else {
                addIncomingMessage(sender, senderEmail, content, timestamp); // Using sender as email temporarily
            }
        }

    }

    if (messages.isEmpty()) {
        chatHistoryDisplay->append("<center><span style='color:#777777;'>--- No messages yet ---</span></center>");
    }

    // Scroll to the bottom
    QScrollBar *scrollbar = chatHistoryDisplay->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void GroupChatWidget::addSystemMessage(const QString &message, QDateTime msgTimestamp)
{
    // Format the timestamp for display
    QString timestampStr = formatTimestamp(msgTimestamp);

    // Create a text cursor at the end of the document
    QTextCursor cursor = chatHistoryDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);

    // Create a block format with center alignment
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignCenter);

    // Apply the block format
    cursor.insertBlock(blockFormat);

    // Create the HTML for the system message
    QString html = "<div style='display: inline-block; background-color: #333; "
                   "border-radius: 10px; padding: 3px 10px; margin: 2px 0;'>"
                   "<span style='color:#9e9e9e;'>" + timestampStr +
                   " — " + message + "</span></div><br>";

    // Insert the HTML at the cursor position
    cursor.insertHtml(html);

    // Auto-scroll to bottom
    QScrollBar *scrollbar = chatHistoryDisplay->verticalScrollBar();
    scrollbar->setValue(scrollbar->maximum());
}

void GroupChatWidget::addIncomingMessage(const QString &sender, const QString &email, const QString &message, QDateTime msgTimestamp)
{
    QString timestamp = formatTimestamp(msgTimestamp);
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
                                   .arg(timestamp));
}

void GroupChatWidget::addOutgoingMessage(const QString &message, QDateTime msgTimestamp)
{
    QString timestamp = formatTimestamp(msgTimestamp);
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
                                   .arg(currentUser.second)
                                   .arg(message)
                                   .arg(timestamp));
}

void GroupChatWidget::sendMessage()
{
    if (!messageInputField->text().isEmpty()) {
        QString message = messageInputField->text();
        messageInputField->clear();

        // Save to database
        bool success = dbHandler.sendGroupMessage(currentUser.second, currentGroupName, message);

        if (success) {
            // Add to UI only after successful database insertion
            QDateTime currentTime = QDateTime::currentDateTime();
            addOutgoingMessage(message, currentTime);

            // Auto-scroll to bottom
            QScrollBar *scrollbar = chatHistoryDisplay->verticalScrollBar();
            scrollbar->setValue(scrollbar->maximum());

            // Emit the message for processing
            emit messageSubmitted(message);

            // Reset the refresh timer to avoid immediate refresh after sending
            refreshTimer->stop();
            refreshTimer->start(8000);
        }
        else {
            // Handle database error
            messageInputField->setText(message); // Put the message back in the input field
            QMessageBox::warning(this, "Error", "Failed to send message. Please try again.");
        }
    }
}

// slot
void GroupChatWidget::leaveChatRequested()
{
    // Ask for confirmation before leaving
    QMessageBox confirmBox;
    confirmBox.setWindowTitle("Leave Chat");
    confirmBox.setText("Are you sure you want to leave this group chat?");
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);

    int result = confirmBox.exec();

    if (result == QMessageBox::Yes) {
        // Remove user from the group in database
        bool success = dbHandler.removeUserFromGroup(currentUser.second, currentGroupName);

        if (success) {
            // Notify other users by adding a system message
            QString leaveMessage = currentUser.first + " has left the group";

            dbHandler.sendGroupMessage(currentUser.second, currentGroupName, leaveMessage, "system");

            // Emit signal to go back to the main menu or group list
            emit backRequested();
        } else {
            QMessageBox errorBox;
            errorBox.setWindowTitle("Error");
            errorBox.setText("Failed to leave the group. Please try again.");
            errorBox.setIcon(QMessageBox::Warning);
            errorBox.exec();
        }
    }
}
