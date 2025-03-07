// chatdbhandler.cpp
#include "chatdbhandler.h"
#include <QFile>

ChatDatabaseHandler::ChatDatabaseHandler(QObject *parent)
    : QObject(parent), dbInitialized(false)
{
}

ChatDatabaseHandler::~ChatDatabaseHandler()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool ChatDatabaseHandler::initialize()
{
    // Check if db is already initialized
    if (dbInitialized) {
        return true;
    }

    // Setup connection
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("chat_database.db");

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return false;
    }

    dbInitialized = true;
    return true;
}

bool ChatDatabaseHandler::isInitialized() const
{
    return dbInitialized;
}

bool ChatDatabaseHandler::executeQuery(const QString &sql)
{
    if (!dbInitialized) {
        qDebug() << "Database not initialized";
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec(sql)) {
        qDebug() << "Query failed:" << query.lastError().text();
        qDebug() << "SQL:" << sql;
        return false;
    }
    return true;
}

bool ChatDatabaseHandler::checkTableExists(const QString &tableName)
{
    if (!dbInitialized) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=:name");
    query.bindValue(":name", tableName);

    if (query.exec() && query.next()) {
        return true;
    }
    return false;
}

bool ChatDatabaseHandler::loginUser(const QString &email, const QString &password)
{
    if (!dbInitialized) {
        qDebug("hello");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id FROM users WHERE email = :email AND password = :password");
    query.bindValue(":email", email);
    query.bindValue(":password", password); // In real app, use hashed passwords

    if (query.exec() && query.next()) {
        return true; // User exists with matching password
    }
    return false;
}

bool ChatDatabaseHandler::registerUser(const QString &username, const QString &email, const QString &password)
{
    if (!dbInitialized) {
        return false;
    }
    // Check if username already exists
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT id FROM users WHERE name = :username OR email = :email");
    checkQuery.bindValue(":username", username);
    checkQuery.bindValue(":email", email);
    if (checkQuery.exec() && checkQuery.next()) {
        return false; // Username or email already exists
    }
    // Create new user
    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO users (name, email, password) VALUES (:username, :email, :password)");
    insertQuery.bindValue(":username", username);
    insertQuery.bindValue(":email", email);
    insertQuery.bindValue(":password", password); // In real app, hash passwords
    return insertQuery.exec();
}

QStringList ChatDatabaseHandler::getAllUsers() const
{
    QStringList users;

    if (!dbInitialized) {
        return users;
    }

    QSqlQuery query(db);
    if (query.exec("SELECT name FROM users ORDER BY name")) {
        while (query.next()) {
            users << query.value(0).toString();
        }
    }

    return users;
}

QStringList ChatDatabaseHandler::getUsersExcept(const QString &username) const
{
    QStringList users;

    if (!dbInitialized) {
        return users;
    }

    QSqlQuery query(db);
    query.prepare("SELECT name FROM users WHERE name != :username ORDER BY name");
    query.bindValue(":username", username);

    if (query.exec()) {
        while (query.next()) {
            users << query.value(0).toString();
        }
    }

    return users;
}

QStringList ChatDatabaseHandler::getGroupChats() const
{
    QStringList groups;

    if (!dbInitialized) {
        return groups;
    }

    QSqlQuery query(db);
    if (query.exec("SELECT name FROM chat_groups ORDER BY name")) {
        while (query.next()) {
            groups << query.value(0).toString();
        }
    }

    return groups;
}

bool ChatDatabaseHandler::createGroupChat(const QString &name)
{
    if (!dbInitialized) {
        return false;
    }

    // Check if group already exists
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT id FROM chat_groups WHERE name = :name");
    checkQuery.bindValue(":name", name);

    if (checkQuery.exec() && checkQuery.next()) {
        return false; // Group already exists
    }

    // Create new group
    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO chat_groups (name, created_at) VALUES (:name, :created_at)");
    insertQuery.bindValue(":name", name);
    insertQuery.bindValue(":created_at", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    return insertQuery.exec();
}

bool ChatDatabaseHandler::joinGroupChat(const QString &username, const QString &groupName)
{
    if (!dbInitialized) {
        return false;
    }

    // Get user ID
    QSqlQuery userQuery(db);
    userQuery.prepare("SELECT id FROM users WHERE name = :name");
    userQuery.bindValue(":name", username);

    if (!userQuery.exec() || !userQuery.next()) {
        return false; // User not found
    }

    int userId = userQuery.value(0).toInt();

    // Get group ID
    QSqlQuery groupQuery(db);
    groupQuery.prepare("SELECT id FROM chat_groups WHERE name = :name");
    groupQuery.bindValue(":name", groupName);

    if (!groupQuery.exec() || !groupQuery.next()) {
        return false; // Group not found
    }

    int groupId = groupQuery.value(0).toInt();

    // Check if user is already in the group
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT user_id FROM user_chat_groups WHERE user_id = :user_id AND chatgroup_id = :group_id");
    checkQuery.bindValue(":user_id", userId);
    checkQuery.bindValue(":group_id", groupId);

    if (checkQuery.exec() && checkQuery.next()) {
        return true; // User is already in the group
    }

    // Add user to group
    QSqlQuery joinQuery(db);
    joinQuery.prepare("INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (:user_id, :group_id)");
    joinQuery.bindValue(":user_id", userId);
    joinQuery.bindValue(":group_id", groupId);

    return joinQuery.exec();
}

QStringList ChatDatabaseHandler::getUserGroups(const QString &username) const
{
    QStringList groups;

    if (!dbInitialized) {
        return groups;
    }

    QSqlQuery query(db);
    query.prepare("SELECT g.name FROM chat_groups g "
                  "JOIN user_chat_groups ug ON g.id = ug.chatgroup_id "
                  "JOIN users u ON u.id = ug.user_id "
                  "WHERE u.name = :username "
                  "ORDER BY g.name");
    query.bindValue(":username", username);

    if (query.exec()) {
        while (query.next()) {
            groups << query.value(0).toString();
        }
    }

    return groups;
}

bool ChatDatabaseHandler::sendDirectMessage(const QString &sender, const QString &recipient, const QString &content)
{
    if (!dbInitialized || content.isEmpty()) {
        return false;
    }

    // Get sender ID
    QSqlQuery senderQuery(db);
    senderQuery.prepare("SELECT id FROM users WHERE name = :name");
    senderQuery.bindValue(":name", sender);

    if (!senderQuery.exec() || !senderQuery.next()) {
        return false; // Sender not found
    }

    int senderId = senderQuery.value(0).toInt();

    // Get recipient ID
    QSqlQuery recipientQuery(db);
    recipientQuery.prepare("SELECT id FROM users WHERE name = :name");
    recipientQuery.bindValue(":name", recipient);

    if (!recipientQuery.exec() || !recipientQuery.next()) {
        return false; // Recipient not found
    }

    int recipientId = recipientQuery.value(0).toInt();

    // Send message
    QSqlQuery messageQuery(db);
    messageQuery.prepare("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                         "VALUES (:sender_id, NULL, :recipient_id, :content, :timestamp)");
    messageQuery.bindValue(":sender_id", senderId);
    messageQuery.bindValue(":recipient_id", recipientId);
    messageQuery.bindValue(":content", content);
    messageQuery.bindValue(":timestamp", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    return messageQuery.exec();
}

bool ChatDatabaseHandler::sendGroupMessage(const QString &sender, const QString &groupName, const QString &content)
{
    if (!dbInitialized || content.isEmpty()) {
        return false;
    }

    // Get sender ID
    QSqlQuery senderQuery(db);
    senderQuery.prepare("SELECT id FROM users WHERE name = :name");
    senderQuery.bindValue(":name", sender);

    if (!senderQuery.exec() || !senderQuery.next()) {
        return false; // Sender not found
    }

    int senderId = senderQuery.value(0).toInt();

    // Get group ID
    QSqlQuery groupQuery(db);
    groupQuery.prepare("SELECT id FROM chat_groups WHERE name = :name");
    groupQuery.bindValue(":name", groupName);

    if (!groupQuery.exec() || !groupQuery.next()) {
        return false; // Group not found
    }

    int groupId = groupQuery.value(0).toInt();

    // Send message
    QSqlQuery messageQuery(db);
    messageQuery.prepare("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                         "VALUES (:sender_id, :group_id, NULL, :content, :timestamp)");
    messageQuery.bindValue(":sender_id", senderId);
    messageQuery.bindValue(":group_id", groupId);
    messageQuery.bindValue(":content", content);
    messageQuery.bindValue(":timestamp", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    return messageQuery.exec();
}

QList<QPair<QString, QString>> ChatDatabaseHandler::getDirectMessageHistory(const QString &user1, const QString &user2, int limit)
{
    QList<QPair<QString, QString>> messages;

    if (!dbInitialized) {
        return messages;
    }

    QSqlQuery query(db);
    query.prepare("SELECT u.name, m.content FROM messages m "
                  "JOIN users u ON m.sender_id = u.id "
                  "WHERE (m.sender_id = (SELECT id FROM users WHERE name = :user1) AND "
                  "       m.recipient_id = (SELECT id FROM users WHERE name = :user2)) OR "
                  "      (m.sender_id = (SELECT id FROM users WHERE name = :user2) AND "
                  "       m.recipient_id = (SELECT id FROM users WHERE name = :user1)) "
                  "ORDER BY m.timestamp DESC LIMIT :limit");
    query.bindValue(":user1", user1);
    query.bindValue(":user2", user2);
    query.bindValue(":limit", limit);

    if (query.exec()) {
        while (query.next()) {
            QString sender = query.value(0).toString();
            QString content = query.value(1).toString();
            messages.append(qMakePair(sender, content));
        }

        // Reverse to get chronological order
        std::reverse(messages.begin(), messages.end());
    }

    return messages;
}

QList<QPair<QString, QString>> ChatDatabaseHandler::getGroupMessageHistory(const QString &groupName, int limit)
{
    QList<QPair<QString, QString>> messages;
    if (!dbInitialized) {
        return messages;
    }
    QSqlQuery query(db);
    query.prepare("SELECT u.name, m.content FROM messages m "
                  "JOIN users u ON m.sender_id = u.id "
                  "JOIN chat_groups g ON m.chatgroup_id = g.id "
                  "WHERE g.name = :group_name "
                  "ORDER BY m.timestamp DESC LIMIT :limit");
    query.bindValue(":group_name", groupName);
    query.bindValue(":limit", limit);
    if (query.exec()) {
        while (query.next()) {
            QString sender = query.value(0).toString();
            QString content = query.value(1).toString();
            messages.append(qMakePair(sender, content));
        }
        // Reverse to get chronological order
        std::reverse(messages.begin(), messages.end());
    }
    return messages;
}
