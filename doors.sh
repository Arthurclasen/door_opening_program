#!/bin/bash

case "$1" in
    login)
        echo "=============="
        echo " LOGIN SYSTEM "
        echo "=============="

        read -p "User: " username
        read -sp "Password: " password
        echo ""

        BODY="{\"user\":\"$username\",\"pass\":\"$password\"}"
        LEN=${#BODY}

        response=$(printf "POST /appdoor/login HTTP/1.1\r\nHost: 172.18.0.3\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s" \
        "$LEN" "$BODY" \
        | openssl s_client -connect 172.18.0.3:3490 -showcerts 2>/dev/null)

        echo "==========================="
        echo " SERVER RESPONSE: $response"
        echo "==========================="
        ;;
    
    create)
        echo "================"
        echo " CREATE ACCOUNT "
        echo "================"

        read -p "New User: " username
        read -sp "Password: " password
        echo ""

        response=$(printf "POST /appdoor/create HTTP/1.1\r\nHost: 172.18.0.3\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s" \
        "$LEN" "$BODY" \
        | openssl s_client -connect 172.18.0.3:3490 -showcerts 2>/dev/null)

        echo "==========================="
        echo " SERVER RESPONSE: $response"
        echo "==========================="
        ;;
    *)
        echo "Use: "
        echo " doorapp login"
        echo " doorapp create"
        ;;
esac