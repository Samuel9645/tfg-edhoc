# EDHOC + OSCORE emulation

This project implements an emulation of the EDHOC (Ephemeral Diffie-Hellman Over
COSE) protocol combined with OSCORE (Object Security for Constrained
RESTful Environments).

This project uses the following libraries:

- [libedhoc](https://github.com/kamil-kielbasa/libedhoc) a C library for the
  EDHOC protocol.
- [libcoap](https://github.com/obgm/libcoap) a C library for the CoAP protocol,
  it also adds support for OSCORE.
- [unity](https://github.com/ThrowTheSwitch/Unity.git) a C unit testing
  framework. Note: Ruby needs to be installed in order to be able to run the
  tests using CMake as this project uses the script described
  in [UnityHelperScriptsGuide.md](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
  to avoid having to boilerplate code for the tests.

## Setup

The project was developed and tested on a Linux environment (WSL 2 on Windows
11). It is recommended to use a similar environment for development and testing.

This project uses submodules to handle external libraries. To clone and
initialize the repository with its submodules, use the following command:

```bash
git clone --recurse-submodules https://github.com/Samuel9645/tfg-edhoc.git
```

If you have already cloned the repository without the `--recurse-submodules`
flag, you can initialize and update the submodules with the following commands:

```bash
git submodule update --init --recursive
```

## Comparison

To run the comparison between DTLS (using libcoap precompiled client and server
binaries) and this project EDHOC + OSCORE approach, go to the
`comparison/scripts` directory:

```bash
cd comparison/scripts
```

Then, execute the `build-and-run-docker.sh` script:

```bash
./build-and-run-docker.sh
```

Network traces will be saved in the `comparison/traces` directory.
The traces are grouped by scenarios.

Scenario A means regular network conditions, while Scenario B means a network
with the script `comparison/scripts/setup-network/limited-network.sh` applied,
which simulates limited MTU network with package loss.

Then for each scenario a new folder with the timestamps of the execution will be
created, and this will contain a folder with the network traces for both DTLS
and EDHOC + OSCORE as well a logs folder with the logs of the containers
executing the tests.
