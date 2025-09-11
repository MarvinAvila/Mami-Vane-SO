---

```markdown
# 🏥 Proyecto Hospital CRUD en C

Este proyecto implementa un sistema CRUD para la gestión hospitalaria utilizando el lenguaje C y PostgreSQL como base de datos. Está dividido en múltiples archivos para mantener una estructura modular y clara.

## 📁 Estructura del proyecto

```
├── main.c
├── database.c / database.h
├── paciente.c / paciente.h
├── medico.c / medico.h
├── medicamento.c / medicamento.h
├── receta.c / receta.h
├── Makefile
```

## ⚙️ Requisitos

- Sistema operativo Linux
- Compilador GCC
- PostgreSQL instalado y corriendo
- Librería de desarrollo de PostgreSQL:
  ```bash
  sudo apt install libpq-dev
  ```

## 🚀 Compilación

Para compilar el proyecto, simplemente ejecuta:

```bash
make
```

Esto generará un ejecutable llamado `hospital_crud`.

## ▶️ Ejecución

Una vez compilado, puedes ejecutar el programa con:

```bash
./hospital_crud
```

Asegúrate de que el servidor de PostgreSQL esté activo y que la base de datos esté correctamente configurada.

## 🧹 Limpieza

Para eliminar los archivos objeto y el ejecutable:

```bash
make clean
```

## 🧠 Notas técnicas

- El proyecto utiliza `-Wall` para mostrar advertencias durante la compilación.
- La conexión con PostgreSQL se realiza mediante la librería `libpq`.
- El Makefile compila automáticamente todos los archivos `.c` listados en la variable `SRCS`.

## 📌 Autor

**Marvin Avila**  
Correo: marvinavila2002@gmail.com  
GitHub: [MarvinAvila](https://github.com/MarvinAvila)

---

```