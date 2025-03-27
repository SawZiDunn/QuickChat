#ifndef SETUP_DB_H
#define SETUP_DB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDebug>
#include <QDateTime>

void executeSQL(QSqlDatabase &db, const QString &sql);
bool checkDatabaseExists(const QString &dbName);

int setup_chat_db()
{
    QString database_name = "chat_database.db";

    // Check if the database already exists
    if (checkDatabaseExists(database_name))
    {
        qDebug() << "Chat database already exists, skipping creation and data population.";
        return 0; // No need to create the database again
    }

    // Create and open the database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(database_name);

    if (!db.open())
    {
        qDebug() << "Can't open database:" << db.lastError().text();
        return 1;
    }
    else
    {
        qDebug() << "Opened database successfully:" << database_name;
    }

    // Create Users table
    QString createUsersTable =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE, "
        "email TEXT NOT NULL UNIQUE, "
        "password TEXT NOT NULL"
        ");";
    executeSQL(db, createUsersTable);

    // Create ChatGroups table
    QString createChatGroupsTable =
        "CREATE TABLE IF NOT EXISTS chat_groups ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE, "
        "created_at DATETIME NOT NULL, "
        "created_by INTEGER, "
        "FOREIGN KEY (created_by) REFERENCES users (id)"
        ");";
    executeSQL(db, createChatGroupsTable);

    // Create UserChatGroups table (junction table for many-to-many relationship)
    QString createUserChatGroupsTable =
        "CREATE TABLE IF NOT EXISTS user_chat_groups ("
        "user_id INTEGER NOT NULL, "
        "chatgroup_id INTEGER NOT NULL, "
        "PRIMARY KEY (user_id, chatgroup_id), "
        "FOREIGN KEY (user_id) REFERENCES users (id), "
        "FOREIGN KEY (chatgroup_id) REFERENCES chat_groups (id)"
        ");";
    executeSQL(db, createUserChatGroupsTable);

    // Create Messages table
    QString createMessagesTable =
        "CREATE TABLE IF NOT EXISTS messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "sender_id INTEGER NOT NULL, "
        "chatgroup_id INTEGER, "
        "recipient_id INTEGER, "
        "content TEXT NOT NULL, "
        "timestamp DATETIME NOT NULL, "
        "FOREIGN KEY (sender_id) REFERENCES users (id), "
        "FOREIGN KEY (chatgroup_id) REFERENCES chat_groups (id), "
        "FOREIGN KEY (recipient_id) REFERENCES users (id), "
        "CHECK ((chatgroup_id IS NULL AND recipient_id IS NOT NULL) OR "
        "(chatgroup_id IS NOT NULL AND recipient_id IS NULL))"
        ");";
    executeSQL(db, createMessagesTable);

    // Populate Users table with demo data
    QStringList demoUsers = {
        "INSERT INTO users (name, email, password) VALUES ('Alice', 'alice@example.com', '123');",
        "INSERT INTO users (name, email, password) VALUES ('Bob', 'bob@gmail.com', '123');",
        "INSERT INTO users (name, email, password) VALUES ('Charlie', 'charlie@gmail.com', '123');",
        "INSERT INTO users (name, email, password) VALUES ('Diana', 'diana@gmail.com', '123');",
        "INSERT INTO users (name, email, password) VALUES ('Evan', 'evan@gmail.com' , '123');"
    };

    for (const QString &sql : demoUsers) {
        executeSQL(db, sql);
    }

    // Current timestamp for chat group creation
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QStringList demoChatGroups = {
        // 'General' group created by Alice (user_id 1)
        QString("INSERT INTO chat_groups (name, created_at, created_by) VALUES ('General', '%1', 1);").arg(currentTime),

        // 'Tech Talk' group created by Alice (user_id 1)
        QString("INSERT INTO chat_groups (name, created_at, created_by) VALUES ('Tech Talk', '%1', 1);").arg(currentTime),

        // 'Coffee Break' group created by Bob (user_id 2)
        QString("INSERT INTO chat_groups (name, created_at, created_by) VALUES ('Coffee Break', '%1', 2);").arg(currentTime)
    };

    for (const QString &sql : demoChatGroups) {
        executeSQL(db, sql);
    }

    // Populate UserChatGroups table (who belongs to which groups)
    QStringList demoUserChatGroups = {
        // Everyone in General
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (1, 1);",
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (2, 1);",
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (3, 1);",
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (4, 1);",
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (5, 1);",
        // Tech Talk members
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (1, 2);",
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (3, 2);",
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (5, 2);",
        // Coffee Break members
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (2, 3);",
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (3, 3);",
        "INSERT INTO user_chat_groups (user_id, chatgroup_id) VALUES (4, 3);"
    };

    for (const QString &sql : demoUserChatGroups) {
        executeSQL(db, sql);
    }

    // Create some demo messages with realistic timestamps
    QDateTime baseTime = QDateTime::currentDateTime().addDays(-1);

    // Group messages
    QStringList demoGroupMessages = {
        // General group messages
        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (1, 1, NULL, 'Hello everyone!', '%1');")
            .arg(baseTime.toString("yyyy-MM-dd hh:mm:ss")),

        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (2, 1, NULL, 'Hi Alice, how are you?', '%1');")
            .arg(baseTime.addSecs(60).toString("yyyy-MM-dd hh:mm:ss")),

        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (3, 1, NULL, 'Welcome to the general chat!', '%1');")
            .arg(baseTime.addSecs(120).toString("yyyy-MM-dd hh:mm:ss")),

        // Tech Talk group messages
        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (1, 2, NULL, 'Anyone using the new Qt framework?', '%1');")
            .arg(baseTime.addSecs(180).toString("yyyy-MM-dd hh:mm:ss")),

        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (3, 2, NULL, 'Yes, I''m working on a project with it right now!', '%1');")
            .arg(baseTime.addSecs(240).toString("yyyy-MM-dd hh:mm:ss")),

        // Coffee Break group messages
        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (2, 3, NULL, 'Anyone want to grab coffee later?', '%1');")
            .arg(baseTime.addSecs(300).toString("yyyy-MM-dd hh:mm:ss")),

        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (4, 3, NULL, 'I''m in! Around 3pm?', '%1');")
            .arg(baseTime.addSecs(360).toString("yyyy-MM-dd hh:mm:ss"))
    };

    for (const QString &sql : demoGroupMessages) {
        executeSQL(db, sql);
    }

    // Direct messages between users
    QStringList demoDirectMessages = {
        // Alice to Bob
        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (1, NULL, 2, 'Hey Bob, do you have the meeting notes?', '%1');")
            .arg(baseTime.addSecs(420).toString("yyyy-MM-dd hh:mm:ss")),

        // Bob to Alice
        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (2, NULL, 1, 'Yes, I''ll send them over shortly!', '%1');")
            .arg(baseTime.addSecs(480).toString("yyyy-MM-dd hh:mm:ss")),

        // Charlie to Diana
        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (3, NULL, 4, 'Diana, are you joining the Tech Talk group?', '%1');")
            .arg(baseTime.addSecs(540).toString("yyyy-MM-dd hh:mm:ss")),

        // Diana to Charlie
        QString("INSERT INTO messages (sender_id, chatgroup_id, recipient_id, content, timestamp) "
                "VALUES (4, NULL, 3, 'Not yet, but I''m thinking about it!', '%1');")
            .arg(baseTime.addSecs(600).toString("yyyy-MM-dd hh:mm:ss"))
    };

    for (const QString &sql : demoDirectMessages) {
        executeSQL(db, sql);
    }

    // Close the database
    db.close();
    qDebug() << "Database setup completed successfully.";

    return 0;
}

void executeSQL(QSqlDatabase &db, const QString &sql)
{
    QSqlQuery query(db);
    if (!query.exec(sql))
    {
        qDebug() << "SQL error:" << query.lastError().text() << "\nQuery: " << sql;
    }
    else
    {
        qDebug() << "SQL executed successfully.";
    }
}

bool checkDatabaseExists(const QString &dbName)
{
    // Check if the database file exists
    return QFile::exists(dbName);
}

#endif // SETUP_DB_H
