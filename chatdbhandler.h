// chatdbhandler.h
#ifndef CHATDBHANDLER_H
#define CHATDBHANDLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QPair>
#include <QDateTime>
#include <QDebug>
#include <tuple>

class ChatDatabaseHandler : public QObject
{
    Q_OBJECT

public:
    explicit ChatDatabaseHandler(QObject *parent = nullptr);
    ~ChatDatabaseHandler();

    // Database setup
    bool initialize();

    // User operations
    QString loginUser(const QString &email, const QString &password);
    bool registerUser(const QString &username, const QString &email, const QString &password);
    QString userExists(const QString & email);

    // Chat group operations
    int createGroupChat(const QString &name, const QString &creatorEmail);
    bool joinGroupChat(const QString &userEmail, const QString &groupId);
    QList<std::tuple<QString, QString, int>> getCreatedGroups(const QString &userEmail) const;  // Returns (id, name, member_count) for groups created by user
    QList<std::tuple<QString, QString, int>> getJoinedGroups(const QString &userEmail) const;  // Returns (id, name, member_count) for groups joined by user
    QStringList getUserGroups(const QString &userEmail) const;
    QList<std::tuple<QString, QString, int>> getGroupDetails(const QString &userEmail) const;  // Returns (id, name, member_count)
    QString groupChatExists(const QString & chatId);
    QList<QPair<QString, QString>> getGroupChatMembers(const QString &chatName);
    bool removeUserFromGroup(const QString &email, const QString &groupName);
    QPair<QString, QString> getGroupAdmin(const QString &groupId);
    bool updateGroupName(const QString &oldName, const QString &newName);
    bool deleteGroup(const QString &groupId);
    bool isGroupMember(const QString &email, const QString &groupName);

    // Message operations
    bool sendDirectMessage(const QString &sender, const QString &recipient,
                           const QString &content);
    bool sendGroupMessage(const QString &sender, const QString &groupName,
                          const QString &content, const QString &type = "message");

    QList<std::tuple<QString, QString, QDateTime>> getDirectMessageHistory(const QString &user1, const QString &user2, int limit);
    QList<std::tuple<QString, QString, QString, QDateTime, QString>> getGroupMessageHistory(const QString &groupName, int limit);



private:
    QSqlDatabase db;
    bool dbInitialized;

    bool executeQuery(const QString &sql);
    bool checkTableExists(const QString &tableName);
};

#endif // CHATDBHANDLER_H
