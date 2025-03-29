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
#include "chatdbhandler.h"
#include "menuwidget.h"


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
    MenuWidget *menuWidget;

    // Helper methods
    void setupWelcomePage();
    void setupLoginPage();
    void setupRegisterPage();
    void setupMainMenuPage();

    // Add the database handler
    ChatDatabaseHandler dbHandler;
    QPair<QString, QString> currentUser;
};
#endif // MAINWINDOW_H
