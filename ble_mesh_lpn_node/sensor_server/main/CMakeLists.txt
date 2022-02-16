# Edit following two lines to set component requirements (see docs)
set(srcs "main.c"
        "sensor_server.c"
        "fast_prov.c"
        "power.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  ".")
