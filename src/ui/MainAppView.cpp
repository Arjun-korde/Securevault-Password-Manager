#include "MainAppView.h"


MainAppView::MainAppView(QWidget *parent)
	: QWidget(parent),
	m_ui(new Ui::MainAppView()),
	m_dashboardPage(new DashboardPage(this)),
	m_addPasswordPage(new AddPasswordPage(this)),
	m_settingsPage(new SettingsPage(this))
{
	m_ui->setupUi(this);

	setupPages();
	setupConnections();

}

MainAppView::~MainAppView()
{
	delete m_ui;
}

void MainAppView::setupPages()
{
	m_ui->contentStack->addWidget(m_dashboardPage);
	m_ui->contentStack->addWidget(m_addPasswordPage);
	m_ui->contentStack->addWidget(m_settingsPage);
}

void MainAppView::setupConnections()
{
	connect(m_ui->dashboardButton, &QPushButton::clicked, this, &MainAppView::showDashboard);
	connect(m_ui->addPasswordButton, &QPushButton::clicked, this, &MainAppView::showAddPassword);
	connect(m_ui->settingsButton, &QPushButton::clicked, this, &MainAppView::showSettings);
	connect(m_ui->logoutButton, &QPushButton::clicked, this, &MainAppView::onLogoutClicked);

	connect(m_settingsPage, &SettingsPage::logoutRequested, this, &MainAppView::onLogoutClicked);
	connect(m_settingsPage, &SettingsPage::deleteAccount, this, &MainAppView::onDeleteAccount);
	connect(m_addPasswordPage, &AddPasswordPage::passwordSaveSuccess, this, &MainAppView::onPasswordSave);
	connect(m_dashboardPage, &DashboardPage::addPasswordClicked, this, &MainAppView::showAddPassword);

}

void MainAppView::showDashboard()
{
	m_ui->contentStack->setCurrentWidget(m_dashboardPage);
}

void MainAppView::showAddPassword()
{
	m_ui->contentStack->setCurrentWidget(m_addPasswordPage);
}

void MainAppView::showSettings()
{
	m_ui->contentStack->setCurrentWidget(m_settingsPage);
}

void MainAppView::onLogoutClicked()
{
	UserSession::instance().reset();
	emit logoutRequested();
}

void MainAppView::onUserLoggedIn() 
{
	m_ui->contentStack->setCurrentWidget(m_dashboardPage);
	m_dashboardPage->loadPasswords();
}

void MainAppView::onPasswordSave() 
{
	m_dashboardPage->loadPasswords();
	showDashboard();
}

void MainAppView::onDeleteAccount() 
{
	UserSession::instance().reset();
	showDashboard();
	emit logoutRequested();
}