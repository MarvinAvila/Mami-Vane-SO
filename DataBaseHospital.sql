
Database Hospital;

create table paciente(
	num_exp serial primary key,
	nombre varchar,
	apellidos varchar,
	fecha_nac date,
	edad int,
	direccion varchar,
	peso varchar,
	estatura varchar,
	telefono int,
	sexo varchar

);

-- Restricciones 
/* 1 La edad no puede ser menor a 18 años
   2 La edad se obtiene a través de la fecha de nacimieneto 
   3 Los telefonos pueden ser Varios (Personal, oficiona, casa, etc)
   4 El sexo solo puede ser (M o F)
*/

create table Medico (
	num_ced serial primary key,
	nombre varchar,
	apellidos varchar,
	fecha-nac date,
	edad int,
	direccion varchar,
	especialidad varchar
);

-- Restricciones 
/* 1 La edad se obtiene a través de la fecha de nacimieneto 
   2 Las especialidades solo son ('medicina general','traumatologia','cardiologia','ginecologia')
*/

create table receta (
	folio serial primary key,
	fecha_receta date,
	lugar varchar,
	nombre_pac varchar,
	edada_pac int,
	nom_medic varchar
);

-- Restricciones 
/* 1 La fecha de elaboracion no puede ser mayor a la fecha actual
   2 El lugar pro default es 'Tapachula, Chiapas' 
*/

create table Medicamentos (
	num_med serial primary key,
	nombre varchar,
	descrip varchar,
	presentacion varchar,
	fecha_caducidad date,
	cantidad varchar
);




-- Base de datos
CREATE DATABASE hospital;
\c hospital;

-- ===========================
-- TABLA PACIENTE
-- ===========================
CREATE TABLE paciente (
    num_exp SERIAL PRIMARY KEY,
    nombre VARCHAR(100) NOT NULL,
    apellidos VARCHAR(100) NOT NULL,
    fecha_nac DATE NOT NULL,
    edad INT,
    direccion VARCHAR(200),
    peso NUMERIC(5,2),
    estatura NUMERIC(4,2),
    sexo CHAR(1) CHECK (sexo IN ('M','F'))
);

-- Trigger function
CREATE OR REPLACE FUNCTION actualizar_edad()
RETURNS TRIGGER AS $$
BEGIN
    NEW.edad := EXTRACT(YEAR FROM age(NEW.fecha_nac));
    IF NEW.edad < 18 THEN
        RAISE EXCEPTION 'El paciente debe ser mayor de 18 años';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Asignar trigger
CREATE TRIGGER set_edad
BEFORE INSERT OR UPDATE ON paciente
FOR EACH ROW
EXECUTE FUNCTION actualizar_edad();


-- ===========================
-- TABLA TELEFONOS (un paciente puede tener varios)
-- ===========================
CREATE TABLE telefono (
    id_tel SERIAL PRIMARY KEY,
    num_exp INT REFERENCES paciente(num_exp) ON DELETE CASCADE,
    telefono VARCHAR(15) NOT NULL,
    tipo VARCHAR(20) CHECK (tipo IN ('personal','oficina','casa','otro'))
);

-- ===========================
-- TABLA MEDICO
-- ===========================
CREATE TABLE medico (
    num_ced SERIAL PRIMARY KEY,
    nombre VARCHAR(100) NOT NULL,
    apellidos VARCHAR(100) NOT NULL,
    fecha_nac DATE NOT NULL,
    edad INT,  -- 👈 se mantiene el campo edad
    direccion VARCHAR(200),
    especialidad VARCHAR(50) CHECK (especialidad IN 
        ('medicina general','traumatologia','cardiologia','ginecologia')
    )
);

-- ===========================
-- FUNCIÓN DEL TRIGGER
-- ===========================
CREATE OR REPLACE FUNCTION actualizar_edad_medico()
RETURNS TRIGGER AS $$
BEGIN
    -- calcular edad con base en la fecha de nacimiento
    NEW.edad := EXTRACT(YEAR FROM age(NEW.fecha_nac));

    -- opcional: validar que el médico tenga al menos 25 años
    IF NEW.edad < 25 THEN
        RAISE EXCEPTION 'El médico debe ser mayor o igual a 25 años';
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- ===========================
-- TRIGGER
-- ===========================
CREATE TRIGGER set_edad_medico
BEFORE INSERT OR UPDATE ON medico
FOR EACH ROW
EXECUTE FUNCTION actualizar_edad_medico();

-- ===========================
-- TABLA MEDICAMENTOS
-- ===========================
CREATE TABLE medicamento (
    num_med SERIAL PRIMARY KEY,
    nombre VARCHAR(100) NOT NULL,
    descrip TEXT,
    presentacion VARCHAR(50),
    fecha_caducidad DATE NOT NULL CHECK (fecha_caducidad > CURRENT_DATE),
    stock INT CHECK (stock >= 0)
);

-- ===========================
-- TABLA RECETA
-- ===========================
CREATE TABLE receta (
    folio SERIAL PRIMARY KEY,
    fecha_receta DATE NOT NULL CHECK (fecha_receta <= CURRENT_DATE),
    lugar VARCHAR(100) DEFAULT 'Tapachula, Chiapas',
    
    -- Relaciones
    num_exp INT REFERENCES paciente(num_exp) ON DELETE CASCADE,
    num_ced INT REFERENCES medico(num_ced) ON DELETE CASCADE
);

-- ===========================
-- TABLA RECETA_MEDICAMENTO (N a N entre receta y medicamentos)
-- ===========================
CREATE TABLE receta_medicamento (
    folio INT REFERENCES receta(folio) ON DELETE CASCADE,
    num_med INT REFERENCES medicamento(num_med) ON DELETE CASCADE,
    cantidad INT CHECK (cantidad > 0),
    PRIMARY KEY (folio, num_med)  -- PK compuesta
);
