Ejecuta el plan de implementación paso a paso.

Plan: $ARGUMENTS

Proceso:

1. Lee el plan en `.ai/workspace/plans/$ARGUMENTS`
2. Para cada item del checklist del plan:
   a. Implementa el cambio mínimo necesario
   b. Ejecuta los tests y verifica que pasan
   c. Si un test falla, diagnostica y corrige antes de continuar
   d. Marca el item como completado en el fichero del plan (`- [ ]` → `- [x]`)
   e. Haz commit con un mensaje descriptivo del paso completado
3. No avances al siguiente item sin completar el actual
4. Si encuentras algo inesperado que no estaba en el plan, detente y explica el
   problema

Al finalizar, genera un summary con:

- Objetivo que se implementó
- Cambios realizados por fichero
- Tests añadidos o modificados
- Decisiones tomadas que no estaban en el plan (si las hubo)

Guarda el summary en `.ai/workspace/summaries/` con el mismo nombre que el plan.
