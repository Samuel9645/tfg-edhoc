# TODO: Add acknowledgments for libcoap and libedhoc

## Setup

The project was developed and tested on a Linux environment (WSL 2 on Windows
11). It is recommended to use a similar environment for development and testing.

This project uses submodules to handle external libraries. To clone and
initialize the repository with its submodules, use the following command:

```bash
git clone --recurse-submodules <REPOSITORY_URL>
```

If you have already cloned the repository without the `--recurse-submodules`
flag, you can initialize and update the submodules with the following commands:

```bash
git submodule update --init --recursive
```

## Comparison

To run the comparison, go to the `comparison/scripts` directory:

```bash
cd comparison/scripts
```

Then, execute the `build-and-run-docker.sh` script:

```bash
./build-and-run-docker.sh
```

Network traces will be saved in the `comparison/traces` directory, and the logs
of each of the containers will be on the `comparison/traces/logs`.

Scenario A means regular network conditions, while Scenario B means a network
with the script `comparison/scripts/setup-network/limited-network.sh` applied,
which simulates limited MTU network with package loss.
