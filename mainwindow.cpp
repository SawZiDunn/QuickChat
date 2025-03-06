#include "mainwindow.h"
#include <QInputDialog>
#include <QStyleFactory>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Apply dark theme
    applyDarkTheme();

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
    setupPrivateChatPage();

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

void MainWindow::applyDarkTheme()
{
    // Set style to Fusion which works well with custom palettes
    qApp->setStyle(QStyleFactory::create("Fusion"));

    // Create dark palette
    QPalette darkPalette;

    // Dark background colors
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 45));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));

    // Text colors
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);

    // Selection colors
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    // Tooltip colors
    darkPalette.setColor(QPalette::ToolTipBase, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);

    // Link color
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    // Apply the palette
    qApp->setPalette(darkPalette);

    // Set stylesheet for additional customization
    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"
                        "QLineEdit, QTextEdit { background-color: #303030; border: 1px solid #5c5c5c; border-radius: 3px; padding: 2px; }"
                        "QPushButton { background-color: #5c5c5c; border-radius: 5px; border: none; color: white; padding: 8px; margin: 2px; max-width: 250px; }"
                        "QPushButton:hover { background-color: #6e6e6e; }"
                        "QPushButton:pressed { background-color: #2a82da; }"
                        "QLabel { color: #ffffff; }"
                        "QMessageBox { background-color: #353535; }");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupWelcomePage()
{
    welcomePage = new QWidget();

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(welcomePage);

    // Add logo/image (can be replaced with actual image)
    QLabel *logoLabel = new QLabel();
    logoLabel->setText("💬"); // Chat emoji as placeholder
    QFont logoFont = logoLabel->font();
    logoFont.setPointSize(60);
    logoLabel->setFont(logoFont);
    logoLabel->setAlignment(Qt::AlignCenter);

    // Add title
    QLabel *titleLabel = new QLabel("Welcome to Quick Chat");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Add subtitle
    QLabel *subtitleLabel = new QLabel("Connect with friends securely");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(12);
    subtitleLabel->setFont(subtitleFont);

    // Create container for buttons
    QWidget *buttonContainer = new QWidget();
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonContainer);

    // Add buttons
    loginButton = new QPushButton("Login");
    registerButton = new QPushButton("Register");

    // Style buttons
    loginButton->setMinimumHeight(40);
    registerButton->setMinimumHeight(40);

    // Give the buttons a fixed width
    loginButton->setFixedWidth(200);
    registerButton->setFixedWidth(200);

    // Connect buttons
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::showLoginForm);
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::showRegisterForm);

    // Add buttons to button layout
    buttonLayout->addWidget(loginButton, 0, Qt::AlignCenter);
    buttonLayout->addWidget(registerButton, 0, Qt::AlignCenter);
    buttonLayout->setSpacing(10);

    // Add widgets to main layout
    layout->addStretch();
    layout->addWidget(logoLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addSpacing(40);
    layout->addWidget(buttonContainer);
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
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Create form container with max width
    QWidget *formContainer = new QWidget();
    formContainer->setFixedWidth(320);
    QVBoxLayout *containerLayout = new QVBoxLayout(formContainer);

    // Create vertical form layout instead of QFormLayout
    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->setSpacing(10);

    // Email label and field
    QLabel *emailLabel = new QLabel("Email:");
    emailLabel->setAlignment(Qt::AlignLeft);
    loginEmailField = new QLineEdit();
    loginEmailField->setMinimumHeight(30);
    loginEmailField->setPlaceholderText("Enter your email");
    loginEmailField->setStyleSheet("QLineEdit { color: white; } QLineEdit::placeholder { color: white; }");

    // Password label and field
    QLabel *passwordLabel = new QLabel("Password:");
    passwordLabel->setAlignment(Qt::AlignLeft);
    loginPasswordField = new QLineEdit();
    loginPasswordField->setMinimumHeight(30);
    loginPasswordField->setEchoMode(QLineEdit::Password);
    loginPasswordField->setPlaceholderText("Enter your password");
    loginPasswordField->setStyleSheet("QLineEdit { color: white; } QLineEdit::placeholder { color: white; }");

    // Add widgets to form layout
    formLayout->addWidget(emailLabel);
    formLayout->addWidget(loginEmailField);
    formLayout->addWidget(passwordLabel);
    formLayout->addWidget(loginPasswordField);

    // Create button container
    QWidget *buttonContainer = new QWidget();
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonContainer);

    // Create buttons
    loginSubmitButton = new QPushButton("Login");
    loginBackButton = new QPushButton("Back");

    // Style buttons
    loginSubmitButton->setMinimumHeight(40);
    loginBackButton->setMinimumHeight(40);
    loginSubmitButton->setFixedWidth(200);
    loginBackButton->setFixedWidth(200);

    // Connect buttons
    connect(loginSubmitButton, &QPushButton::clicked, this, &MainWindow::performLogin);
    connect(loginBackButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentWidget(welcomePage);
    });

    // Add buttons to button layout
    buttonLayout->addWidget(loginSubmitButton, 0, Qt::AlignCenter);
    buttonLayout->addWidget(loginBackButton, 0, Qt::AlignCenter);
    buttonLayout->setSpacing(10);

    // Add layouts to container
    containerLayout->addLayout(formLayout);
    containerLayout->addSpacing(20);

    // Add widgets to main layout
    layout->addSpacing(30);
    layout->addWidget(titleLabel);
    layout->addSpacing(40);
    layout->addWidget(formContainer, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(buttonContainer);
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
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Create form container with max width
    QWidget *formContainer = new QWidget();
    formContainer->setFixedWidth(320);
    QVBoxLayout *containerLayout = new QVBoxLayout(formContainer);

    // Create vertical form layout instead of QFormLayout
    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->setSpacing(10);

    // Username label and field
    QLabel *usernameLabel = new QLabel("Username:");
    usernameLabel->setAlignment(Qt::AlignLeft);
    registerUsernameField = new QLineEdit();
    registerUsernameField->setMinimumHeight(30);
    registerUsernameField->setPlaceholderText("Choose a username");
    registerUsernameField->setStyleSheet("QLineEdit { color: white; } QLineEdit::placeholder { color: white; }");

    // Email label and field
    QLabel *emailLabel = new QLabel("Email:");
    emailLabel->setAlignment(Qt::AlignLeft);
    registerEmailField = new QLineEdit();
    registerEmailField->setMinimumHeight(30);
    registerEmailField->setPlaceholderText("Enter your email");
    registerEmailField->setStyleSheet("QLineEdit { color: white; } QLineEdit::placeholder { color: white; }");

    // Password label and field
    QLabel *passwordLabel = new QLabel("Password:");
    passwordLabel->setAlignment(Qt::AlignLeft);
    registerPasswordField = new QLineEdit();
    registerPasswordField->setMinimumHeight(30);
    registerPasswordField->setEchoMode(QLineEdit::Password);
    registerPasswordField->setPlaceholderText("Choose a password");
    registerPasswordField->setStyleSheet("QLineEdit { color: white; } QLineEdit::placeholder { color: white; }");

    // Add widgets to form layout
    formLayout->addWidget(usernameLabel);
    formLayout->addWidget(registerUsernameField);
    formLayout->addWidget(emailLabel);
    formLayout->addWidget(registerEmailField);
    formLayout->addWidget(passwordLabel);
    formLayout->addWidget(registerPasswordField);

    // Create button container
    QWidget *buttonContainer = new QWidget();
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonContainer);

    // Create buttons
    registerSubmitButton = new QPushButton("Register");
    registerBackButton = new QPushButton("Back");

    // Style buttons
    registerSubmitButton->setMinimumHeight(40);
    registerBackButton->setMinimumHeight(40);
    registerSubmitButton->setFixedWidth(200);
    registerBackButton->setFixedWidth(200);

    // Connect buttons
    connect(registerSubmitButton, &QPushButton::clicked, this, &MainWindow::performRegistration);
    connect(registerBackButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentWidget(welcomePage);
    });

    // Add buttons to button layout
    buttonLayout->addWidget(registerSubmitButton, 0, Qt::AlignCenter);
    buttonLayout->addWidget(registerBackButton, 0, Qt::AlignCenter);
    buttonLayout->setSpacing(10);

    // Add layouts to container
    containerLayout->addLayout(formLayout);
    containerLayout->addSpacing(20);

    // Add widgets to main layout
    layout->addSpacing(30);
    layout->addWidget(titleLabel);
    layout->addSpacing(40);
    layout->addWidget(formContainer, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(buttonContainer);
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
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Welcome message
    QLabel *welcomeLabel = new QLabel();
    welcomeLabel->setAlignment(Qt::AlignCenter);
    QFont welcomeFont = welcomeLabel->font();
    welcomeFont.setPointSize(12);
    welcomeLabel->setFont(welcomeFont);
    welcomeLabel->setText("Welcome to Qt Chat Application");

    // Create card widget containers
    QWidget *cardContainer = new QWidget();
    QGridLayout *cardLayout = new QGridLayout(cardContainer);
    cardLayout->setSpacing(15);

    // Define button styles
    QString cardButtonStyle = "QPushButton {"
                              "  background-color: #3a3a3a;"
                              "  border-radius: 8px;"
                              "  padding: 15px;"
                              "  text-align: center;"
                              "  color: white;"
                              "}"
                              "QPushButton:hover {"
                              "  background-color: #4a4a4a;"
                              "}"
                              "QPushButton:pressed {"
                              "  background-color: #2a82da;"
                              "}";

    // Create buttons
    startPrivateChatButton = new QPushButton("Start Private Chat");
    viewGroupChatsButton = new QPushButton("View Group Chats");
    createGroupChatButton = new QPushButton("Create Group Chat");
    joinGroupChatButton = new QPushButton("Join Group Chat");

    // Style card buttons
    startPrivateChatButton->setStyleSheet(cardButtonStyle);
    viewGroupChatsButton->setStyleSheet(cardButtonStyle);
    createGroupChatButton->setStyleSheet(cardButtonStyle);
    joinGroupChatButton->setStyleSheet(cardButtonStyle);

    startPrivateChatButton->setFixedSize(200, 120);
    viewGroupChatsButton->setFixedSize(200, 120);
    createGroupChatButton->setFixedSize(200, 120);
    joinGroupChatButton->setFixedSize(200, 120);

    // Create logout button
    logoutButton = new QPushButton("Logout");
    logoutButton->setFixedWidth(120);

    // Connect buttons
    connect(startPrivateChatButton, &QPushButton::clicked, this, &MainWindow::startPrivateChat);
    connect(viewGroupChatsButton, &QPushButton::clicked, this, &MainWindow::viewGroupChats);
    connect(createGroupChatButton, &QPushButton::clicked, this, &MainWindow::createGroupChat);
    connect(joinGroupChatButton, &QPushButton::clicked, this, &MainWindow::joinGroupChat);
    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        currentUser = "";
        stackedWidget->setCurrentWidget(welcomePage);
    });

    // Add buttons to card layout
    cardLayout->addWidget(startPrivateChatButton, 0, 0);
    cardLayout->addWidget(viewGroupChatsButton, 0, 1);
    cardLayout->addWidget(createGroupChatButton, 1, 0);
    cardLayout->addWidget(joinGroupChatButton, 1, 1);

    // Add widgets to layout
    layout->addSpacing(20);
    layout->addWidget(titleLabel);
    layout->addWidget(welcomeLabel);
    layout->addSpacing(30);
    layout->addWidget(cardContainer, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(logoutButton, 0, Qt::AlignCenter);
    layout->addStretch();

    // Add page to stacked widget
    stackedWidget->addWidget(mainMenuPage);
}

void MainWindow::setupPrivateChatPage()
{
    privateChatPage = new QWidget();

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(privateChatPage);

    // Create header
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);

    QLabel *chatPartnerLabel = new QLabel("Chat with: ");
    chatPartnerLabel->setFont(QFont("Arial", 12, QFont::Bold));

    QLabel *partnerNameLabel = new QLabel();
    partnerNameLabel->setFont(QFont("Arial", 12));

    leaveChatButton = new QPushButton("Leave Chat");
    leaveChatButton->setFixedWidth(100);

    headerLayout->addWidget(chatPartnerLabel);
    headerLayout->addWidget(partnerNameLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(leaveChatButton);

    // Create chat display
    chatHistoryDisplay = new QTextEdit();
    chatHistoryDisplay->setReadOnly(true);
    chatHistoryDisplay->setStyleSheet("background-color: #232323; border: 1px solid #3a3a3a; border-radius: 5px;");

    // Create message input area
    QWidget *messageWidget = new QWidget();
    QHBoxLayout *messageLayout = new QHBoxLayout(messageWidget);

    messageInputField = new QLineEdit();
    messageInputField->setPlaceholderText("Type your message here...");
    messageInputField->setStyleSheet("padding: 8px;");

    sendMessageButton = new QPushButton("Send");
    sendMessageButton->setFixedWidth(80);

    messageLayout->addWidget(messageInputField);
    messageLayout->addWidget(sendMessageButton);

    // Add widgets to main layout
    layout->addWidget(headerWidget);
    layout->addWidget(chatHistoryDisplay);
    layout->addWidget(messageWidget);

    // Connect signals
    connect(leaveChatButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentWidget(mainMenuPage);
    });

    connect(sendMessageButton, &QPushButton::clicked, this, [this]() {
        if (!messageInputField->text().isEmpty()) {
            // Add sent message to chat history
            chatHistoryDisplay->append("<span style='color:#42a5f5;'><b>You:</b></span> " + messageInputField->text());

            // Clear input field
            messageInputField->clear();

            // Simulate reply (for demo purposes)
            QTimer::singleShot(1000, this, [this]() {
                chatHistoryDisplay->append("<span style='color:#66bb6a;'><b>" + currentChatPartner +
                                           ":</b></span> This is a simulated response.");
            });
        }
    });

    // Add page to stacked widget
    stackedWidget->addWidget(privateChatPage);
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
    // For demonstration, we'll simulate choosing a user
    QStringList userList;
    for (auto it = users.begin(); it != users.end(); ++it) {
        if (it.value().first != currentUser) {
            userList.append(it.value().first);
        }
    }

    if (userList.isEmpty()) {
        QMessageBox::information(this, "No Users",
                                 "There are no other users available for chat.");
        return;
    }

    bool ok;
    QString selectedUser = QInputDialog::getItem(this, "Start Private Chat",
                                                 "Select a user to chat with:",
                                                 userList, 0, false, &ok);

    if (ok && !selectedUser.isEmpty()) {
        // Set current chat partner
        currentChatPartner = selectedUser;

        // Clear chat history
        chatHistoryDisplay->clear();

        // Add welcome message
        chatHistoryDisplay->append("<center><span style='color:#9e9e9e;'>--- Starting chat with " +
                                   selectedUser + " ---</span></center>");

        // Switch to private chat page
        stackedWidget->setCurrentWidget(privateChatPage);
    }
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
