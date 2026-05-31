#!/bin/bash
set +e

if [ "$#" -lt 3 ]; then
    echo "Error: Missing required arguments."
    echo "Usage: $0 <SCENARIO> <TRACE_NAME> <COMMAND_TO_EXECUTE...>"
    echo "Example: $0 dtls_server A coap-server -v 8 -A 0.0.0.0"
    exit 1
fi

SCENARIO="$1"
TRACE_NAME="$2"
shift 2
FOLDER_NAME="${RUN_TIMESTAMP:""}"
if [ "$FOLDER_NAME" == "" ]; then
    echo "Warning: RUN_TIMESTAMP environment variable not set. Using current timestamp as fallback."
    FOLDER_NAME=$(date +"%Y-%m-%d_%H-%M-%S")
fi

BASE_DIR="/traces/Scenario_${SCENARIO}/${FOLDER_NAME}"
NETWORK_DIR="${BASE_DIR}/network"
LOGS_DIR="${BASE_DIR}/logs"

mkdir -p "${NETWORK_DIR}"
mkdir -p "${LOGS_DIR}"

if [ "${SCENARIO}" = 'B' ]; then
    /scripts/setup/limited-network.sh;
fi

tcpdump -Z root -U -i any -w "${NETWORK_DIR}/${TRACE_NAME}.pcap" "udp port 5683 or udp port 5684" &
TCP_DUMP_PID=$!
sleep 2

cleanup() {
    echo "Stopping packet capture and flushing to disk..."
    kill -2 $TCP_DUMP_PID 2>/dev/null || true
    sleep 1
    exit 0
}
trap cleanup SIGTERM SIGINT
"$@" > "${LOGS_DIR}/${TRACE_NAME}_${SCENARIO}.txt" 2>&1
sleep 3
cleanup