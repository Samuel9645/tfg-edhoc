#!/bin/bash
# 🚀 ¡CLAVE! Eliminamos 'set -e' para que un código de salida de la app no rompa el guardado
set +e

TRACE_NAME="$1"
shift

echo "📸 Starting packet capture: /traces/${TRACE_NAME}.pcap"

# Lanzamos tcpdump de fondo como root y sin buffers
tcpdump -Z root -U -i any -w "/traces/${TRACE_NAME}.pcap" "udp port 5683 or udp port 5684" &
TCP_DUMP_PID=$!
sleep 2


cleanup() {
    echo "🐳 Stopping packet capture and flushing to disk..."
    kill -2 $TCP_DUMP_PID 2>/dev/null || true
    sleep 1  # Damos un segundo físico para que el archivo escriba los descriptores
    exit 0
}

# Si docker para el servidor, capturamos la señal
trap cleanup SIGTERM SIGINT

# Ejecutamos la aplicación en primer plano de forma normal
"$@"
APP_EXIT_CODE=$? # Guardamos el código de salida por si acaso
sleep 3
echo "🏁 Application finished with code ${APP_EXIT_CODE}. Closing capture..."

# 🚀Pase lo que pase, obligamos a pasar por cleanup para salvar el .pcap
cleanup