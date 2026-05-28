#!/bin/bash
set -e

echo "Changing default client gateway route..."
ip route del default || true
ip route add default via 192.168.10.2 || true

echo "Waiting for server port $TARGET_PORT to be open..."
sleep 7

echo "🚀 Executing client command: $*"
exec "$@"