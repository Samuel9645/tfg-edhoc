set -e

cd ../../
rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ../comparison/docker
rm -rf certs
mkdir certs
cd certs
../../scripts/generate-certificates.sh
mkdir -p ../traces/logs
rm -f ../traces/*.pcap
rm -f ../traces/logs/*.txt
cd ../

save_logs() {
    local scenario=$1
    echo "📥 Exporting execution logs for Scenario ${scenario}..."
    for service in dtls-client dtls-server edhoc-client edhoc-server; do
        docker logs iiot-${service} > ../traces/logs/${service//-/_}_"${scenario}".txt 2>&1 || true
    done
}
wait_for_clients() {
    echo "⏳ Waiting for clients to finish execution..."
    # Bucle que comprueba si iiot-dtls-client o iiot-edhoc-client siguen corriendo
    while docker ps --format '{{.Names}}' | grep -qE "iiot-dtls-client|iiot-edhoc-client"; do
        sleep 1
    done
    echo "✅ Clients finished."
}

# =======================================================
# 🚀 TEST MATRIX: SCENARIO A (Stable network)
# =======================================================
echo "=== Running Scenario A Matrix (DTLS & EDHOC) ==="
export SCENARIO=A

docker compose up --build -d
echo "⏳ Waiting for Scenario A clients to finish execution..."
wait_for_clients

# 🚀 LA CLAVE: Damos 3 segundos para que los procesos de tcpdump terminen de escribir el pcap
echo "⏳ Giving tcpdump some time to safely write .pcap to disk..."
sleep 3

save_logs "A"
echo "📥 Flushing packet buffers to disk..."
sleep 2
docker compose down -v --remove-orphans
sleep 2

# =======================================================
# 🚧 TEST MATRIX: SCENARIO B (Restricted network)
# =======================================================
echo "=== Running Scenario B Matrix (DTLS & EDHOC) ==="
export SCENARIO=B

docker compose up --build -d
echo "⏳ Waiting for Scenario B clients to finish execution..."
wait_for_clients

# 🚀 LO MISMO AQUÍ: Margen de seguridad antes del desmontaje destructivo
echo "⏳ Giving tcpdump some time to safely write .pcap to disk..."
sleep 3

save_logs "B"
echo "📥 Flushing packet buffers to disk..."
sleep 2
docker compose down -v --remove-orphans

echo "======================================================="
echo "🎉 SUCCESS: Complete profile-free test matrix finished!"
echo "======================================================="