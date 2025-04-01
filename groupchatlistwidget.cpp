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
        "   background: transparent;"
        "   border-radius: 8px;"
        "   margin: 5px;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: rgba(74, 74, 74, 0.7);"
        "}"
    );

    // Joined groups list
    joinedGroupsListWidget = new QListWidget(this);
    joinedGroupsListWidget->setStyleSheet(
        "QListWidget {"
        "   background-color: #363636;"
        "   border-radius: 12px;"
        "   border: 1px solid #404040;"
        "   padding: 12px;"
        "}"
        "QListWidget::item {"
        "   background: transparent;"
        "   border-radius: 8px;"
        "   margin: 5px;"
        "   color: #ffffff;"
        "}" 
    );

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
    QList<std::tuple<QString, QString, int>> groups = dbHandler.getCreatedGroups(userEmail);
    for (const auto &group : groups) {
        QString id = std::get<0>(group);
        QString name = std::get<1>(group);
        int memberCount = std::get<2>(group);
        addGroupItemWithEditButton(id, name, memberCount, createdGroupsListWidget);
    }
}

void GroupChatListWidget::loadJoinedGroups()
{
    joinedGroupsListWidget->clear();
    QList<std::tuple<QString, QString, int>> groups = dbHandler.getJoinedGroups(userEmail);

    for (const auto &group : groups) {
        QString id = std::get<0>(group);
        QString name = std::get<1>(group);
        int memberCount = std::get<2>(group);

        QWidget *itemWidget = new QWidget(joinedGroupsListWidget);
        itemWidget->setFixedHeight(50);
        itemWidget->setStyleSheet(
            "QWidget {"
            "   background-color: #404040;"
            "   border-radius: 8px;"
            "}"
        );
        
        QHBoxLayout *layout = new QHBoxLayout(itemWidget);
        layout->setContentsMargins(15, 5, 15, 5);
        layout->setSpacing(10);

        // Group icon
        QLabel *iconLabel = new QLabel(itemWidget);
        iconLabel->setPixmap(QPixmap(":/icons/group.png").scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        layout->addWidget(iconLabel);

        // Group info
        QVBoxLayout *infoLayout = new QVBoxLayout();
        QLabel *nameLabel = new QLabel(name, itemWidget);
        nameLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #ffffff;");
        
        QLabel *detailsLabel = new QLabel(QString("ID: %1 • %2 members").arg(id).arg(memberCount), itemWidget);
        detailsLabel->setStyleSheet("font-size: 12px; color: #a0a0a0;");
        
        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(detailsLabel);
        layout->addLayout(infoLayout, 1);

        QListWidgetItem *item = new QListWidgetItem(joinedGroupsListWidget);
        item->setSizeHint(itemWidget->sizeHint());
        item->setData(Qt::UserRole, id);
        joinedGroupsListWidget->setItemWidget(item, itemWidget);
    }
}

void GroupChatListWidget::addGroupItemWithEditButton(const QString &groupId, const QString &groupName, int memberCount, QListWidget *listWidget)
{
    QWidget *itemWidget = new QWidget(listWidget);
    itemWidget->setFixedHeight(50);  // Set fixed height for consistency
    itemWidget->setStyleSheet(
        "QWidget {"
        "   background-color: #404040;"
        "   border-radius: 8px;"
        "}"
    );
    
    QHBoxLayout *mainLayout = new QHBoxLayout(itemWidget);
    mainLayout->setContentsMargins(15, 5, 15, 5);
    mainLayout->setSpacing(10);

    // Left side - Group info
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(2);
    infoLayout->setContentsMargins(0, 0, 0, 0);

    // Group name and ID
    QLabel *nameLabel = new QLabel(groupName, itemWidget);
    nameLabel->setStyleSheet(
        "QLabel {"
        "   color: #ffffff;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 0;"
        "   background: transparent;"
        "}"
    );
    infoLayout->addWidget(nameLabel);

    // ID and member count in gray
    QLabel *detailsLabel = new QLabel(QString("ID: %1  •  %2 members").arg(groupId).arg(memberCount), itemWidget);
    detailsLabel->setStyleSheet(
        "QLabel {"
        "   color: #a0a0a0;"
        "   font-size: 12px;"
        "   padding: 0;"
        "   background: transparent;"
        "}"
    );
    infoLayout->addWidget(detailsLabel);
    mainLayout->addLayout(infoLayout, 1);

    // Right side - Edit button
    QPushButton *editButton = new QPushButton("Edit", itemWidget);
    editButton->setProperty("groupName", groupName);
    editButton->setCursor(Qt::PointingHandCursor);
    editButton->setFixedSize(35, 22);
    editButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 255, 255, 0.1);"
        "   border-radius: 3px;"
        "   padding: 0;"
        "   color: #ffffff;"
        "   font-size: 10px;"
        "   border: none;"
        "   margin: 0;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(255, 255, 255, 0.2);"
        "}"
    );
    mainLayout->addWidget(editButton);

    connect(editButton, &QPushButton::clicked, this, &GroupChatListWidget::onEditGroupNameClicked);

    QListWidgetItem *item = new QListWidgetItem(listWidget);
    item->setData(Qt::UserRole, groupId);
    item->setSizeHint(QSize(itemWidget->sizeHint().width(), 50));  // Fixed height
    listWidget->addItem(item);
    listWidget->setItemWidget(item, itemWidget);
}

void GroupChatListWidget::onEditGroupNameClicked()
{
    QPushButton *editButton = qobject_cast<QPushButton*>(sender());
    if (!editButton) return;

    QString oldGroupName = editButton->property("groupName").toString();
    bool ok;
    QString newGroupName = QInputDialog::getText(this, "Edit Group Name",
                                               "Enter new group name:",
                                               QLineEdit::Normal,
                                               oldGroupName, &ok);

    if (ok && !newGroupName.isEmpty() && newGroupName != oldGroupName) {
        // Update the group name in database
        // Note: You'll need to add this function to ChatDatabaseHandler
        if (dbHandler.updateGroupName(oldGroupName, newGroupName)) {
            refreshGroupLists();
        } else {
            QMessageBox::warning(this, "Error", "Failed to update group name.");
        }
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