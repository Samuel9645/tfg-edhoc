# Plan de implementación: Diccionario interno de almacenamiento de sesiones

## Objetivo

Mover el almacenamiento de contexto de sesión EDHOC desde los datos de la sesión
CoAP a un diccionario estático interno que mapea IDs de conexión a contextos
EDHOC. Esto desvincula el ciclo de vida de la sesión EDHOC del ciclo de vida de
la sesión CoAP.

## Ficheros afectados

### Nuevos ficheros (a crear)

- `src/coap/server/internal/srv_session.h` — Interfaz del diccionario
- `src/coap/server/internal/srv_session.c` — Implementación del diccionario

### Ficheros existentes a modificar

- `src/coap/server/internal/srv_dispatch_engine.h` — Actualizar struct
  `srv_coap_dispatch_deps`: remover `get_session_app_data`, añadir
  `set_context_by_cid`, `get_context_by_cid`, `remove_context_by_cid`
- `src/coap/server/internal/srv_dispatch_engine.c` — Actualizar lógica de
  dispatcher: guardar M1 en diccionario, recuperar M3 del diccionario, limpiar
  diccionario en errores
- `src/coap/server/edhoc_message_process/srv_coap_m1_process.h` — Simplificar
  interfaz: remover parámetro `coap_session_t* session`
- `src/coap/server/edhoc_message_process/srv_coap_m1_process.c` — Simplificar
  implementación: solo mapear status a código CoAP, sin gestión de diccionario
- `src/coap/server/srv_dispatch.c` — Inyectar dependencias del diccionario en
  `COAP_SERVER_EDHOC_DISPATCH_DEFAULT_DEPS`, remover `get_session_app_data`

### Ficheros de test a actualizar

- `tests/src/coap/server/dispatch/test_coap_server_dispatch.c` — Actualizar
  tests del dispatcher para validar diccionario
- `tests/src/coap/server/dispatch/tst_create_base_dependencies.c` — Crear stub
  del diccionario
- `tests/src/coap/server/dispatch/tst_srv_dispatch_stubs.h` — Declarar stubs del
  diccionario
- `tests/src/coap/server/dispatch/tst_srv_dispatch_stubs.c` — Implementar stubs
  del diccionario
- `tests/src/edhoc/server/handshake/message_1/test_edhoc_message_1_process.c` —
  Simplificar tests de M1 (ya no mockean CoAP session)

## Pasos de implementación

### Fase 1: Crear interfaz y stub del diccionario

- [ ] **Crear header del diccionario** —
  `src/coap/server/internal/srv_session.h` — Criterio: Define tipos y funciones
  sin lógica de almacenamiento
    - Enum de estados: `srv_session_status` (OK, FULL, DUPLICATE_CID, NOT_FOUND,
      NULL_CONTEXT, etc.) con prefijo `SRV_SESSION_`
    - Struct de resultado: `struct srv_session_result` (status + small payload
      si es necesario)
    - Funciones públicas:
        - `srv_session_set_context_by_cid()` — Registrar contexto
        - `srv_session_get_context_by_cid()` — Recuperar contexto
        - `srv_session_remove_context_by_cid()` — Eliminar contexto
        - `srv_session_clear_all()` — Limpiar todo (para tests)

- [ ] **Crear interfaz de dependencias del diccionario** —
  `src/coap/server/internal/srv_dispatch_engine.h` — Criterio: Extiende
  `srv_coap_dispatch_deps` con función pointers del diccionario
    - Typedefs para set, get, remove
    - Actualizar struct `srv_coap_dispatch_deps`:
        - **Remover**: `srv_coap_get_session_app_data_fn get_session_app_data`
        - **Añadir**: `srv_session_set_context_fn set_context_by_cid`
        - **Añadir**: `srv_session_get_context_fn get_context_by_cid`
        - **Añadir**: `srv_session_remove_context_fn remove_context_by_cid`

- [ ] **Implementar diccionario estático** —
  `src/coap/server/internal/srv_session.c` — Criterio: Array fijo de entradas,
  tamaño configurable (10 sesiones máximo por ahora)
    - Struct interno de entrada: `{ cid, edhoc_context* }`
    - Funciones de búsqueda, inserción, eliminación
    - Validaciones: CID duplicado, diccionario lleno, contexto NULL

### Fase 2: Actualizar procesador de Message 1

- [ ] **Simplificar M1 process** —
  `src/coap/server/edhoc_message_process/srv_coap_m1_process.c` — Criterio: M1
  process SÍ mapea status a código CoAP, pero NO gestiona diccionario
    - `srv_coap_process_message_1_result()` mantiene firma actual (sin
      diccionario)
    - Función solo mapea enum `srv_edhoc_message_1_responder_status` →
      `coap_pdu_code_t`
    - Loggea errores si `status != SRV_EDHOC_MSG1_RESPONDER_OK`
    - **No intenta guardar en CoAP session** (el dispatcher lo hará
      explícitamente)
    - **No toca el diccionario**

- [ ] **Remover referencia a CoAP session** —
  `src/coap/server/edhoc_message_process/srv_coap_m1_process.h` — Criterio:
  Interfaz simplificada
    - `srv_coap_process_message_1_result()` retorna solo `coap_pdu_code_t`
    - Ya no recibe parámetro `coap_session_t* session`
    - Documentar: Esta función es un puro adaptador EDHOC → CoAP

### Fase 3: Actualizar dispatcher — Guardado y eliminación explícitos

- [ ] **Registrar contexto después de M1 éxito** —
  `src/coap/server/internal/srv_dispatch_engine.c` — Criterio: Dispatcher
  registra explícitamente en diccionario
    - Tras `respond_to_message_1()` exitoso, ANTES de mapear resultado:
        - Extraer CID del contexto (`edhoc_context->private_cid`)
        - Llamar `deps->set_context_by_cid(cid, message_1_result.edhoc_ctx)`
        - Si falla: limpiar contexto, retornar `INTERNAL_ERROR`
        - Si éxito: continuar con mapeo de status

- [ ] **Recuperar contexto antes de M3** —
  `src/coap/server/internal/srv_dispatch_engine.c` — Criterio: Dispatcher
  obtiene contexto del diccionario en lugar de CoAP session
    - En `route_and_process_edhoc_message()`, reemplazar:
        - `edhoc_ctx = deps->get_session_app_data(session)`
        - CON `edhoc_ctx = deps->get_context_by_cid(extracted_cid.cid)` después
          de extraer CID
    - Validar que contexto no sea NULL (fallo = `INTERNAL_ERROR`)

- [ ] **Limpiar diccionario en errores** —
  `src/coap/server/internal/srv_dispatch_engine.c` — Criterio: Dispatcher
  elimina explícitamente del diccionario
    - En sección final de error (M1 o M3 retorna `INTERNAL_ERROR` o
      `BAD_REQUEST`):
        - Extraer CID del contexto (si contexto válido)
        - Llamar `deps->remove_context_by_cid(cid)` para remover del diccionario
        - Luego `srv_edhoc_cleanup_context(&edhoc_ctx)` para liberar memoria
        - Registrar log de limpieza

### Fase 4: Inyectar dependencias en producción

- [ ] **Actualizar dispatcher público** — `src/coap/server/srv_dispatch.c` —
  Criterio: Diccionario está disponible como dependencia
    - Actualizar `COAP_SERVER_EDHOC_DISPATCH_DEFAULT_DEPS`
    - Inyectar funciones del diccionario: `set_context_by_cid`,
      `get_context_by_cid`, `remove_context_by_cid`

- [ ] **Remover CoAP session app-data callback** —
  `src/coap/server/srv_dispatch.c` — Criterio: Ya no usamos app-data para
  guardar contexto
    - Remover dependencia `get_session_app_data = coap_session_get_app_data`
    - Ya no se necesita pasar `coap_session_t*` al procesador M1

### Fase 5: Actualizar tests — Stubs y mocks

- [ ] **Crear helpers de diccionario en tests** —
  `tests/src/coap/server/dispatch/tst_srv_dispatch_stubs.c` — Criterio: Stubs
  simulan diccionario con lista simple
    - `stb_srv_session_set_context_by_cid()` — Mock almacena en array temporal
    - `stb_srv_session_get_context_by_cid()` — Mock recupera desde array
    - `stb_srv_session_remove_context_by_cid()` — Mock elimina
    - `stb_srv_session_clear_all()` — Mock limpia
    - Helpers para forzar fallos: "diccionario lleno", "CID duplicado"
    - **Nota**: Los stubs se usan **en el dispatcher**, no en M1 process

- [ ] **Declarar stubs en header de tests** —
  `tests/src/coap/server/dispatch/tst_srv_dispatch_stubs.h` — Criterio:
  Funciones disponibles
    - Declarar stubs del diccionario
    - Declarar helpers de forzado de fallos

- [ ] **Actualizar creador de dependencias de test** —
  `tests/src/coap/server/dispatch/tst_create_base_dependencies.c` — Criterio:
  Base dependencies incluye diccionario
    - `test_srv_coap_dispatch_create_base_dependencies()` retorna struct con
      stubs del diccionario
    - **Remover**: `get_session_app_data = stb_srv_coap_get_session_null`
    - **Añadir**: `set_context_by_cid = stb_srv_session_set_context_by_cid`
    - **Añadir**: `get_context_by_cid = stb_srv_session_get_context_by_cid`
    - **Añadir**:
      `remove_context_by_cid = stb_srv_session_remove_context_by_cid`

### Fase 6: Escribir tests de dispatcher

- [ ] **Test: Dispatcher registra M1 en diccionario** —
  `tests/src/coap/server/dispatch/test_coap_server_dispatch.c` — Criterio:
  Dispatcher llama `set_context_by_cid()` tras M1 exitoso
    - Enviar M1 válido
    - Verificar que stub `set_context_by_cid()` fue llamado con CID correcto
    - Verificar que respuesta es `2.04 Changed`
    - Verificar que contexto está registrado en diccionario de stub

- [ ] **Test: Dispatcher limpia cuando diccionario lleno** —
  `tests/src/coap/server/dispatch/test_coap_server_dispatch.c` — Criterio:
  Dispatcher retorna error, limpia contexto
    - Configurar stub para simular diccionario lleno en `set_context_by_cid()`
    - Enviar M1 válido
    - Verificar respuesta `5.00 Internal Server Error`
    - Verificar que contexto fue limpiado (mock de `srv_edhoc_cleanup_context`
      fue llamado)

- [ ] **Test: Dispatcher limpia cuando CID duplicado** —
  `tests/src/coap/server/dispatch/test_coap_server_dispatch.c` — Criterio:
  Dispatcher retorna error en CID duplicado
    - Configurar stub para simular CID duplicado en `set_context_by_cid()`
    - Enviar M1 válido
    - Verificar respuesta `5.00 Internal Server Error`
    - Verificar que contexto fue limpiado

- [ ] **Test: Dispatcher recupera M3 del diccionario** —
  `tests/src/coap/server/dispatch/test_coap_server_dispatch.c` — Criterio:
  Dispatcher llama `get_context_by_cid()` para M3
    - Simular estado post-M1 (contexto en diccionario de stub)
    - Enviar M3 válido con CID correcto
    - Verificar que stub `get_context_by_cid()` fue llamado
    - Verificar que M3 se procesa correctamente

- [ ] **Test: Dispatcher retorna error si contexto no existe** —
  `tests/src/coap/server/dispatch/test_coap_server_dispatch.c` — Criterio: M3
  falla cuando `get_context_by_cid()` retorna NULL
    - Configurar stub para simular contexto no encontrado
    - Enviar M3
    - Verificar respuesta `5.00 Internal Server Error`

- [ ] **Test: Dispatcher retorna error en mismatch de CID** —
  `tests/src/coap/server/dispatch/test_coap_server_dispatch.c` — Criterio: M3
  falla si CID extraído ≠ CID en contexto
    - Enviar M3 con CID que no coincide con contexto en diccionario
    - Verificar respuesta `5.00 Internal Server Error`

- [ ] **Test: Dispatcher limpia diccionario en error** —
  `tests/src/coap/server/dispatch/test_coap_server_dispatch.c` — Criterio:
  `remove_context_by_cid()` es llamado en M1/M3 error
    - Simular error en M1 o M3
    - Verificar que stub `remove_context_by_cid()` fue invocado con CID correcto

### Fase 7: Verificaciones de compilación y seguridad

- [ ] **Compilar sin advertencias** — En project root — Criterio:
  `cmake --build <build-dir>` sin warnings
    - Verificar `-Wall -Wextra -Wpedantic -Werror`

- [ ] **Ejecutar tests del dispatcher** —
  `ctest --test-dir <build-dir> -R dispatch` — Criterio: Todos los tests pasan
    - Verificar no hay memory leaks (ASAN)
    - Verificar no hay undefined behavior (UBSAN)

- [ ] **Ejecutar todos los tests** — `ctest --test-dir <build-dir>` — Criterio:
  Suite completa pasa sin regresiones
    - Verificar cobertura de nuevos caminos de código

## Tests a crear o modificar

### Nuevos tests

En `tests/src/coap/server/session/test_srv_session.c`

- `test_session_set_context()` — Verificar registro en diccionario
- `test_session_get_context()` — Verificar recuperación desde diccionario
- `test_session_remove_context()` — Verificar eliminación
- `test_session_full()` — Diccionario lleno rechaza nuevas inserciones
- `test_session_fails_on_duplicate_cid()` — CID duplicado rechazado

En `tests/src/coap/server/dispatch/test_coap_server_dispatch.c`

- `test_server_dispatch_m1_registers_context_in_dict()` — **Dispatcher**
  registra contexto en diccionario tras M1
- `test_server_dispatch_m1_fails_on_dict_full()` — **Dispatcher** limpia
  contexto y retorna error cuando diccionario está lleno
- `test_server_dispatch_m1_fails_on_duplicate_cid()` — **Dispatcher** limpia
  contexto y retorna error con CID duplicado
- `test_server_dispatch_m3_retrieves_context_from_dict()` — **Dispatcher**
  recupera contexto del diccionario para M3
- `test_server_dispatch_m3_fails_on_context_not_found()` — **Dispatcher**
  retorna error si contexto ausente
- `test_server_dispatch_m3_fails_on_cid_mismatch()` — **Dispatcher** retorna
  error si CID no coincide
- `test_server_dispatch_cleanup_removes_from_dict_on_error()` — **Dispatcher**
  elimina del diccionario en errores

### Tests a modificar

- `test_coap_server_dispatch.c` — Reemplazar mocks de CoAP session por mocks de
  diccionario
- `test_edhoc_message_1_process.c` — Simplificar (M1 process ya no toca
  diccionario, solo mapea status)

## Dudas o riesgos

### Riesgos identificados

1. **Seguridad de concurrencia**: El diccionario estático es compartido entre
   todas las sesiones. Si el servidor maneja concurrencia, se necesita mutex (
   revisar modelo de concurrencia actual).
2. **Tamaño máximo del diccionario**: Configurar capacidad adecuada. Si se
   llena, nuevos EDHOC fallan. Considerar edad de entradas para limpiar
   automáticamente.
3. **Extracción de CID**: El dispatcher necesita extraer el CID del contexto
   para guardarlo/recuperarlo. Verificar que libedhoc proporciona función para
   esto.

### Ventajas de esta arquitectura

- **Separación clara**: M1/M3 process solo mapean status → CoAP code. El
  dispatcher orquesta.
- **Responsabilidad única**: Dispatcher es responsable de set/get/remove en
  diccionario.
- **Testeable**: Stubs del diccionario inyectables en dispatcher.
- **Sin acoplamiento**: M1 process no conoce ni CoAP session ni diccionario.

---

**Fecha de creación**: 2026-05-14  
**Estado**: Preparado para aprobación
