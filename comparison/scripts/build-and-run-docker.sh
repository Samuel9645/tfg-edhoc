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
docker compose down -v --remove-orphans

save_logs() {
    local scenario=$1
    echo "📥 Exporting execution logs for Scenario ${scenario}..."
    for service in dtls-client dtls-server edhoc-client edhoc-server; do
        docker logs iiot-${service} > ../traces/logs/${service//-/_}_"${scenario}".txt 2>&1 || true
    done
}

# =======================================================
# 🚀 TEST MATRIX: SCENARIO A (Stable network)
# =======================================================
echo "=== Running Scenario A Matrix (DTLS & EDHOC) ==="
SCENARIO=A docker compose up --build -d
echo "⚙️ Injecting Scenario A parameters into router..."
docker exec iiot-router /bin/bash /scripts/regular-network.sh
echo "⏳ Waiting for Scenario A clients to finish execution..."
docker compose wait dtls-client edhoc-client
save_logs "A"
echo "📥 Flushing packet buffers to disk..."
sleep 2
docker compose down -v --remove-orphans
sleep 2

# =======================================================
# 🚧 TEST MATRIX: SCENARIO B (Restricted network)
# =======================================================
echo "=== Running Scenario B Matrix (DTLS & EDHOC) ==="
SCENARIO=B docker compose up -d
echo "⚙️ Injecting Scenario B constraints into router..."
docker exec iiot-router /bin/bash /scripts/limited-network.sh
echo "⏳ Waiting for Scenario B clients to finish execution..."
docker compose wait dtls-client edhoc-client
save_logs "B"
echo "📥 Flushing packet buffers to disk..."
sleep 2

# Final cleanup teardown
docker compose down -v --remove-orphans

echo "======================================================="
echo "🎉 SUCCESS: Complete profile-free test matrix finished!"
echo "======================================================="