# Plan de refactor: logging local para errores EDHOC

## Objetivo
Sustituir el envío de mensajes de error por payloads EDHOC por logging local con `com_edhoc_log_error`, manteniendo intacto `com_edhoc_add_cipher_suite_mismatch_error`.

## Alcance
- Procesadores/compositores de handshake EDHOC en `src/edhoc/client/handshake/**` y `src/edhoc/server/handshake/**`.
- Capa de setup de contexto y validación de parámetros si todavía propagan buffers de error.
- Despachadores CoAP que consumen los resultados de handshake.
- Tests unitarios e integración que validan payloads de error codificados.

## Decisiones
- Los errores locales se registran con `com_edhoc_log_error("mensaje estático")`.
- En fallos sin payload de error, el resultado devolverá solo estado; no se expondrá un buffer de lectura vacío.
- La ruta especial de `add_cipher_suite_mismatch_error` no se toca.

## Pasos
1. Localizar todas las llamadas a:
   - `com_edhoc_add_internal_error_view`
   - `com_edhoc_add_protocol_error_view`
   - `com_edhoc_add_protocol_error_with_description_view`
2. Reemplazarlas por `com_edhoc_log_error(...)` y devolver estado de fallo.
3. Eliminar o simplificar campos de error en los `*_result` que solo se usaban para payloads de error.
4. Ajustar llamadas encadenadas en responder/dispatcher para trabajar solo con estados.
5. Adaptar tests para validar estados, no payloads de error.
6. Validar compilación y test afectados.

## Riesgos
- Algunos tests o adaptadores pueden seguir esperando `error_buffer`; habrá que limpiar esos contratos.
- `com_edhoc_setup_context` / `com_edhoc_parameters` pueden necesitar alinearse con el nuevo contrato para evitar buffers intermedios.

## Verificación
- Recompilar y ejecutar los tests EDHOC/CoAP afectados.
- Revisar que `com_logging.c` esté enlazado en los targets que usan logging.

