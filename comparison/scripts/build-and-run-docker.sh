set -e

#cd ../../
#rm -rf build
#mkdir build
#cd build
#cmake -DCMAKE_BUILD_TYPE=Release ..
#make
#cd ../comparison/docker
cd ../docker
DTLS_SERVER_NAME=$(grep -v '^#' .env | grep 'DTLS_SERVER_IP' | cut -d= -f2 | tr -d '\r')
DTLS_CLIENT_NAME=$(grep -v '^#' .env | grep 'DTLS_CLIENT_IP' | cut -d= -f2 | tr -d '\r')
rm -rf certs
mkdir certs
cd certs
../../scripts/generate-credentials/generate-credentials.sh "$DTLS_SERVER_NAME" "$DTLS_CLIENT_NAME"
cd ../
RUN_TIMESTAMP=$(date +"%d-%m-%Y-_%H-%M-%S")
export RUN_TIMESTAMP

wait_for_clients() {
    echo "⏳ Waiting for clients to finish execution..."
    while docker ps --format '{{.Names}}' | grep -qE "iiot-dtls-client|iiot-edhoc-client"; do
        sleep 1
    done
    echo "✅ Clients finished."
}

echo "=== Running Scenario A (DTLS & EDHOC) ==="
export SCENARIO=A

docker compose up --build -d
wait_for_clients
docker compose down -v --remove-orphans

echo "=== Running Scenario B Matrix (DTLS & EDHOC) ==="
export SCENARIO=B

docker compose up --build -d
wait_for_clients
docker compose down -v --remove-orphans

echo "========================================================="
echo "          ✅ All scenarios completed."
echo "Traces and logs are available in the 'traces' directory."
echo "========================================================="