#pragma once

#include <QWidget>

#include "ui_DashboardPage.h"
#include "database/PasswordRepository.h"
#include "app/UserSession.h"
#include "EditPasswordDialog.h"

//enum ViewState;
enum ViewState {
	Passwords,
	NoResults,
	Empty
};

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
	QTimer* m_searchDebounceTimer;

private:
	void setupPasswordTable();
	void addPasswordRow(const PasswordEntry& item);
	void setupConnections() const;
	void renderPasswords(const QVector<PasswordEntry>& entries);
	void setViewState(ViewState state, const QString& searchText = QString());

signals:
	void passwordDeleteSuccess();
	void addPasswordClicked() const;

private slots:
	void onEditPassword();
	void onDeletePassword();
	void onCopyUsername();
	void onCopyPassword();
	void onPasswordUpdate(const PasswordEntry& entry);
	void onSearchTextChanged();
	void onRefreshPasswordsClicked();
	void performSearch();
	
public slots:
	void loadPasswords();

public:
	void clearSearchText();
};

