#!/bin/bash

gcc -g -o main_server \
    server/main.c server/connection_handler.c server/match.c server/logger.c \

gcc -g -o main_client \
    client/main.c