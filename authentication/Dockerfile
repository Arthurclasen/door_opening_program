# 1. build
FROM ubuntu:24.04 AS builder

# download essentials and data-base dependencies
RUN apt update && apt install -y build-essential -y libmysqlclient-dev

WORKDIR /app

RUN mkdir authentication

COPY authentication/Makefile ./
COPY authentication/authentication.c ./authentication/
COPY common ./common/

RUN make

# 2. runtime
FROM ubuntu:24.04

# production environment with port 3490
ENV APP_PORT=3490 \
    APP_ENV=production \
    MODEL_THREADS=1

# install in runtime
RUN apt update && apt install -y libmysqlclient21

# create a non-root user
RUN groupadd -r appuser && useradd -r -g appuser appuser

WORKDIR /app

# copy the binary made in the builder
COPY --from=builder /app/authentication_out /app/
COPY --from=builder /app/common/    /app/common/

RUN chmod +777 authentication_out

# user mode
USER appuser

# port to be used
EXPOSE 3490

# run the application
ENTRYPOINT [ "./authentication_out" ]   

