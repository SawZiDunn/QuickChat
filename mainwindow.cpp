#include "mainwindow.h"
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Set up the main window
    setWindowTitle("Qt Chat Application");
    resize(800, 600);

    // Create stacked widget to manage different pages
    stackedWidget = new QStackedWidget();
    setCentralWidget(stackedWidget);

    // Set up each page
    setupWelcomePage();
    setupLoginPage();
    setupRegisterPage();
    setupMainMenuPage();

    // Start with welcome page
    stackedWidget->setCurrentWidget(welcomePage);

    // Initialize chat request timer
    requestCheckTimer = new QTimer(this);
    connect(requestCheckTimer, &QTimer::timeout, this, &MainWindow::checkPendingRequests);
    requestCheckTimer->start(5000); // Check every 5 seconds

    // Add some mock users for testing
    users.insert("user1@gmail.com", qMakePair(QString("User1"), QString("123")));
    users.insert("user2@gmail.com", qMakePair(QString("User2"), QString("123")));

    // Add some mock group chats
    groupChats << "General Discussion" << "Qt Developers" << "Beginners Help";
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupWelcomePage()
{
    welcomePage = new QWidget();

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(welcomePage);

    // Add title
    QLabel *titleLabel = new QLabel("Welcome to Qt Chat");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Add buttons
    loginButton = new QPushButton("Login");
    registerButton = new QPushButton("Register");

    // Style buttons
    loginButton->setMinimumHeight(40);
    registerButton->setMinimumHeight(40);

    // Connect buttons
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::showLoginForm);
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::showRegisterForm);

    // Add widgets to layout
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addSpacing(30);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
    layout->addStretch();

    // Add page to stacked widget
    stackedWidget->addWidget(welcomePage);
}

void MainWindow::setupLoginPage()
{
    loginPage = new QWidget();

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(loginPage);

    // Add title
    QLabel *titleLabel = new QLabel("Login to Your Account");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Create form layout
    QFormLayout *formLayout = new QFormLayout();
    loginEmailField = new QLineEdit();
    loginPasswordField = new QLineEdit();
    loginPasswordField->setEchoMode(QLineEdit::Password);

    formLayout->addRow("Email:", loginEmailField);
    formLayout->addRow("Password:", loginPasswordField);

    // Create buttons
    loginSubmitButton = new QPushButton("Login");
    loginBackButton = new QPushButton("Back");

    // Connect buttons
    connect(loginSubmitButton, &QPushButton::clicked, this, &MainWindow::performLogin);
    connect(loginBackButton, &QPushButton::clicked, this, &MainWindow::showLoginForm);
    connect(loginBackButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentWidget(welcomePage);
    });

    // Add widgets to layout
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addLayout(formLayout);
    layout->addSpacing(20);
    layout->addWidget(loginSubmitButton);
    layout->addWidget(loginBackButton);
    layout->addStretch();

    // Add page to stacked widget
    stackedWidget->addWidget(loginPage);
}

void MainWindow::setupRegisterPage()
{
    registerPage = new QWidget();

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(registerPage);

    // Add title
    QLabel *titleLabel = new QLabel("Create a New Account");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Create form layout
    QFormLayout *formLayout = new QFormLayout();
    registerUsernameField = new QLineEdit();
    registerEmailField = new QLineEdit();
    registerPasswordField = new QLineEdit();
    registerPasswordField->setEchoMode(QLineEdit::Password);

    formLayout->addRow("Username:", registerUsernameField);
    formLayout->addRow("Email:", registerEmailField);
    formLayout->addRow("Password:", registerPasswordField);

    // Create buttons
    registerSubmitButton = new QPushButton("Register");
    registerBackButton = new QPushButton("Back");

    // Connect buttons
    connect(registerSubmitButton, &QPushButton::clicked, this, &MainWindow::performRegistration);
    connect(registerBackButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentWidget(welcomePage);
    });

    // Add widgets to layout
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addLayout(formLayout);
    layout->addSpacing(20);
    layout->addWidget(registerSubmitButton);
    layout->addWidget(registerBackButton);
    layout->addStretch();

    // Add page to stacked widget
    stackedWidget->addWidget(registerPage);
}

void MainWindow::setupMainMenuPage()
{
    mainMenuPage = new QWidget();

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(mainMenuPage);

    // Add title
    QLabel *titleLabel = new QLabel("Main Menu");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Welcome message
    QLabel *welcomeLabel = new QLabel();
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setText("Welcome to Qt Chat Application");

    // Create buttons
    startPrivateChatButton = new QPushButton("Start Private Chat");
    viewGroupChatsButton = new QPushButton("View Group Chats");
    createGroupChatButton = new QPushButton("Create Group Chat");
    joinGroupChatButton = new QPushButton("Join Group Chat");
    logoutButton = new QPushButton("Logout");

    // Connect buttons
    connect(startPrivateChatButton, &QPushButton::clicked, this, &MainWindow::startPrivateChat);
    connect(viewGroupChatsButton, &QPushButton::clicked, this, &MainWindow::viewGroupChats);
    connect(createGroupChatButton, &QPushButton::clicked, this, &MainWindow::createGroupChat);
    connect(joinGroupChatButton, &QPushButton::clicked, this, &MainWindow::joinGroupChat);
    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        currentUser = "";
        stackedWidget->setCurrentWidget(welcomePage);
    });

    // Add widgets to layout
    layout->addWidget(titleLabel);
    layout->addWidget(welcomeLabel);
    layout->addSpacing(30);
    layout->addWidget(startPrivateChatButton);
    layout->addWidget(viewGroupChatsButton);
    layout->addWidget(createGroupChatButton);
    layout->addWidget(joinGroupChatButton);
    layout->addSpacing(20);
    layout->addWidget(logoutButton);
    layout->addStretch();

    // Add page to stacked widget
    stackedWidget->addWidget(mainMenuPage);
}

void MainWindow::showLoginForm()
{
    loginEmailField->clear();
    loginPasswordField->clear();
    stackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::showRegisterForm()
{
    registerUsernameField->clear();
    registerEmailField->clear();
    registerPasswordField->clear();
    stackedWidget->setCurrentWidget(registerPage);
}

void MainWindow::showMainMenu()
{
    stackedWidget->setCurrentWidget(mainMenuPage);
}

void MainWindow::performLogin()
{
    QString email = loginEmailField->text();
    QString password = loginPasswordField->text();

    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Error", "Please enter both email and password.");
        return;
    }

    if (users.contains(email) && users[email].second == password) {
        currentUser = users[email].first;
        QMessageBox::information(this, "Login Successful",
                                 "Welcome back, " + currentUser + "!");
        showMainMenu();
    } else {
        QMessageBox::warning(this, "Login Error",
                             "Invalid email or password. Please try again.");
    }
}

void MainWindow::performRegistration()
{
    QString username = registerUsernameField->text();
    QString email = registerEmailField->text();
    QString password = registerPasswordField->text();

    if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Registration Error",
                             "All fields are required.");
        return;
    }

    if (users.contains(email)) {
        QMessageBox::warning(this, "Registration Error",
                             "Email already registered. Please use a different email.");
        return;
    }

    // Add new user
    users.insert(email, qMakePair(username, password));

    QMessageBox::information(this, "Registration Successful",
                             "Account created successfully! You can now login.");

    // Switch to login page
    stackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::startPrivateChat()
{
    QMessageBox::information(this, "Feature Coming Soon",
                             "Private chat functionality will be implemented in a future update.");
}

void MainWindow::viewGroupChats()
{
    if (groupChats.isEmpty()) {
        QMessageBox::information(this, "No Group Chats",
                                 "There are no group chats available at the moment.");
        return;
    }

    QString chatList = "Available Group Chats:\n\n";
    for (const QString &chat : groupChats) {
        chatList += "- " + chat + "\n";
    }

    QMessageBox::information(this, "Group Chats", chatList);
}

void MainWindow::createGroupChat()
{
    bool ok;
    QString chatName = QInputDialog::getText(this, "Create Group Chat",
                                             "Enter a name for the new group chat:",
                                             QLineEdit::Normal, "", &ok);

    if (ok && !chatName.isEmpty()) {
        if (groupChats.contains(chatName)) {
            QMessageBox::warning(this, "Create Group Chat",
                                 "A group chat with this name already exists.");
        } else {
            groupChats.append(chatName);
            QMessageBox::information(this, "Group Chat Created",
                                     "The group chat '" + chatName + "' has been created successfully.");
        }
    }
}

void MainWindow::joinGroupChat()
{
    if (groupChats.isEmpty()) {
        QMessageBox::information(this, "No Group Chats",
                                 "There are no group chats available to join.");
        return;
    }

    bool ok;
    QString selectedChat = QInputDialog::getItem(this, "Join Group Chat",
                                                 "Select a group chat to join:",
                                                 groupChats, 0, false, &ok);

    if (ok && !selectedChat.isEmpty()) {
        QMessageBox::information(this, "Group Chat Joined",
                                 "You have joined the group chat '" + selectedChat + "'.");
    }
}

void MainWindow::checkPendingRequests()
{
    // This method will be implemented when we add private chat functionality
}

void MainWindow::sendChatRequest(const QString &toUser)
{
    // This method will be implemented when we add private chat functionality
}

void MainWindow::acceptChatRequest(const ChatRequest &request)
{
    // This method will be implemented when we add private chat functionality
}

void MainWindow::rejectChatRequest(const ChatRequest &request)
{
    // This method will be implemented when we add private chat functionality
}

void MainWindow::openPrivateChat(const QString &otherUser)
{
    // This method will be implemented when we add private chat functionality
}
