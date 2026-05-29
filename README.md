# TODO: Add acknowledgments for libcoap and libedhoc

This project uses submodules to handle external libraries. To clone and
initialize the repository with its submodules, use the following command:

```bash
git clone --recurse-submodules <URL_DEL_REPOSITORIO>
```

If you have already cloned the repository without the `--recurse-submodules`
flag, you can initialize and update the submodules with the following commands:

```bash
git submodule update --init --recursive
```
