#include "menuwidget.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QInputDialog>
#include <QMessageBox>

MenuWidget::MenuWidget(ChatDatabaseHandler& dbHandler, QStackedWidget *stackedWidget, QWidget *parent)
    : QWidget(parent), stackedWidget(stackedWidget), dbHandler(dbHandler)
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
    viewGroupChatsButton = new QPushButton("Group Chats");
    createGroupChatButton = new QPushButton("Create New Group");
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

void MenuWidget::startPrivateChat() {
    bool ok;
    QString userEmail = QInputDialog::getText(this, "Start Private Chat",
                                           "Enter user's email address:",
                                           QLineEdit::Normal, "", &ok);

    if (ok && !userEmail.isEmpty()) {
        QString userName = dbHandler.userExists(userEmail);
        if (userName != "") {
            // Pass the database handler reference
            PrivateChatWidget* privateChatWidget = new PrivateChatWidget(currentUser.second, userEmail, userName, dbHandler, this);

            // Connect the back button signal
            connect(privateChatWidget, &PrivateChatWidget::backToMenuRequested, this, [this, privateChatWidget]() {
                // Switch back to menu widget
                stackedWidget->setCurrentWidget(this);

                // Optional: Remove the chat widget to free up resources
                // This should be done after a delay or in a safe way to prevent crashes
                privateChatWidget->deleteLater();
            });

            stackedWidget->addWidget(privateChatWidget);
            stackedWidget->setCurrentWidget(privateChatWidget);

        } else {
            QMessageBox::warning(this, "User Not Found",
                              "No user with this email address was found.");
        }
    }
}


void MenuWidget::viewGroupChats() {
    // Delete existing widget if it exists
    // if (groupChatListWidget) {
    //     stackedWidget->removeWidget(groupChatListWidget);
    //     delete groupChatListWidget;
    // }

    // Create new widget with current user
    groupChatListWidget = new GroupChatListWidget(dbHandler, currentUser.second, this);
    
    // Connect back button signal
    connect(groupChatListWidget, &GroupChatListWidget::backToMenuRequested, [this]() {
        stackedWidget->setCurrentWidget(this);
    });
    
    // Connect group chat selection signal
    connect(groupChatListWidget, &GroupChatListWidget::groupChatSelected, 
            [this](const QString &groupId) {

        // Create and set up the group chat widget
        GroupChatWidget* groupChatWidget = new GroupChatWidget(dbHandler, groupId, currentUser, this);
        
        // Connect the back button signal
        connect(groupChatWidget, &GroupChatWidget::backRequested, this, [this, groupChatWidget]() {
            // Switch back to group list widget
            stackedWidget->setCurrentWidget(groupChatListWidget);
            groupChatWidget->deleteLater();
        });
        
        // Add and show the group chat widget
        stackedWidget->addWidget(groupChatWidget);
        stackedWidget->setCurrentWidget(groupChatWidget);
    });
    
    stackedWidget->addWidget(groupChatListWidget);
    stackedWidget->setCurrentWidget(groupChatListWidget);
}

void MenuWidget::createGroupChat() {
    bool ok;
    QString chatName = QInputDialog::getText(this, "Create New Group",
                                             "Enter a name for the new group chat:",
                                             QLineEdit::Normal, "", &ok);

    if (ok && !chatName.isEmpty()) {
        // Call dbHandler to create the group chat, which returns an integer ID

        int chatId = dbHandler.createGroupChat(chatName, currentUser.second);

        if (chatId > 0) {
            // Successfully created, convert the int ID to string for display
            QString chatIdStr = QString::number(chatId);

            QMessageBox::information(this, "Group Chat Created",
                                     "The group chat '" + chatName + "' has been created successfully.\n" +
                                         "Group Chat ID: " + chatIdStr);

        } else {
            QMessageBox::warning(this, "Create Group Chat",
                                 "Failed to create group chat. Please try again.");
        }
    }
}

void MenuWidget::joinGroupChat() {
    bool ok;
    QString chatId = QInputDialog::getText(this, "Join Group Chat",
                                             "Enter the group chat ID:",
                                             QLineEdit::Normal, "", &ok);
    if (ok && !chatId.isEmpty()) {
        // Check if the group chat exists
        QString chatName = dbHandler.groupChatExists(chatId);
        qDebug() << chatName;
        if (!chatName.isEmpty()) {
            // Add user to group chat in database
            // Create and set up the GroupChatWidget

            GroupChatWidget* groupChatWidget = new GroupChatWidget(dbHandler, chatId, currentUser, this);

            // Connect the back button signal
            connect(groupChatWidget, &GroupChatWidget::backRequested, this, [this, groupChatWidget]() {
                // Switch back to menu widget
                stackedWidget->setCurrentWidget(this);

                // Optional: Remove the chat widget to free up resources
                // This should be done after a delay or in a safe way to prevent crashes
                groupChatWidget->deleteLater();
            });

            // Switch to the GroupChatWidget in the stacked widget
            stackedWidget->addWidget(groupChatWidget);
            stackedWidget->setCurrentWidget(groupChatWidget);


        } else {
            QMessageBox::warning(this, "Group Chat Not Found",
                                 "No group chat with this name was found.");
        }
    }
}

void MenuWidget::logoutRequested() {
    currentUser = qMakePair("", "");
    stackedWidget->setCurrentWidget(stackedWidget->widget(0)); // welcomePage is in index 0 of stackedWidget
}
