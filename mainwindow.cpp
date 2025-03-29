#include "mainwindow.h"
#include "menuwidget.h"
#include "privatechatwidget.h"
#include "groupchatwidget.h"

#include <QInputDialog>
#include <QStyleFactory>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), dbHandler(this)
{
    // Apply dark theme
    applyDarkTheme();

    // Set up the main window
    setWindowTitle("QuickChat");
    resize(800, 600);

    // Create stacked widget to manage different pages
    stackedWidget = new QStackedWidget();
    setCentralWidget(stackedWidget);

    // Set up each page
    setupWelcomePage();
    setupLoginPage();
    setupRegisterPage();

    menuWidget = new MenuWidget(stackedWidget);

    stackedWidget->addWidget(menuWidget);


    // Start with welcome page
    stackedWidget->setCurrentWidget(welcomePage);


    // Initialize the database
    if (!dbHandler.initialize()) {
        QMessageBox::critical(this, "Database Error", "Failed to initialize the database connection.");
    }
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

void MainWindow::performLogin()
{
    QString email = loginEmailField->text();
    QString password = loginPasswordField->text();

    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Error", "Please enter both email and password.");
        return;
    }

    QString currentUserName = dbHandler.loginUser(email, password);

    if (!currentUserName.isEmpty()) {
        currentUser = qMakePair(currentUserName, email); // Store the current user pair
        QMessageBox::information(this, "Login Successful",
                                 "Welcome back, " + currentUser.first + "!");
        showMainMenu();
    } else {
        QMessageBox::warning(this, "Login Error",
                             "Invalid email or password. Please try again.");
    }
}

void MainWindow::showMainMenu() {
    menuWidget->setCurrentUser(currentUser.first, currentUser.second);
    stackedWidget->setCurrentWidget(menuWidget);
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

    // Register user in the database
    if (dbHandler.registerUser(username, email, password)) {
        // You might want to save the email as well - see suggestion below
        QMessageBox::information(this, "Registration Successful",
                                 "Account created successfully! You can now login.");
        // Switch to login page
        stackedWidget->setCurrentWidget(loginPage);
    } else {
        QMessageBox::warning(this, "Registration Error",
                             "Username already exists or database error occurred.");
    }
}
