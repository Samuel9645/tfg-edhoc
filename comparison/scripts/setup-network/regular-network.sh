#!/bin/bash
echo "Configurando Escenario A (Limpio)..."

# Activar el forwarding en el router para que pasen paquetes de una red a otra
sysctl -w net.ipv4.ip_forward=1

# Limpiar y resetear interfaces a valores por defecto
for INT in eth0 eth1; do
    tc qdisc del dev $INT root 2>/dev/null
    ip link set dev $INT mtu 1500
done

echo "Escenario A aplicado con éxito."