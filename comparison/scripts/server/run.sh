#!/bin/bash
set -e

echo "Changing default server gateway route..."
ip route del default || true
ip route add default via 192.168.20.2 || true

TRACE_NAME="$1"
shift

echo "📸 Starting packet capture: /traces/${TRACE_NAME}.pcap"
# Using administrative write context directly
tcpdump -Z root -U -i any -w "/traces/${TRACE_NAME}.pcap" "udp port 5683 or udp port 5684" &
TCP_DUMP_PID=$!

sleep 2

# Cleanup function to gracefully stop tcpdump and flush keys on container stop
cleanup() {
    echo "🐳 Stopping packet capture and flushing keys..."
    kill -2 $TCP_DUMP_PID 2>/dev/null || true
    exit 0
}
# Trap SIGTERM (sent by docker compose down/stop)
trap cleanup SIGTERM SIGINT

# Lanzar servidor en background
"$@" &
APP_PID=$!

# Esperar a que termine o reciba señal
wait $APP_PID