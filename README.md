[README.md](https://github.com/user-attachments/files/26412456/README.md)

# Door Opening Program

This project simulates the core logic of an IoT environment where security is the primary focus. It manages a distributed network of "devices" (doors, monitors) and users, utilizing HTTPS for secure data transit and MySQL for persistent state management.

Key Features

    Role-Based Access Control (RBAC): Implements specialized permissions (Teams) to restrict actions—e.g., specific teams can operate physical security (doors) while others are restricted to network monitoring.

    Secure Authentication Server: The current version (v0.1) features a dedicated central authentication server (using server.c) implemented with  HTTPS protocol using libssl to validate credentials and manage sessions.

Technical Stack

    Language: C (Low-level Socket Programming)

    Security: libssl (OpenSSL) for TLS/HTTPS implementation

    Database: MySQL for persistent user and session state

    Testing: Bash scripts for automated TLS handshakes

The program is still on development

## Program Status

In Development: Currently solving mysql connection to each connection to the API and solving variables to be put into database. Future versions will expand to include full device-to-server logic and expanded network tools.
## API Reference

#### Login

```
  POST /appdoor/login       (Authenticates existing users and initiates session.)
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `user` | `string` | **Required**. User username |
| `pass` | `string` | **Required**. User password |


#### Create account

```
  POST /appdoor/create      (Registers new users into the system.)
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `user` | `string` | **Required**. User username |
| `pass` | `string` | **Required**. User password |

#### Verify 2FA code

```
  POST /appdoor/verify-code (Validates 2FA codes for enhanced security.)
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `code` | `string` | **Required**. Code sent to 2FA |

Disclaimer : next versions (>v0.1) will have more parameters that must be passed to create the account such as team the user is inserted for example. Furthermore, the JSON reader is not complete yet so verify-code endpoint will have problems solving the parameters. 

There is a .sh (doors.sh) file that realizes the POSTs to the refered endpoints correctly using the openssl command to connect throw TLS (HTTPS)

To run the script:

```
  ./doors.sh login      
```
to access appdoor/login endpoint and
```
  ./doors.sh create     
```
to access appdoor/create endpoint
