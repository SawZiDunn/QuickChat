// chatdbhandler.cpp
#include "chatdbhandler.h"

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

QString ChatDatabaseHandler::loginUser(const QString &email, const QString &password)
{
    if (!dbInitialized) {
        qDebug() << "Database not initialized";
        return QString(); // Return empty string on failure
    }

    QSqlQuery query(db);
    query.prepare("SELECT name FROM users WHERE email = :email AND password = :password");
    query.bindValue(":email", email);
    query.bindValue(":password", password); // In real app, use hashed passwords

    if (query.exec() && query.next()) {
        QString userName = query.value(0).toString();
        qDebug() << "Login successful for user:" << userName;
        return userName;
    }

    qDebug() << "Login failed for email:" << email;
    return QString(); // Return empty string if login fails
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

QString ChatDatabaseHandler::userExists(const QString & email) {
    if (!dbInitialized) {
        return QString();
    }

    QSqlQuery query(db);
    query.prepare("SELECT name FROM users WHERE email = :email");
    query.bindValue(":email", email);


    if (query.exec() && query.next()) {
        return query.value(0).toString(); // User exists with matching password
    }
    return QString();

}

QString ChatDatabaseHandler::groupChatExists(const QString & chatId) {
    if (!dbInitialized) {
        return QString();
    }


    QSqlQuery query(db);
    query.prepare("SELECT name FROM chat_groups WHERE id = :chatId");
    query.bindValue(":chatId", chatId);


    if (query.exec() && query.next()) {

        return query.value(0).toString(); // Group exists
    }
    return QString();

}




int ChatDatabaseHandler::createGroupChat(const QString &name, const QString &creatorEmail)
{
    if (!dbInitialized) {
        return -1; // Return -1 to indicate failure
    }


    // First, find the user ID for the given email
    QSqlQuery userQuery(db);
    userQuery.prepare("SELECT id FROM users WHERE email = :creatorEmail");

    userQuery.bindValue(":creatorEmail", creatorEmail);


    if (!userQuery.exec() || !userQuery.next()) {

        return -1; // No user found with this email
    }

    int creatorId = userQuery.value(0).toInt();


    // Check if group already exists
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT id FROM chat_groups WHERE name = :name");
    checkQuery.bindValue(":name", name);
    if (checkQuery.exec() && checkQuery.next()) {
        return checkQuery.value(0).toInt(); // Return existing group ID
    }

    // Create new group
    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO chat_groups (name, created_at, created_by) VALUES (:name, :created_at, :created_by)");
    insertQuery.bindValue(":name", name);
    insertQuery.bindValue(":created_at", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    insertQuery.bindValue(":created_by", creatorId);

    if (!insertQuery.exec()) {
        return -1; // Return -1 if insertion fails
    }

    // Get the ID of the newly created group
    int newGroupId = insertQuery.lastInsertId().toInt();

    // Automatically add the creator to the group
    QSqlQuery addCreatorQuery(db);
    addCreatorQuery.prepare("INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (:user_id, :chatgroup_id)");
    addCreatorQuery.bindValue(":user_id", creatorId);
    addCreatorQuery.bindValue(":chatgroup_id", newGroupId);

    if (!addCreatorQuery.exec()) {
        // Optionally, you might want to delete the group if adding the creator fails
        return -1;
    }

    return newGroupId;
}


bool ChatDatabaseHandler::joinGroupChat(const QString &userEmail, const QString &groupId)
{
    if (!dbInitialized) {
        return false;
    }

    // Get user ID
    QSqlQuery userQuery(db);
    userQuery.prepare("SELECT id FROM users WHERE email = :email");
    userQuery.bindValue(":email", userEmail);

    if (!userQuery.exec() || !userQuery.next()) {
        return false; // User not found
    }

    int userId = userQuery.value(0).toInt();

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

QStringList ChatDatabaseHandler::getUserGroups(const QString &userEmail) const
{
    QStringList groups;

    if (!dbInitialized) {
        return groups;
    }

    QSqlQuery query(db);
    query.prepare("SELECT g.name FROM chat_groups g "
                  "JOIN user_chat_groups ug ON g.id = ug.chatgroup_id "
                  "JOIN users u ON u.id = ug.user_id "
                  "WHERE u.email = :userEmail "
                  "ORDER BY g.name");
    query.bindValue(":userEmail", userEmail);

    if (query.exec()) {
        while (query.next()) {
            groups << query.value(0).toString();
        }
    }

    return groups;
}

QList<QPair<QString, QString>> ChatDatabaseHandler::getGroupChatMembers(const QString &chatName) {
    QList<QPair<QString, QString>> members;
    if (!dbInitialized) {
        return members;
    }

    QSqlQuery query(db);
    query.prepare("SELECT u.name, u.email FROM users u "
                  "JOIN user_chat_groups ug ON u.id = ug.user_id "
                  "JOIN chat_groups cg ON ug.chatgroup_id = cg.id "
                  "WHERE cg.name = :chatName "
                  "ORDER BY u.name");
    query.bindValue(":chatName", chatName);

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(0).toString();
            QString email = query.value(1).toString();
            members.append(QPair<QString, QString>(name, email));
        }
    } else {
        qDebug() << "Error fetching group members:" << query.lastError().text();
    }

    return members;
}

bool ChatDatabaseHandler::sendDirectMessage(const QString &sender, const QString &recipient, const QString &content)
{
    if (!dbInitialized || content.isEmpty()) {
        return false;
    }

    // Get sender ID
    QSqlQuery senderQuery(db);
    senderQuery.prepare("SELECT id FROM users WHERE email = :email");
    senderQuery.bindValue(":email", sender);

    if (!senderQuery.exec() || !senderQuery.next()) {
        qDebug() << "Sender not found:" << sender;
        return false; // Sender not found
    }

    int senderId = senderQuery.value(0).toInt();

    // Get recipient ID
    QSqlQuery recipientQuery(db);
    recipientQuery.prepare("SELECT id FROM users WHERE email = :email");
    recipientQuery.bindValue(":email", recipient);

    if (!recipientQuery.exec() || !recipientQuery.next()) {
        qDebug() << "Recipient not found:" << recipient;
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

    if (!messageQuery.exec()) {
        qDebug() << "Failed to insert message:" << messageQuery.lastError().text();
        return false;
    }

    return true;
}

bool ChatDatabaseHandler::sendGroupMessage(const QString &sender, const QString &groupName,
                                           const QString &content, const QString &type)
{

    if (!dbInitialized || content.isEmpty()) {
        return false;
    }

    // Get sender ID
    QSqlQuery senderQuery(db);
    senderQuery.prepare("SELECT id FROM users WHERE email = :email");
    senderQuery.bindValue(":email", sender);
    if (!senderQuery.exec() || !senderQuery.next()) {
        qDebug() << sender;

        return false; // Sender not found
    }
    int senderId = senderQuery.value(0).toInt();

    // Get group ID
    QSqlQuery groupQuery(db);
    groupQuery.prepare("SELECT id FROM chat_groups WHERE name = :name");
    groupQuery.bindValue(":name", groupName);
    if (!groupQuery.exec() || !groupQuery.next()) {
        qDebug() << "gpnot found";
        return false; // Group not found

    }
    int groupId = groupQuery.value(0).toInt();

    // Send message
    QSqlQuery messageQuery(db);
    messageQuery.prepare("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp, type) "
                         "VALUES (:sender_id, :group_id, NULL, :content, :timestamp, :type)");
    messageQuery.bindValue(":sender_id", senderId);
    messageQuery.bindValue(":group_id", groupId);
    messageQuery.bindValue(":content", content);
    messageQuery.bindValue(":timestamp", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    messageQuery.bindValue(":type", type);
    return messageQuery.exec();
}

// Using std::tuple
QList<std::tuple<QString, QString, QDateTime>> ChatDatabaseHandler::getDirectMessageHistory(const QString &user1, const QString &user2, int limit)
{
    QList<std::tuple<QString, QString, QDateTime>> messages;
    if (!dbInitialized) {
        return messages;
    }

    QSqlQuery query(db);
    query.prepare("SELECT u.email, m.content, m.timestamp FROM messages m "
                  "JOIN users u ON m.sender_id = u.id "
                  "WHERE (m.sender_id = (SELECT id FROM users WHERE email = :user1) AND "
                  "       m.recipient_id = (SELECT id FROM users WHERE email = :user2)) OR "
                  "      (m.sender_id = (SELECT id FROM users WHERE email = :user2) AND "
                  "       m.recipient_id = (SELECT id FROM users WHERE email = :user1)) "
                  "ORDER BY m.timestamp DESC LIMIT :limit");
    query.bindValue(":user1", user1);
    query.bindValue(":user2", user2);
    query.bindValue(":limit", limit);

    if (query.exec()) {
        while (query.next()) {
            QString sender = query.value(0).toString();
            QString content = query.value(1).toString();
            QDateTime timestamp = query.value(2).toDateTime();
            

            messages.append(std::make_tuple(sender, content, timestamp));
        }
        // Reverse to get chronological order
        std::reverse(messages.begin(), messages.end());
    } else {
        qDebug() << "Query failed:" << query.lastError().text();
        qDebug() << "Query string:" << query.lastQuery(); // Debug line
    }

    return messages;
}


QList<std::tuple<QString, QString, QString, QDateTime, QString>> ChatDatabaseHandler::getGroupMessageHistory(const QString &groupName, int limit)
{
    QList<std::tuple<QString, QString, QString, QDateTime, QString>> messages;

    QSqlQuery query(db);
    query.prepare("SELECT u.name, u.email, m.content, m.timestamp, m.type FROM messages m "
                  "JOIN users u ON m.sender_id = u.id "
                  "JOIN chat_groups g ON m.chatgroup_id = g.id "
                  "WHERE g.name = :groupName "
                  "ORDER BY m.timestamp ASC LIMIT :limit");

    query.bindValue(":groupName", groupName);
    query.bindValue(":limit", limit);

    if (query.exec()) {
        while (query.next()) {
            QString sender = query.value(0).toString();
            QString senderEmail = query.value(1).toString();
            QString content = query.value(2).toString();
            QDateTime timestamp = QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd hh:mm:ss");
            QString type = query.value(4).toString();  // Get the type

            messages.append(std::make_tuple(sender, senderEmail, content, timestamp, type));
        }
    }


    return messages;
}

bool ChatDatabaseHandler::isGroupMember(const QString &email, const QString &groupName)
{
    if (!dbInitialized) {
        return false;
    }


    QSqlQuery query(db);
    query.prepare("SELECT user_id FROM user_chat_groups ucg "
                  "JOIN users u ON ucg.user_id = u.id "
                  "JOIN chat_groups g ON ucg.chatgroup_id = g.id "
                  "WHERE u.email = :email AND g.name = :groupName");

    query.bindValue(":email", email);
    query.bindValue(":groupName", groupName);

    if (query.exec() && query.next()) {
        // If a record is found, the user is a member of the group
        // qDebug() << email << "is already a group member";
        return true;
    }

    // qDebug() << email << "is not a group member";

    return false;
}

bool ChatDatabaseHandler::removeUserFromGroup(const QString &email, const QString &groupName)
{
    if (!dbInitialized) {
        return false;
    }



    // First, get the user and group IDs
    int userId = -1;
    int groupId = -1;

    QSqlQuery userQuery(db);
    userQuery.prepare("SELECT id FROM users WHERE email = :email");
    userQuery.bindValue(":email", email);

    if (userQuery.exec() && userQuery.next()) {
        userId = userQuery.value(0).toInt();

    } else {
        return false; // User not found
    }

    QSqlQuery groupQuery(db);
    groupQuery.prepare("SELECT id FROM chat_groups WHERE name = :name");
    groupQuery.bindValue(":name", groupName);

    if (groupQuery.exec() && groupQuery.next()) {
        groupId = groupQuery.value(0).toInt();

    } else {
        return false; // Group not found
    }

    // Now remove the user from the group
    QSqlQuery removeQuery(db);
    removeQuery.prepare("DELETE FROM user_chat_groups WHERE user_id = :userId AND chatgroup_id = :groupId");
    removeQuery.bindValue(":userId", userId);
    removeQuery.bindValue(":groupId", groupId);

    return removeQuery.exec();
}

QList<std::tuple<QString, QString, int>> ChatDatabaseHandler::getGroupDetails(const QString &userEmail) const
{
    QList<std::tuple<QString, QString, int>> groups;

    if (!dbInitialized) {
        return groups;
    }

    QSqlQuery query(db);
    query.prepare(
        "SELECT g.id, g.name, COUNT(DISTINCT ug2.user_id) as member_count "
        "FROM chat_groups g "
        "JOIN user_chat_groups ug ON g.id = ug.chatgroup_id "
        "JOIN users u ON u.id = ug.user_id "
        "JOIN user_chat_groups ug2 ON g.id = ug2.chatgroup_id "
        "WHERE u.email = :userEmail "
        "GROUP BY g.id, g.name "
        "ORDER BY g.name"
    );
    query.bindValue(":userEmail", userEmail);

    if (query.exec()) {
        while (query.next()) {
            groups.append(std::make_tuple(
                query.value(0).toString(),  // group id
                query.value(1).toString(),  // group name
                query.value(2).toInt()      // member count
            ));
        }
    }

    return groups;
}

QList<std::tuple<QString, QString, int>> ChatDatabaseHandler::getCreatedGroups(const QString &userEmail) const
{
    QList<std::tuple<QString, QString, int>> groups;
    if (!dbInitialized) {
        return groups;
    }

    QSqlQuery query(db);
    query.prepare(
        "SELECT cg.id, cg.name, COUNT(DISTINCT ucg.user_id) as member_count "
        "FROM chat_groups cg "
        "LEFT JOIN user_chat_groups ucg ON cg.id = ucg.chatgroup_id "
        "WHERE cg.created_by = (SELECT id FROM users WHERE email = :email) "
        "GROUP BY cg.id, cg.name "
        "ORDER BY cg.name"
    );
    query.bindValue(":email", userEmail);

    if (query.exec()) {
        while (query.next()) {
            groups.append(std::make_tuple(
                query.value(0).toString(),
                query.value(1).toString(),
                query.value(2).toInt()
            ));
        }
        // qDebug() << "Created groups for" << userEmail << ":" << groups.size() << "groups found";
    } else {
        qDebug() << "Error in getCreatedGroups:" << query.lastError().text();
        qDebug() << "Query:" << query.lastQuery();
    }

    return groups;
}

QList<std::tuple<QString, QString, int>> ChatDatabaseHandler::getJoinedGroups(const QString &userEmail) const
{
    QList<std::tuple<QString, QString, int>> groups;
    if (!dbInitialized) {
        return groups;
    }

    // Get user ID first
    QSqlQuery userQuery(db);
    userQuery.prepare("SELECT id FROM users WHERE email = :email");
    userQuery.bindValue(":email", userEmail);
    
    if (!userQuery.exec() || !userQuery.next()) {
        qDebug() << "Error getting user ID for" << userEmail;
        return groups;
    }
    
    int userId = userQuery.value(0).toInt();

    QSqlQuery query(db);
    query.prepare(
        "SELECT cg.id, cg.name, COUNT(DISTINCT ucg2.user_id) as member_count "
        "FROM chat_groups cg "
        "INNER JOIN user_chat_groups ucg1 ON cg.id = ucg1.chatgroup_id AND ucg1.user_id = :user_id "
        "LEFT JOIN user_chat_groups ucg2 ON cg.id = ucg2.chatgroup_id "
        "WHERE cg.created_by != :user_id "
        "GROUP BY cg.id, cg.name "
        "ORDER BY cg.name"
    );
    query.bindValue(":user_id", userId);

    if (query.exec()) {
        while (query.next()) {
            groups.append(std::make_tuple(
                query.value(0).toString(),
                query.value(1).toString(),
                query.value(2).toInt()
            ));
        }
        qDebug() << "Joined groups for" << userEmail << ":" << groups.size() << "groups found";
    } else {
        qDebug() << "Error in getJoinedGroups:" << query.lastError().text();
        qDebug() << "Query:" << query.lastQuery();
    }

    return groups;
}

QPair<QString, QString> ChatDatabaseHandler::getGroupAdmin(const QString &groupId) {
    QString name;
    QString email;

    if (!dbInitialized) {
        return QPair<QString, QString>(name, email);
    }

    QSqlQuery query(db);
    query.prepare(
        "SELECT u.name, u.email FROM users u "
        "JOIN chat_groups cg ON u.id = cg.created_by "
        "WHERE cg.id = :groupId"
        );
    query.bindValue(":groupId", groupId);

    if (query.exec()) {
        if (query.next()) {  // Move to the first (and only) result row
            name = query.value(0).toString();
            email = query.value(1).toString();
        } else {
            qDebug() << "Admin not found for group:" << groupId;
        }
    } else {
        qDebug() << "SQL Error:" << query.lastError().text();
    }

    return QPair<QString, QString>(name, email);
}

bool ChatDatabaseHandler::updateGroupName(const QString &oldName, const QString &newName)
{
    if (!dbInitialized) return false;

    QSqlQuery query(db);
    query.prepare("UPDATE chat_groups SET name = :newName WHERE name = :oldName");
    query.bindValue(":newName", newName);
    query.bindValue(":oldName", oldName);

    if (!query.exec()) {
        qDebug() << "Failed to update group name:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool ChatDatabaseHandler::deleteGroup(const QString &groupId)
{
    QSqlDatabase::database().transaction();

    // Delete group messages
    QSqlQuery deleteMessages(QSqlDatabase::database());
    deleteMessages.prepare("DELETE FROM messages WHERE chatgroup_id = :groupId");
    deleteMessages.bindValue(":groupId", groupId);
    if (!deleteMessages.exec()) {
        QSqlDatabase::database().rollback();
        qDebug() << "Failed to delete group messages:" << deleteMessages.lastError();
        return false;
    }

    // Delete group memberships
    QSqlQuery deleteMembers(QSqlDatabase::database());
    deleteMembers.prepare("DELETE FROM user_chat_groups WHERE chatgroup_id = :groupId");
    deleteMembers.bindValue(":groupId", groupId);
    if (!deleteMembers.exec()) {
        QSqlDatabase::database().rollback();
        qDebug() << "Failed to delete group members:" << deleteMembers.lastError();
        return false;
    }

    // Delete the group itself
    QSqlQuery deleteGroup(QSqlDatabase::database());
    deleteGroup.prepare("DELETE FROM chat_groups WHERE id = :groupId");
    deleteGroup.bindValue(":groupId", groupId);
    if (!deleteGroup.exec()) {
        QSqlDatabase::database().rollback();
        qDebug() << "Failed to delete group:" << deleteGroup.lastError();
        return false;
    }

    return QSqlDatabase::database().commit();
}
