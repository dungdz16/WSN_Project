set(srcs "main.c"
        "fast_prov_client.c"
        "sensor_client.c"
        "power.c")

idf_component_register(SRCS "${srcs}" 
                    INCLUDE_DIRS  ".")
