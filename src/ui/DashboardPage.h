#pragma once

#include <QWidget>

#include "ui_DashboardPage.h"
#include "database/PasswordRepository.h"
#include "app/UserSession.h"
#include "EditPasswordDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DashboardPage; };
QT_END_NAMESPACE

class DashboardPage : public QWidget
{
	Q_OBJECT

public:
	DashboardPage(QWidget *parent = nullptr);
	~DashboardPage();

private:
	Ui::DashboardPage *m_ui;
	PasswordRepository m_passwordRepo;

private:
	void setupPasswordTable();
	void addPasswordRow(const PasswordEntry& item);
	void setupConnections() const;
	void renderPasswords(const QVector<PasswordEntry>& entries);

signals:
	void passwordDeleteSuccess();

private slots:
	void onEditPassword();
	void onDeletePassword();
	void onCopyUsername();
	void onCopyPassword();
	void onPasswordUpdate(const PasswordEntry& entry);
	void onSearchTextChanged(const QString& text);
	void onRefreshPasswordsClicked();
	
public slots:
	void loadPasswords();
};

