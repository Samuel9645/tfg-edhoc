cd ../docker
DTLS_SERVER_NAME=$(grep -v '^#' .env | grep 'DTLS_SERVER_IP' | cut -d= -f2 | tr -d '\r')
DTLS_CLIENT_NAME=$(grep -v '^#' .env | grep 'DTLS_CLIENT_IP' | cut -d= -f2 | tr -d '\r')
rm -rf certs
mkdir certs
cd certs
../../scripts/generate-credentials/generate-credentials.sh "$DTLS_SERVER_NAME" "$DTLS_CLIENT_NAME"
cd ../
RUN_TIMESTAMP=$(date +"%d-%m-%Y_%H-%M-%S")
export RUN_TIMESTAMP

wait_for_clients() {
    echo "⏳ Waiting for clients to finish execution..."
    while docker ps --format '{{.Names}}' | grep -qE "iiot-dtls-client|iiot-edhoc-client"; do
        sleep 1
    done
    echo "✅ Clients finished."
}
dump_docker_logs() {
    local scenario=$1
    local log_dir="../traces/Scenario_${scenario}/${RUN_TIMESTAMP}/logs"
    mkdir -p "$log_dir"

    echo "📥 Saving container logs via docker logs..."
    docker logs iiot-dtls-server > "${log_dir}/dtls_server.txt" 2>&1
    docker logs iiot-dtls-client > "${log_dir}/dtls_client.txt" 2>&1
    docker logs iiot-edhoc-server > "${log_dir}/edhoc_server.txt" 2>&1
    docker logs iiot-edhoc-client > "${log_dir}/edhoc_client.txt" 2>&1
}

echo "=== Running Scenario A (DTLS & EDHOC) ==="
export SCENARIO=A

docker compose up --build -d
wait_for_clients
dump_docker_logs "A"
docker compose down -v --remove-orphans

echo "=== Running Scenario B (DTLS & EDHOC) ==="
export SCENARIO=B

docker compose up --build -d
wait_for_clients
dump_docker_logs "B"
docker compose down -v --remove-orphans

echo "========================================================="
echo "          ✅ All scenarios completed."
echo "Traces and logs are available in the 'traces' directory."
echo "========================================================="