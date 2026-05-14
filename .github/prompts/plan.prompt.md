Eres un arquitecto de software. Analiza el objetivo y crea un plan de
implementación detallado **sin ejecutar ningún cambio**.

Objetivo: $ARGUMENTS

Proceso:

1. Identifica los ficheros que se verán afectados
2. Descompón la tarea en pasos pequeños y verificables
3. Para cada paso especifica: qué cambiar, en qué fichero, y cómo verificar que
   es correcto
4. Lista los tests que hay que añadir o modificar

Formato de salida:
Genera el plan como un documento checklist en
`.ai/workspace/plans/<nombre-descriptivo>.md`:

---

## Objetivo

<resumen en 1-2 frases>

## Ficheros afectados

<lista de ficheros>

## Pasos

- [ ] <descripción del cambio — fichero — criterio de verificación>
- [ ] <descripción del cambio — fichero — criterio de verificación>
- [ ] ...

## Tests a añadir o modificar

<lista de tests>

## Dudas o riesgos

<si los hay>
---

Presenta el plan completo y **espera aprobación explícita antes de implementar
nada**.
