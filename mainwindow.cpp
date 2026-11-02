#include <QDebug>
#include <QByteArray>

#include "mainwindow.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#endif

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_loginPage(new LoginPage(this)),
    m_signupPage(new SignupPage(this)),
    m_mainAppView(new MainAppView(this))
{
    m_ui->setupUi(this);
    setWindowTitle("SecureVault");

    setupPages();
    setupConnections();
    applyTitleBarTheme();
}

void MainWindow::applyTitleBarTheme()
{
#ifdef Q_OS_WIN
    // Match the native title bar to the window background (#111827).
    // Caption/border/text colour attributes need Windows 11; ignored elsewhere.
    const HWND hwnd = reinterpret_cast<HWND>(winId());
    const BOOL dark = TRUE;
    const COLORREF caption = RGB(0x11, 0x18, 0x27);
    const COLORREF text = RGB(0xFF, 0xFF, 0xFF);
    DwmSetWindowAttribute(hwnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &dark, sizeof(dark));
    DwmSetWindowAttribute(hwnd, 35 /*DWMWA_CAPTION_COLOR*/, &caption, sizeof(caption));
    DwmSetWindowAttribute(hwnd, 34 /*DWMWA_BORDER_COLOR*/, &caption, sizeof(caption));
    DwmSetWindowAttribute(hwnd, 36 /*DWMWA_TEXT_COLOR*/, &text, sizeof(text));
#endif
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::setupPages()
{
    m_ui->stackedWidget->addWidget(m_loginPage);
    m_ui->stackedWidget->addWidget(m_signupPage);
    m_ui->stackedWidget->addWidget(m_mainAppView);

    m_ui->stackedWidget->setCurrentWidget(m_loginPage);
}

void MainWindow::setupConnections()
{
    connect(m_loginPage, &LoginPage::loginSuccessful,
        this, [this](const User& user) -> void {
            UserSession::instance().setCurrentUser(user);
            m_ui->stackedWidget->setCurrentWidget(m_mainAppView);
        });

    // load password only after user successfully logged in
    connect(m_loginPage, &LoginPage::loginSuccessful, m_mainAppView, &MainAppView::onUserLoggedIn);

   connect(m_loginPage, &LoginPage::signupRequested,
        this, [this]() {
            m_ui->stackedWidget->setCurrentWidget(m_signupPage);
        });

    connect(m_signupPage, &SignupPage::loginRequested,
        this, [this]() {
            m_ui->stackedWidget->setCurrentWidget(m_loginPage);
        });

    connect(m_mainAppView, &MainAppView::logoutRequested,
        this, [this]() {
            m_ui->stackedWidget->setCurrentWidget(m_loginPage);
        });
}
