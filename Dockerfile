# Dockerfile for C Backend
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    gcc \
    cmake \
    libpq-dev \
    libssl-dev \
    libcjson-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY src src

RUN gcc -o smart_maintenance_backend \
    src/backend/cloud_main.c \
    src/backend/api/http_server.c \
    src/backend/api/router.c \
    src/backend/api/handlers/auth_handler.c \
    src/backend/api/handlers/inventory_handler.c \
    src/backend/api/handlers/maintenance_handler.c \
    src/backend/api/handlers/report_handler.c \
    src/backend/api/handlers/fault_handler.c \
    src/backend/api/handlers/machine_handler.c \
    src/backend/database/db_connection.c \
    src/backend/database/inventory_service.c \
    src/backend/database/machine_service.c \
    src/backend/database/maintenance_service.c \
    src/backend/database/report_service.c \
    src/backend/database/sensor_service.c \
    src/backend/database/alert_service.c \
    src/backend/database/api_handlers.c \
    src/backend/database/cJSON.c \
    src/backend/security/jwt.c \
    src/backend/security/rbac.c \
    src/backend/security/encryption.c \
    src/backend/core/data_structures/queue.c \
    src/backend/core/data_structures/stack.c \
    src/backend/core/data_structures/bst.c \
    src/backend/core/data_structures/heap.c \
    src/backend/core/data_structures/graph.c \
    -Isrc/backend \
    -Isrc/backend/api \
    -Isrc/backend/api/handlers \
    -Isrc/backend/database \
    -Isrc/backend/security \
    -Isrc/backend/core/data_structures \
    -I/usr/include/postgresql \
    -lpq -lssl -lcrypto -lpthread -lm

EXPOSE 8080

CMD ["./smart_maintenance_backend"]