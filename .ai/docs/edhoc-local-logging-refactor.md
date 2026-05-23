# Refactor: EDHOC local error logging

## Summary

Se sustituyó la transmisión de errores EDHOC por payload por logging local con
`com_edhoc_log_error` en los flujos afectados.

## Cambios principales

- `src/edhoc/common/com_logging.c` ahora corta la ejecución cuando recibe
  `NULL`.
- `com_edhoc_setup_context` y `com_edhoc_validate_parameters` pasan a devolver
  solo estado y registrar el error localmente.
- Los procesadores/compositores EDHOC afectados usan logging local en vez de
  construir payloads de error, salvo la ruta especial de
  `com_edhoc_add_cipher_suite_mismatch_error`.
- Los tests actualizados dejan de verificar payloads de error para los casos
  locales y solo validan estado, manteniendo la cobertura del caso especial de
  cipher suite mismatch.

## Verification

- Compilación de los tests afectados.
- Ejecución de la batería de tests afectada con `ctest`.

