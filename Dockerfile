FROM gcc:latest
COPY ./server /app/server
COPY ./utils /app/utils
WORKDIR /app
RUN gcc -g -o main_server server/main.c server/connection_handler.c server/match.c server/logger.c
CMD ["stdbuf", "-i0", "-o0", "-e0", "./main_server"]