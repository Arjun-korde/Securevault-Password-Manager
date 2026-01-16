#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGuiApplication>
#include <QClipboard>
#include <QTimer>
#include <QMessageBox>

#include "DashboardPage.h"

DashboardPage::DashboardPage(QWidget *parent)
	: QWidget(parent)
	, m_ui(new Ui::DashboardPage())
{
	m_ui->setupUi(this);
	setupPasswordTable();
    setupConnections();
    
}

DashboardPage::~DashboardPage()
{
	delete m_ui;
}

void DashboardPage::setupConnections() const
{
    connect(this, &DashboardPage::passwordDeleteSuccess, this, &DashboardPage::loadPasswords);
    connect(m_ui->refreshPasswordListButton, &QPushButton::clicked, this, &DashboardPage::onRefreshPasswordsClicked);
    connect(m_ui->searchLineEdit,&QLineEdit::textChanged, this, &DashboardPage::onSearchTextChanged
    );
}

void DashboardPage::setupPasswordTable()
{
    m_ui->passwordTable->setColumnCount(4);
    m_ui->passwordTable->setHorizontalHeaderLabels(
        { "ID", "Domain", "Username", "Actions" });

    // hide ID column
    m_ui->passwordTable->setColumnHidden(0, true);

    auto* header = m_ui->passwordTable->horizontalHeader();
    header->setSectionResizeMode(1, QHeaderView::Stretch); // Domain
    header->setSectionResizeMode(2, QHeaderView::Stretch); // Username
    header->setSectionResizeMode(3, QHeaderView::Fixed);   // Actions

    m_ui->passwordTable->setColumnWidth(3, 350);

    // align header to the left
    m_ui->passwordTable->horizontalHeader()->setDefaultAlignment(
        Qt::AlignLeft | Qt::AlignVCenter
    );
}

void DashboardPage::addPasswordRow(const PasswordEntry& item)
{
    // create new row
    const int row = m_ui->passwordTable->rowCount();
    m_ui->passwordTable->insertRow(row);

    // data rows
    m_ui->passwordTable->setItem(row, 0, new QTableWidgetItem(QString::number(item.id())));
    m_ui->passwordTable->setItem(row, 1, new QTableWidgetItem(item.domain()));
    m_ui->passwordTable->setItem(row, 2, new QTableWidgetItem(item.username()));

    // create action button layout
    QWidget* actionWidget = new QWidget(m_ui->passwordTable);
    QHBoxLayout* layout = new QHBoxLayout(actionWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    QPushButton* editBtn = new QPushButton("Edit", actionWidget);
    QPushButton* deleteBtn = new QPushButton("Delete", actionWidget);
    QPushButton* copyUserBtn = new QPushButton("Copy User", actionWidget);
    QPushButton* copyPassBtn = new QPushButton("Copy Pass", actionWidget);

    copyUserBtn->setObjectName("copyUserBtn");
    copyPassBtn->setObjectName("copyPassBtn");
    editBtn->setObjectName("editBtn");
    deleteBtn->setObjectName("deleteBtn");

    copyUserBtn->setFixedWidth(85);
    copyPassBtn->setFixedWidth(85);

    // add new property to the buttons
    for (auto btn : { editBtn, deleteBtn, copyUserBtn, copyPassBtn }) {
        btn->setProperty("passwordId", item.id());
        btn->setProperty("username", item.username());
        btn->setCursor(Qt::PointingHandCursor);
    }

    connect(editBtn, &QPushButton::clicked, this, &DashboardPage::onEditPassword);
    connect(deleteBtn, &QPushButton::clicked, this, &DashboardPage::onDeletePassword);
    connect(copyUserBtn, &QPushButton::clicked, this, &DashboardPage::onCopyUsername);
    connect(copyPassBtn, &QPushButton::clicked, this, &DashboardPage::onCopyPassword);

    // add buttons to the layout
    layout->addWidget(editBtn);
    layout->addWidget(deleteBtn);
    layout->addWidget(copyUserBtn);
    layout->addWidget(copyPassBtn);

    actionWidget->setLayout(layout);
    m_ui->passwordTable->setCellWidget(row, 3, actionWidget);
}

void DashboardPage::loadPasswords() {;

    int userId = UserSession::instance().currentUser().id();
    const auto passwordList = m_passwordRepo.fetchPasswords(userId);
    
    renderPasswords(passwordList);
}

void DashboardPage::onCopyUsername()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;

    QString username = btn->property("username").toString();
    
    btn->setText("copied");
    QTimer::singleShot(2000, this, [this, btn]() {
        btn->setText("Copy User");
    });

    QGuiApplication::clipboard()->setText(username);

}

void DashboardPage::onCopyPassword()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;

    int passwordId = btn->property("passwordId").toInt();

    auto entry = m_passwordRepo.fetchById(passwordId);

    btn->setText("copied");
    QTimer::singleShot(2000, this, [this, btn]() {
        btn->setText("Copy Pass");
        });

    if (!entry.has_value()) {
        qDebug() << "password with given id not found : " << passwordId;
        return;
    }
    QGuiApplication::clipboard()->setText(entry.value().password());

}

void DashboardPage::onDeletePassword()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;

    int passwordId = btn->property("passwordId").toInt();

    const auto reply = QMessageBox::question(
        this,
        "Delete Password",
        "Are you sure you want to delete this password?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        m_passwordRepo.deletePasswordById(passwordId);
        emit passwordDeleteSuccess();
    }
}

void DashboardPage::onEditPassword() {

    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn)
        return;
    int passwordId = btn->property("passwordId").toInt();

    auto entry = m_passwordRepo.fetchById(passwordId);

    if (!entry.has_value()) {
        qDebug() << "password with given id not found : " << passwordId;
        return;
    }

    EditPasswordDialog dialog(entry.value(), this);

    connect(&dialog, &EditPasswordDialog::passwordUpdate,
        this, &DashboardPage::onPasswordUpdate);

    dialog.exec();
}

void DashboardPage::onPasswordUpdate(const PasswordEntry& entry) {

    int userId = UserSession::instance().currentUser().id();

    bool isUpdated = m_passwordRepo.updatePassword(userId, entry);

    if (!isUpdated) {
        qDebug() << "password update failed";
        return;
    }

    loadPasswords();
}

void DashboardPage::onSearchTextChanged(const QString& text)
{
    const User& user = UserSession::instance().currentUser();
    const int userId = user.id();

    const auto passwordList = m_passwordRepo.searchPasswords(userId, text.trimmed());

    if (passwordList.isEmpty()) {
        qDebug() << "No results found!";

        // In future emit signal to display message instead table
    }

    renderPasswords(passwordList);
}

void DashboardPage::onRefreshPasswordsClicked() 
{
    const QString searchText = m_ui->searchLineEdit->text().trimmed();
    onSearchTextChanged(searchText);
}

void DashboardPage::renderPasswords(const QVector<PasswordEntry>& entries) 
{
    m_ui->passwordTable->setRowCount(0);

    for (const auto& entry : entries) {
        addPasswordRow(entry);
    }
}