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

class ChatDatabaseHandler : public QObject
{
    Q_OBJECT

public:
    explicit ChatDatabaseHandler(QObject *parent = nullptr);
    ~ChatDatabaseHandler();

    // Database setup
    bool initialize();
    bool isInitialized() const;

    // User operations
    bool loginUser(const QString &username, const QString &password);
    bool registerUser(const QString &username, const QString &email, const QString &password);
    QStringList getAllUsers() const;
    QStringList getUsersExcept(const QString &username) const;

    // Chat group operations
    QStringList getGroupChats() const;
    bool createGroupChat(const QString &name);
    bool joinGroupChat(const QString &username, const QString &groupName);
    QStringList getUserGroups(const QString &username) const;

    // Message operations
    bool sendDirectMessage(const QString &sender, const QString &recipient,
                           const QString &content);
    bool sendGroupMessage(const QString &sender, const QString &groupName,
                          const QString &content);
    QList<QPair<QString, QString>> getDirectMessageHistory(const QString &user1,
                                                           const QString &user2,
                                                           int limit = 50);
    QList<QPair<QString, QString>> getGroupMessageHistory(const QString &groupName,
                                                          int limit = 50);

private:
    QSqlDatabase db;
    bool dbInitialized;

    bool executeQuery(const QString &sql);
    bool checkTableExists(const QString &tableName);
};

#endif // CHATDBHANDLER_H
