Revisa la implementación realizada y evalúa su calidad.

Proceso:

1. Lee el summary en `.ai/workspace/summaries/` para entender qué se implementó
   y qué decisiones se tomaron
2. Ejecuta `git diff main` para ver todos los cambios de la rama respecto a main
3. Evalúa los cambios en estas dimensiones:
    - **Corrección**: ¿los cambios hacen lo que describe el summary?
    - **Reglas**: ¿respetan las convenciones definidas en AGENTS.md y docs/?
    - **Tests**: ¿los cambios tienen cobertura de tests suficiente?
    - **Seguridad**: ¿se introducen vulnerabilidades o se exponen datos
      sensibles?

Formato de salida:

- ✓ Puntos positivos
- ✗ Problemas a corregir (con fichero y línea)
- ⚠ Sugerencias opcionales de mejora
