#!/bin/bash
echo "Configurando Escenario B (Restringido IIoT en Endpoints)..."

# Solo operamos en eth0, que es la única interfaz real del contenedor
INT="eth0"

# Limpiar reglas previas por si acaso
tc qdisc del dev $INT root 2>/dev/null

# 1. Reducir MTU a 300 bytes para forzar fragmentación en DTLS
ip link set dev $INT mtu 300

# 2. Aplicar latencia (125ms promedio) y pérdidas (2%)
tc qdisc add dev $INT root netem delay 125ms 50ms loss 2%

echo "Escenario B aplicado en la interfaz $INT con éxito."