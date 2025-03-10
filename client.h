// #ifndef CLIENT_H
// #define CLIENT_H

// #include <QObject>
// #include <QTcpSocket>
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QTimer>

// class Client : public QObject
// {
//     Q_OBJECT

// public:
//     explicit Client(QObject *parent = nullptr);
//     ~Client();

//     bool connectToServer(const QString &address, quint16 port);
//     void disconnectFromServer();
//     bool isConnected() const;

//     // Chat actions
//     void sendPrivateMessage(const QString &recipient, const QString &message);
//     void sendGroupMessage(const QString &groupName, const QString &message);
//     void sendChatRequest(const QString &recipient);
//     void respondToChatRequest(const QString &sender, bool accepted);
//     void joinGroupChat(const QString &groupName);
//     void leaveGroupChat(const QString &groupName);
//     void createGroupChat(const QString &groupName);

// signals:
//     void connected();
//     void disconnected();
//     void error(const QString &errorMessage);

//     void privateMessageReceived(const QString &sender, const QString &message);
//     void groupMessageReceived(const QString &groupName, const QString &sender, const QString &message);
//     void chatRequestReceived(const QString &sender);
//     void chatRequestResponse(const QString &recipient, bool accepted);
//     void userJoinedGroup(const QString &groupName, const QString &username);
//     void userLeftGroup(const QString &groupName, const QString &username);
//     void onlineUsersUpdated(const QStringList &users);
//     void availableGroupsUpdated(const QStringList &groups);

// private slots:
//     void onReadyRead();
//     void onDisconnected();
//     void onError(QAbstractSocket::SocketError socketError);
//     void checkConnection();

// private:
//     QTcpSocket *socket;
//     QTimer *heartbeatTimer;

//     void sendJson(const QJsonObject &jsonObject);
//     void processIncomingData(const QByteArray &data);
// };

// #endif // CLIENT_H
