#!/bin/bash

docker compose up --build
docker cp MarioStrikersCMD_server:/tmp/log_match.txt log_match.txt
