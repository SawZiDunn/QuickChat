#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QMap>
#include <QTimer>
#include <QTextEdit>
#include <QGridLayout>
#include <QApplication>

// Structure to store chat request information
struct ChatRequest {
    QString fromUser;
    QString toUser;
    bool pending;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showLoginForm();
    void showRegisterForm();
    void showMainMenu();
    void performLogin();
    void performRegistration();
    void createGroupChat();
    void joinGroupChat();
    void startPrivateChat();
    void viewGroupChats();

private:
    // Apply dark theme
    void applyDarkTheme();

    // Widgets for different pages
    QStackedWidget *stackedWidget;

    // Welcome page widgets
    QWidget *welcomePage;
    QPushButton *loginButton;
    QPushButton *registerButton;

    // Login page widgets
    QWidget *loginPage;
    QLineEdit *loginEmailField;
    QLineEdit *loginPasswordField;
    QPushButton *loginSubmitButton;
    QPushButton *loginBackButton;

    // Register page widgets
    QWidget *registerPage;
    QLineEdit *registerUsernameField;
    QLineEdit *registerEmailField;
    QLineEdit *registerPasswordField;
    QPushButton *registerSubmitButton;
    QPushButton *registerBackButton;

    // Main menu page widgets
    QWidget *mainMenuPage;
    QPushButton *createGroupChatButton;
    QPushButton *joinGroupChatButton;
    QPushButton *viewGroupChatsButton;
    QPushButton *startPrivateChatButton;
    QPushButton *logoutButton;

    // Chat request management
    QList<ChatRequest> chatRequests;
    QTimer *requestCheckTimer;
    void setupPrivateChatPage();
    void checkPendingRequests();
    void sendChatRequest(const QString &toUser);
    void acceptChatRequest(const ChatRequest &request);
    void rejectChatRequest(const ChatRequest &request);
    void openPrivateChat(const QString &otherUser);

    // Private chat UI elements
    QWidget *privateChatPage;
    QTextEdit *chatHistoryDisplay;
    QLineEdit *messageInputField;
    QPushButton *sendMessageButton;
    QPushButton *leaveChatButton;
    QString currentChatPartner;

    // Mock database
    QMap<QString, QPair<QString, QString>> users; // email -> (username, password)
    QStringList groupChats;

    // Currently logged in user
    QString currentUser;

    // Helper methods
    void setupWelcomePage();
    void setupLoginPage();
    void setupRegisterPage();
    void setupMainMenuPage();
};
#endif // MAINWINDOW_H
