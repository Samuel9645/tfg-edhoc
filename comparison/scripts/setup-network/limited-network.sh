#!/bin/bash
echo "Configurando Escenario B (Restringido IIoT)..."

# Activar el forwarding
sysctl -w net.ipv4.ip_forward=1

for INT in eth0 eth1; do
    # Limpiar reglas previas
    tc qdisc del dev $INT root 2>/dev/null

    # 1. Reducir MTU a 300 bytes para forzar fragmentación
    ip link set dev $INT mtu 300

    # 2. Aplicar latencia (125ms promedio) y pérdidas (2%)
    tc qdisc add dev $INT root netem delay 125ms 50ms loss 2%
done

echo "Escenario B aplicado. MTU=300, Latencia=125ms, Pérdidas=2%"