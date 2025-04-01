#ifndef GROUPCHATLISTWIDGET_H
#define GROUPCHATLISTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QScrollArea>
#include <QTabWidget>
#include "chatdbhandler.h"

class GroupChatListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GroupChatListWidget(ChatDatabaseHandler &dbHandler, const QString &userEmail, QWidget *parent = nullptr);
    void refreshGroupLists();

signals:
    void backToMenuRequested();
    void groupChatSelected(const QString &groupId, const QString &groupName);

private slots:
    void onGroupItemClicked(QListWidgetItem *item);

private:
    void setupUI();
    void loadCreatedGroups();
    void loadJoinedGroups();
    void setupTabStyle();

    ChatDatabaseHandler &dbHandler;
    QString userEmail;
    
    // UI Components
    QLabel *titleLabel;
    QPushButton *backButton;
    QTabWidget *tabWidget;
    QListWidget *createdGroupsListWidget;
    QListWidget *joinedGroupsListWidget;
};

#endif // GROUPCHATLISTWIDGET_H