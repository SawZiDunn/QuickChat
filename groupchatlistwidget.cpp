#include "groupchatlistwidget.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFont>
#include <QIcon>

GroupChatListWidget::GroupChatListWidget(ChatDatabaseHandler &dbHandler, const QString &userEmail, QWidget *parent)
    : QWidget(parent), dbHandler(dbHandler), userEmail(userEmail)
{
    setupUI();
    loadCreatedGroups();
    loadJoinedGroups();
}

void GroupChatListWidget::setupUI()
{
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Set widget background and font
    setStyleSheet(
        "QWidget { background-color: #2d2d2d; color: #ffffff; }"
        "QWidget { font-family: 'Segoe UI', Arial, sans-serif; }"
    );

    // Header section
    QHBoxLayout *headerLayout = new QHBoxLayout();

    // Back button with modern style
    backButton = new QPushButton("←", this);
    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #363636;"
        "   border-radius: 15px;"
        "   padding: 8px;"
        "   font-size: 18px;"
        "   min-width: 30px;"
        "   max-width: 30px;"
        "   color: #ffffff;"
        "}"
        "QPushButton:hover { background-color: #404040; }"
    );
    connect(backButton, &QPushButton::clicked, this, &GroupChatListWidget::backToMenuRequested);

    // Title with modern font
    titleLabel = new QLabel("Group Chats", this);
    titleLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #ffffff;"
        "margin: 10px;"
    );
    titleLabel->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(backButton);
    headerLayout->addWidget(titleLabel, 1);
    mainLayout->addLayout(headerLayout);

    // Tab widget for group lists
    tabWidget = new QTabWidget(this);
    setupTabStyle();

    // Created groups list
    createdGroupsListWidget = new QListWidget(this);
    createdGroupsListWidget->setStyleSheet(
        "QListWidget {"
        "   background-color: #363636;"
        "   border-radius: 10px;"
        "   border: none;"
        "   padding: 10px;"
        "}"
        "QListWidget::item {"
        "   background-color: #404040;"
        "   color: white;"
        "   border-radius: 8px;"
        "   margin: 5px;"
        "   padding: 15px;"
        "   font-size: 14px;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #4a4a4a;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #2196F3;"
        "}"
    );

    // Joined groups list
    joinedGroupsListWidget = new QListWidget(this);
    joinedGroupsListWidget->setStyleSheet(createdGroupsListWidget->styleSheet());

    // Connect signals
    connect(createdGroupsListWidget, &QListWidget::itemClicked, this, &GroupChatListWidget::onGroupItemClicked);
    connect(joinedGroupsListWidget, &QListWidget::itemClicked, this, &GroupChatListWidget::onGroupItemClicked);

    // Add tabs
    tabWidget->addTab(createdGroupsListWidget, "My Groups");
    tabWidget->addTab(joinedGroupsListWidget, "Joined Groups");

    mainLayout->addWidget(tabWidget);
}

void GroupChatListWidget::setupTabStyle()
{
    tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "   border: none;"
        "   background-color: #363636;"
        "   border-radius: 10px;"
        "   margin-top: -1px;"
        "}"
        "QTabBar::tab {"
        "   background-color: #404040;"
        "   color: #ffffff;"
        "   padding: 12px 25px;"
        "   margin-right: 2px;"
        "   border-top-left-radius: 8px;"
        "   border-top-right-radius: 8px;"
        "   font-weight: bold;"
        "}"
        "QTabBar::tab:selected {"
        "   background-color: #2196F3;"
        "}"
        "QTabBar::tab:hover:!selected {"
        "   background-color: #4a4a4a;"
        "}"
    );
}

void GroupChatListWidget::loadCreatedGroups()
{
    createdGroupsListWidget->clear();
    auto groups = dbHandler.getGroupDetails(userEmail);
    
    for (const auto &group : groups) {
        QString id = std::get<0>(group);
        QString name = std::get<1>(group);
        int memberCount = std::get<2>(group);
        
        QString displayText = QString("%1 (ID: %2)\n%3 members")
                                .arg(name)
                                .arg(id)
                                .arg(memberCount);

        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, id);  // store group ID
        createdGroupsListWidget->addItem(item);
    }
}

void GroupChatListWidget::loadJoinedGroups()
{
    joinedGroupsListWidget->clear();
    auto groups = dbHandler.getGroupDetails(userEmail);
    
    for (const auto &group : groups) {
        QString id = std::get<0>(group);
        QString name = std::get<1>(group);
        int memberCount = std::get<2>(group);
        
        QString displayText = QString("%1 (ID: %2)\n%3 members")
                                .arg(name)
                                .arg(id)
                                .arg(memberCount);

        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, id);  // store group ID
        joinedGroupsListWidget->addItem(item);
    }
}

void GroupChatListWidget::refreshGroupLists()
{
    loadCreatedGroups();
    loadJoinedGroups();
}

void GroupChatListWidget::onGroupItemClicked(QListWidgetItem *item)
{
    QString groupId = item->data(Qt::UserRole).toString();
    QString groupName = item->text();
    emit groupChatSelected(groupId, groupName);
}