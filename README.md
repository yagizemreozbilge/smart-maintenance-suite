# Smart Maintenance Suite

## Overview

**Smart Maintenance Suite**  is an IoT-based industrial maintenance management platform that digitalizes and optimizes factory machinery operations. It enables real-time equipment monitoring, predictive maintenance planning, and downtime reduction to improve production efficiency.

The system is built with a React frontend, a high-performance C backend, and PostgreSQL database integration. It follows modern software architecture principles, utilizes efficient data structures for real-time processing, maintains 90%+ backend and 96%+ frontend test coverage, and is containerized with Docker and deployed on Google Cloud for scalability and reliability.
### Key Features
-  **Machine Monitoring**: Real-time sensor data processing and health tracking
- **Analytics Dashboard**: Interactive charts and visualizations
-  **Role-Based Access Control**: Admin, Technician, and Operator roles
-  **Inventory Management**: Stock tracking with low-stock alerts
-  **Maintenance Logging**: Comprehensive maintenance history
-  **Report Generation**: XML/CSV/PDF export capabilities
-  **Comprehensive Testing**: 100+ unit tests with >80% coverage target

## Requirements

- CMake >= 4.2.3
- GCC 15.2.0 (MinGW x86_64)
- Docker 29.1.2
- Node.js / npx 11.8.0
- WSL 2 (Windows Subsystem for Linux)
- React 18.2.0

## 📂 Project Structure

```bash
smart-maintenance-suite/
│
├── .github/                     # CI/CD workflows
├── assets/                      # Coverage badges, static assets
├── diagrams/                    # UML & architecture diagrams
├── docs/                        # Documentation & coverage reports
│
├── backend/                     # Core C backend application
│   ├── api/                     # HTTP handlers & routing
│   ├── core/                    # Core logic & constants
│   ├── data_structures/         # BST, Graph, Heap, Queue, Stack
│   ├── database/                # DB connection & migrations
│   ├── models/                  # Domain models
│   ├── modules/                 # Fault, Inventory, Machine, Maintenance
│   ├── report/                  # PDF & report services
│   ├── security/                # JWT, RBAC, Encryption
│   ├── integration/             # Azure IoT & Blob integration
│   ├── utils/                   # Logger, memory, config
│   └── main.c                   # Application entry point
│
├── frontend/                    # React + Vite application
│   ├── src/
│   │   ├── components/          # UI components
│   │   ├── context/             # React context
│   │   └── App.js
│   └── tests/                   # Frontend unit tests (Vitest)
│
├── tests/                       # Backend unit & integration tests
│   ├── unit/
│   ├── integration/
│   └── runners/
│
├── scripts/                     # Build, coverage & automation scripts
│
├── docker-compose.yml
├── Dockerfile.backend
├── Dockerfile.frontend
├── CMakeLists.txt
├── Doxyfile*
└── README.md

## 📂 Scripts & Automation

```bash
smart-maintenance-suite/
│
├── scripts/
│   ├── build.sh
│   ├── deploy.sh
│   ├── docker_build.sh
│   ├── run_tests.sh
│   ├── coverage_report.sh
│   ├── memory_check.sh
│
├── 0-init-submodules.bat
├── 0-update-submodules.bat
├── 1-configure-git-hooks.bat
├── 2-create-git-ignore.bat
├── 3-install-package-manager.bat
├── 4-install-windows-enviroment.bat
├── 4-install-wsl-environment.sh
├── 5-format-code.bat
├── 6_download_plantuml.bat
├── 7-build-app-linux.sh
├── 7-build-app-windows.bat
├── 7-build-doc-windows.bat
├── 8-build-test-windows.bat
├── 9-clean-configure-app-windows.bat
├── 9-clean-project.bat
├── 10-generate_backend_coverage.bat
├── 10-generate_backend_coverage.sh
├── 11-run-frontend-coverage.bat
├── 11-run-frontend-coverage.sh
├── delete_desktop_ini.bat
├── delete_desktop_ini.sh
├── pre-commit
└── pre-push



## Setup Development Environment

### Step-1 (Run on Windows, Can Effect on WSL)

Run 1-configure-pre-commit.bat file to copy 1-pre-commit script to .git/hooks that checkes. README.md, gitignore and doxygenfiles. Also format code with astyle tool

### Step-2 (Run on Windows, Can Effect on WSL)

If gitignore missing then you can create gitignore with 2-create-git-ignore.bat file run this file.

### Step-3 (Only Windows)

Install package managers that we will use to install applications. Run 3-install-package-manager.bat to install choco and scoop package managers

### Step-4 (Only Windows)

Run 4-install-windows-enviroment.bat to install required applications. 

### Step-5 (Only WSL)

Open powershell as admin and enter WSL then goto project folder and run 4-install-wsl-environment.sh to setup WSL environment



## Generate Development Environment

You can run 9-clean-configure-app-windows.bat to generate Visual Studio Communit Edition Project of this file. Or You can use Cmake project development with Visual Studio Community Edition



## Build, Test and Package Application on Windows

Run 7-build-app-windows.bat to build, test and generate packed binaries for your application on windows.



Also you can run 7-build-doc-windows.bat to only generate documentation and 8-build-test-windows.bat to only test application. 

## Build, Test and Package Application on WSL

Run 7-build-app-linux.sh to build, test and generate packed binaries for your application on WSL environment.



## Clean Project

You can run 9-clean-project.bat to clean project outputs. 

## Generate Coverage in Backend and Frontend
Run 10-generate_backend_coverage.bat
Run 11-run-frontend-coverage.bat

## Generate Coverage in Backend and Frontend in WSL
sed -i 's/\r$//' 10-generate_backend_coverage.sh
chmod +x 10-generate_backend_coverage.sh
./10-generate_backend_coverage.sh
sed -i 's/\r$//' 11-run-frontend-coverage.sh
chmod +x 11-run-frontend-coverage.sh
./11-run-frontend-coverage.sh
## Supported Platforms

![Ubuntu badge](assets/badge-ubuntu.svg)

![macOS badge](assets/badge-macos.svg)

![Windows badge](assets/badge-windows.svg)

### Cross-Platform Test Coverage

> **Note** : There is a known bug on doxygen following badges are in different folder but has same name for this reason in doxygen html report use same image for all content [Images with same name overwrite each other in output directory · Issue #8362 · doxygen/doxygen · GitHub](https://github.com/doxygen/doxygen/issues/8362). README.md and WebPage show correct badges.

| Application Layer | Metric | Windows OS | Linux OS (WSL-Ubuntu 20.04) |
|-------------------|--------|------------|-----------------------------|
| Backend (C) | Line | [![90%](assets/codecoveragelibwin/badge_linecoverage.svg)](docs/coverage_backend_report/index.html) | [![90%](assets/codecoverageliblinux/badge_linecoverage.svg)](docs/coverage_backend_report_linux/index.html) |
| Backend (C) | Branch | [![88%](assets/codecoveragelibwin/badge_branchcoverage.svg)](docs/coverage_backend_report/index.html) | [![87%](assets/codecoverageliblinux/badge_branchcoverage.svg)](docs/coverage_backend_report_linux/index.html) |
| Frontend (React) | Line | [![96%](assets/codecoveragelibwin/frontend_badge_linecoverage.svg)](docs/coverage_frontend_report/index.html) | [![96%](assets/codecoverageliblinux/frontend_badge_linecoverage.svg)](docs/coverage_frontend_report_linux/index.html) |
| Frontend (React) | Branch | [![94%](assets/codecoveragelibwin/frontend_badge_branchcoverage.svg)](docs/coverage_frontend_report/index.html) | [![94%](assets/codecoverageliblinux/frontend_badge_branchcoverage.svg)](docs/coverage_frontend_report_linux/index.html) |


### Documentation Coverage Ratios

|                    | Windows OS                                                        | Linux OS (WSL-Ubuntu 20.04)                                         |
| ------------------ | ----------------------------------------------------------------- | ------------------------------------------------------------------- |
| **Coverage Ratio** | ![Line Coverage](assets/doccoveragelibwin/badge_linecoverage.svg) | ![Line Coverage](assets/doccoverageliblinux/badge_linecoverage.svg) |

## 📡 API Testing

All backend endpoints were manually tested using Postman and automatically validated through unit and integration tests.

### Example Endpoints

| Method | Endpoint | Description |
|--------|----------|------------|
| POST   | /api/login | Authenticate user |
| GET    | /api/machines | Retrieve machine list |
| GET    | /api/inventory | Retrieve inventory status |
| GET    | /api/alerts | Retrieve active alerts |
| POST   | /api/maintenance | Create maintenance record |




## UML Diagrams
### System Architecture
![System Architecture](diagrams/system_architecture.png)

### Backend Class Diagram
![Backend Class Diagram](diagrams/detailed_class_diagram.png)

### Database Schema
![Database Schema](diagrams/database_schema.png)

### Maintenance Scheduling Sequence
![Maintenance Scheduling Sequence](diagrams/maintenance_scheduling_sequence.png)

## 🚀 Quick Start

```bash
docker compose up --build
Frontend:
http://localhost:5173

Backend:
http://localhost:8080

```md
## 🔑 Demo Credentials

Default development users:

| Role        | Username | Password  |
|-------------|----------|----------|
| Admin       | admin    | admin123 |
| Technician  | tech     | tech123  |
| Operator    | operator | op123    |



> Development only.

```bash
docker compose down

$End-Of-File$
