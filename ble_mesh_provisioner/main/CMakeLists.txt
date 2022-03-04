set(srcs "main.c"
        "prov.c"
        "sensor_client.c"
        "power.c")

idf_component_register(SRCS "${srcs}" 
                    INCLUDE_DIRS  ".")
