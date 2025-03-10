#include "menuwidget.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QInputDialog>
#include <QMessageBox>

MenuWidget::MenuWidget(QStackedWidget *stackedWidget, QWidget *parent)
    : QWidget(parent), stackedWidget(stackedWidget)
{
    setupUI();
}

void MenuWidget::setupUI()
{
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Add title
    titleLabel = new QLabel("Main Menu");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Welcome message
    welcomeLabel = new QLabel();
    welcomeLabel->setAlignment(Qt::AlignCenter);
    QFont welcomeFont = welcomeLabel->font();
    welcomeFont.setPointSize(12);
    welcomeLabel->setFont(welcomeFont);
    welcomeLabel->setText("Welcome to Qt Chat Application");

    // Create card widget containers
    QWidget *cardContainer = new QWidget();
    QGridLayout *cardLayout = new QGridLayout(cardContainer);
    cardLayout->setSpacing(15);

    // Define button styles
    QString cardButtonStyle = "QPushButton {"
                              "  background-color: #3a3a3a;"
                              "  border-radius: 8px;"
                              "  padding: 15px;"
                              "  text-align: center;"
                              "  color: white;"
                              "}"
                              "QPushButton:hover {"
                              "  background-color: #4a4a4a;"
                              "}"
                              "QPushButton:pressed {"
                              "  background-color: #2a82da;"
                              "}";

    // Create buttons
    startPrivateChatButton = new QPushButton("Start Private Chat");
    viewGroupChatsButton = new QPushButton("View Group Chats");
    createGroupChatButton = new QPushButton("Create Group Chat");
    joinGroupChatButton = new QPushButton("Join Group Chat");

    // Style card buttons
    startPrivateChatButton->setStyleSheet(cardButtonStyle);
    viewGroupChatsButton->setStyleSheet(cardButtonStyle);
    createGroupChatButton->setStyleSheet(cardButtonStyle);
    joinGroupChatButton->setStyleSheet(cardButtonStyle);
    startPrivateChatButton->setFixedSize(200, 120);
    viewGroupChatsButton->setFixedSize(200, 120);
    createGroupChatButton->setFixedSize(200, 120);
    joinGroupChatButton->setFixedSize(200, 120);

    // Create logout button
    logoutButton = new QPushButton("Logout");
    logoutButton->setFixedWidth(120);

    // Connect buttons to signals and methods
    connect(startPrivateChatButton, &QPushButton::clicked, this, &MenuWidget::startPrivateChatRequested);
    connect(startPrivateChatButton, &QPushButton::clicked, this, &MenuWidget::startPrivateChat);

    connect(viewGroupChatsButton, &QPushButton::clicked, this, &MenuWidget::viewGroupChatsRequested);
    connect(viewGroupChatsButton, &QPushButton::clicked, this, &MenuWidget::viewGroupChats);

    connect(createGroupChatButton, &QPushButton::clicked, this, &MenuWidget::createGroupChatRequested);
    connect(createGroupChatButton, &QPushButton::clicked, this, &MenuWidget::createGroupChat);

    connect(joinGroupChatButton, &QPushButton::clicked, this, &MenuWidget::joinGroupChatRequested);
    connect(joinGroupChatButton, &QPushButton::clicked, this, &MenuWidget::joinGroupChat);

    connect(logoutButton, &QPushButton::clicked, this, &MenuWidget::logoutRequested);

    // Add buttons to card layout
    cardLayout->addWidget(startPrivateChatButton, 0, 0);
    cardLayout->addWidget(viewGroupChatsButton, 0, 1);
    cardLayout->addWidget(createGroupChatButton, 1, 0);
    cardLayout->addWidget(joinGroupChatButton, 1, 1);

    // Add widgets to layout
    layout->addSpacing(20);
    layout->addWidget(titleLabel);
    layout->addWidget(welcomeLabel);
    layout->addSpacing(30);
    layout->addWidget(cardContainer, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(logoutButton, 0, Qt::AlignCenter);
    layout->addStretch();
}

void MenuWidget::setUsername(const QString &username)
{
    welcomeLabel->setText("Welcome, " + username);
}

void MenuWidget::startPrivateChat()
{
    // Get access to users map (you'll need to add this as a member variable or pass it as a parameter)
    if (users.isEmpty() || users.size() <= 1) {
        QMessageBox::information(this, "No Users",
                                 "There are no other users available for chat.");
        return;
    }

    // For demonstration, we'll simulate choosing a user
    QStringList userList;
    for (auto it = users.begin(); it != users.end(); ++it) {
        if (it.value().first != currentUser) {
            userList.append(it.value().first);
        }
    }

    if (userList.isEmpty()) {
        QMessageBox::information(this, "No Users",
                                 "There are no other users available for chat.");
        return;
    }

    bool ok;
    QString selectedUser = QInputDialog::getItem(this, "Start Private Chat",
                                                 "Select a user to chat with:",
                                                 userList, 0, false, &ok);

    if (ok && !selectedUser.isEmpty()) {
        // Emit signal with selected user
        emit privateChatSelected(selectedUser);
    }
}

void MenuWidget::viewGroupChats()
{
    if (groupChats.isEmpty()) {
        QMessageBox::information(this, "No Group Chats",
                                 "There are no group chats available at the moment.");
        return;
    }

    QString chatList = "Available Group Chats:\n\n";
    for (const QString &chat : groupChats) {
        chatList += "- " + chat + "\n";
    }

    QMessageBox::information(this, "Group Chats", chatList);
}

void MenuWidget::createGroupChat()
{
    bool ok;
    QString chatName = QInputDialog::getText(this, "Create Group Chat",
                                             "Enter a name for the new group chat:",
                                             QLineEdit::Normal, "", &ok);

    if (ok && !chatName.isEmpty()) {
        if (groupChats.contains(chatName)) {
            QMessageBox::warning(this, "Create Group Chat",
                                 "A group chat with this name already exists.");
        } else {
            groupChats.append(chatName);
            QMessageBox::information(this, "Group Chat Created",
                                     "The group chat '" + chatName + "' has been created successfully.");
        }
    }
}

void MenuWidget::joinGroupChat()
{
    if (groupChats.isEmpty()) {
        QMessageBox::information(this, "No Group Chats",
                                 "There are no group chats available to join.");
        return;
    }

    bool ok;
    QString selectedChat = QInputDialog::getItem(this, "Join Group Chat",
                                                 "Select a group chat to join:",
                                                 groupChats, 0, false, &ok);

    if (ok && !selectedChat.isEmpty()) {
        QMessageBox::information(this, "Group Chat Joined",
                                 "You have joined the group chat '" + selectedChat + "'.");
        emit groupChatJoined(selectedChat);
    }
}

void MenuWidget::logoutRequested() {
    currentUser = "";
    stackedWidget->setCurrentWidget(stackedWidget->widget(0)); // welcomePage is in index 0 of stackedWidget
}
