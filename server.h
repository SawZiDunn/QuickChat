/ #ifndef SERVER_H
// #define SERVER_H

// #include <QObject>
// #include <QTcpServer>
// #include <QTcpSocket>
// #include <QMap>
// #include <QSet>
// #include <QJsonDocument>
// #include <QJsonObject>

// struct ClientInfo {
//     QString username;
//     QString email;
//     QSet<QString> joinedGroups;
// };

// class Server : public QObject
// {
//     Q_OBJECT

// public:
//     explicit Server(QObject *parent = nullptr);
//     ~Server();

//     bool start(quint16 port = 8080);
//     void stop();
//     bool isRunning() const;

//     // Server management
//     void broadcastOnlineUsers();
//     void broadcastAvailableGroups();

//     // Direct methods (for local server use)
//     void addUser(const QString &username, const QString &email);
//     bool createGroupChat(const QString &groupName);
//     void removeGroupChat(const QString &groupName);

// signals:
//     void started();
//     void stopped();
//     void clientConnected(const QString &username);
//     void clientDisconnected(const QString &username);
//     void privateMessageSent(const QString &from, const QString &to, const QString &message);
//     void groupMessageSent(const QString &groupName, const QString &from, const QString &message);
//     void error(const QString &errorMessage);

// private slots:
//     void onNewConnection();
//     void onReadyRead();
//     void onClientDisconnected();

// private:
//     QTcpServer *server;
//     QMap<QTcpSocket*, ClientInfo> clients;
//     QMap<QString, QSet<QTcpSocket*>> groupChats;

//     void processClientMessage(QTcpSocket *senderSocket, const QJsonObject &json);
//     void sendToClient(QTcpSocket *client, const QJsonObject &jsonObject);
//     void broadcastToGroup(const QString &groupName, const QJsonObject &jsonObject, QTcpSocket *except = nullptr);

//     // Helper methods
//     bool authenticateUser(const QString &email, const QString &password);
//     QTcpSocket* findClientSocket(const QString &username);
// };

// #endif // SERVER_H
