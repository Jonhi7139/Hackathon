-- Table: public.sensor_telemetry

-- DROP TABLE IF EXISTS public.sensor_telemetry;

CREATE TABLE IF NOT EXISTS public.sensor_telemetry
(
    id integer NOT NULL DEFAULT nextval('sensor_telemetry_id_seq'::regclass),
    ax real,
    ay real,
    az real,
    gx real,
    gy real,
    gz real,
    temp_i real,
    time_raw bigint,
    temp_o real,
    humidity real,
    pressure real,
    gas bigint,
    mx smallint,
    my smallint,
    mz smallint,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT sensor_telemetry_pkey PRIMARY KEY (id)
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.sensor_telemetry
    OWNER to postgres;
