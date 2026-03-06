#!/bin/bash

SERVER_URL="http://localhost:3490/appdoor"

case "$1" in
    login)
        echo "=============="
        echo " LOGIN SYSTEM "
        echo "=============="

        read -p "User: " username
        read -sp "Password: " password
        echo ""

        response=$(curl -s -X POST "$SERVER_URL/login" \
        -H "Content-Type: application/json" \
        -d "{\"user\":\"$username\", \"pass\":\"$password\"}")

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

        response=$(curl -s -X POST "$SERVER_URL/create" \
        -H "Content-Type: application/json" \
        -d "{\"user\":\"$username\", \"pass\":\"$password\"}")

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