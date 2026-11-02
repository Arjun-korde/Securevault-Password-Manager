# SecureVault Password Manager

A native Windows desktop password manager written in C++17 with Qt 6 Widgets. Credentials are encrypted with AES-256 before they are stored in a PostgreSQL database.

> **Status:** learning/portfolio project under active development. It has not been security-audited. See [Known Limitations](#known-limitations) before storing real passwords in it.

## Table of Contents

- [Features](#features)
- [Tech Stack](#tech-stack)
- [Architecture](#architecture)
- [Security Design](#security-design)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Known Limitations](#known-limitations)
- [Roadmap](#roadmap)

## Features

- User sign-up and login with a master password
- Add, edit and delete saved credentials (domain, username, password)
- Debounced live search across domain and username
- One-click copy of a username or password to the clipboard
- Change master password and delete account from the Settings page
- Per-user data isolation (all queries are scoped by `user_id`)
- Automatic creation of the application database and tables on first run

## Tech Stack

| Area        | Technology                                   |
|-------------|----------------------------------------------|
| Language    | C++17                                        |
| UI          | Qt 6 Widgets (Qt Designer `.ui` files)       |
| Database    | PostgreSQL via Qt SQL (`QPSQL` driver)       |
| Encryption  | AES-256-CBC through Windows CNG (`bcrypt`)   |
| Build       | CMake (3.16+), Ninja, MSVC 2022              |
| Platform    | Windows 10/11 x64                            |

## Architecture

The code is split into layers under `src/`:

```
UI (src/ui)  ->  Repositories (src/database)  ->  PostgreSQL
                      |
                      +--> Security (src/security): hashing, key derivation, AES
                      +--> Session  (src/app):      logged-in user + encryption key
```

- **UI** - pages and dialogs (`LoginPage`, `SignupPage`, `DashboardPage`, `AddPasswordPage`, `SettingsPage`, `EditPasswordDialog`, `ChangeMasterPasswordDialog`), hosted by `MainAppView`.
- **Repositories** - `UserRepository` and `PasswordRepository` contain all SQL. Queries are parameterised.
- **Security** - `PasswordHasher` (login verification), `KeyDerivation` (encryption key from the master password) and `CryptoManager` (AES encrypt/decrypt).
- **Session** - `UserSession` is a singleton holding the current user and the in-memory encryption key. The key is wiped on `reset()`.
- **Database** - `DatabaseManager` connects to PostgreSQL and creates the schema.

### Database schema

- `users` - `id`, `username` (unique, stored lower-cased), `password_hash`, `password_salt`, `encryption_salt`, `created_at`
- `passwords` - `id`, `user_id` (FK, `ON DELETE CASCADE`), `domain`, `username`, `encrypted_password`, `iv`, `require_master_reprompt`, `created_at`, `updated_at`

## Security Design

1. **Authentication:** the master password is salted (random 16 bytes) and hashed with iterated SHA-256. Only the hash and salt are stored.
2. **Encryption key:** a separate random salt is combined with the master password and hashed to a 256-bit key. The key lives only in memory for the duration of the session.
3. **Encryption:** each saved password is encrypted with AES-256 in CBC mode using a fresh random 16-byte IV, stored next to the ciphertext.
4. **Isolation:** the master password and encryption key are never written to the database.

## Getting Started

### Prerequisites

- Windows 10/11 (64-bit)
- Visual Studio 2022 with the *Desktop development with C++* workload
- CMake 3.16 or newer and Ninja (both bundled with Visual Studio)
- Qt 6 for MSVC 2022 64-bit, including the **Qt SQL** module (presets reference 6.8.3 and 6.12.0)
- PostgreSQL 13+ running on `localhost:5432`
- The Qt `QPSQL` driver must be able to find `libpq.dll`; add the PostgreSQL `bin` folder to `PATH`

### Configure the database connection

Connection settings are currently constants at the top of `src/database/DatabaseManager.cpp`:

```cpp
static QString APP_DB_NAME = "aesdb";
static QString DB_HOST = "localhost";
static QString DB_USER = "postgres";
static QString DB_PASS = "<your postgres password>";
static int     DB_PORT = 5432;
```

Set these to match your local PostgreSQL instance. The `aesdb` database and its tables are created automatically on first launch, so the configured user needs permission to create databases.

> Do not commit real credentials. Moving these into environment variables or a config file is on the roadmap.

### Build

The Qt install path is set in `CMakeUserPresets.json` (`QTDIR`). Adjust it if Qt is installed elsewhere.

**Visual Studio:** open the project folder, choose the Qt preset, then *Build > Build All* and run `PasswordManager.exe`.

**Command line** (from a *x64 Native Tools Command Prompt for VS 2022*):

```bat
cmake --list-presets
cmake --preset <preset-name>
cmake --build --preset <preset-name>
```

**Qt Creator:** open `CMakeLists.txt` and select a Qt 6 MSVC 2022 64-bit kit.

## Project Structure

```
.
|-- main.cpp, mainwindow.*      Application entry point and main window
|-- CMakeLists.txt, qt.cmake    Build configuration
|-- CMakePresets.json           Shared build presets
|-- CMakeUserPresets.json       Local Qt kit presets
|-- resources/                  Icons and Resource.qrc
`-- src/
    |-- app/                    UserSession
    |-- database/               DatabaseManager, UserRepository, PasswordRepository
    |-- models/                 User, PasswordEntry
    |-- security/               CryptoManager, KeyDerivation, PasswordHasher, SecurityConfig
    |-- ui/                     Pages, dialogs and .ui forms
    `-- utils/                  Input validation helpers
```

## Known Limitations

Please read these before relying on the app:

- Password hashing and key derivation use a plain iterated SHA-256 loop, not a memory-hard KDF such as PBKDF2, Argon2 or scrypt.
- AES-CBC provides confidentiality but no integrity check; an authenticated mode (AES-GCM) would be stronger.
- `CryptoManager` does not yet check the status codes returned by Windows CNG calls.
- Database credentials are hard-coded in source.
- Changing the master password must re-encrypt all stored entries with the new key; verify this behaviour before using it on data you care about.
- The code uses Windows CNG, so it only builds on Windows.

## Roadmap

- [ ] Replace the SHA-256 loops with PBKDF2 or Argon2
- [ ] Move to AES-256-GCM
- [ ] Error handling for all crypto calls
- [ ] Load database settings from environment variables or a config file
- [ ] Re-encrypt vault when the master password changes
- [ ] Auto-lock on inactivity and clipboard auto-clear
- [ ] Unit tests for the security and repository layers
- [ ] Password generator

## License

No license has been specified yet. Add a `LICENSE` file to define how others may use this code.
