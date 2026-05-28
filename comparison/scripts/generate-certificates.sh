#!/bin/bash
set -e

# Define base directory for certificates
BASE_DIR="."

# Create clean, isolated directory structures
mkdir -p "$BASE_DIR/ca"
mkdir -p "$BASE_DIR/server"
mkdir -p "$BASE_DIR/client"

echo "=== GENERATING PKI STRUCTURE FOR LIBCOAP (DTLS) ==="

# 1. CERTIFICATE AUTHORITY (CA)
echo "-> Generating Root Certificate Authority..."
openssl ecparam -name prime256v1 -genkey -noout -out "$BASE_DIR/ca/ca.key"
openssl req -new -x509 -days 365 -key "$BASE_DIR/ca/ca.key" -out "$BASE_DIR/ca/ca.crt" \
    -subj "/CN=IIoT-Root-CA/O=University/ST=Thesis"

# 2. IIoT SERVER
echo "-> Generating IIoT Server Certificate..."
openssl ecparam -name prime256v1 -genkey -noout -out "$BASE_DIR/server/server.key"
openssl req -new -key "$BASE_DIR/server/server.key" -out "$BASE_DIR/server/server.csr" \
    -subj "/CN=10.0.20.3/O=University/ST=Thesis"
# Root CA signs the Server CSR
openssl x509 -req -days 365 -in "$BASE_DIR/server/server.csr" \
    -CA "$BASE_DIR/ca/ca.crt" -CAkey "$BASE_DIR/ca/ca.key" \
    -CAcreateserial -out "$BASE_DIR/server/server.crt"
# Clean up temporary Certificate Signing Request
rm "$BASE_DIR/server/server.csr"

# 3. IIoT CLIENT
echo "-> Generating IIoT Client Certificate..."
openssl ecparam -name prime256v1 -genkey -noout -out "$BASE_DIR/client/client.key"
openssl req -new -key "$BASE_DIR/client/client.key" -out "$BASE_DIR/client/client.csr" \
    -subj "/CN=10.0.10.3/O=University/ST=Thesis"
# Root CA signs the Client CSR
openssl x509 -req -days 365 -in "$BASE_DIR/client/client.csr" \
    -CA "$BASE_DIR/ca/ca.crt" -CAkey "$BASE_DIR/ca/ca.key" \
    -CAcreateserial -out "$BASE_DIR/client/client.crt"
# Clean up temporary Certificate Signing Request
rm "$BASE_DIR/client/client.csr"

# Distribute the public Root CA certificate to both endpoints
# They strictly require this file to mutally authenticate each other
cp "$BASE_DIR/ca/ca.crt" "$BASE_DIR/server/"
cp "$BASE_DIR/ca/ca.crt" "$BASE_DIR/client/"
