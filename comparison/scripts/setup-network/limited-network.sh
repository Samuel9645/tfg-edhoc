#!/bin/bash

INT="eth0"
tc qdisc del dev $INT root 2>/dev/null
ethtool -K $INT tso off gfo off gso off gro off 2>/dev/null || true
ip link set dev $INT mtu 300
tc qdisc add dev $INT root netem delay 125ms 50ms loss 2%