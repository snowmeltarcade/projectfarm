target_sources(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PRIVATE
        logging.cpp
    PUBLIC
        logging.h
        log_levels.h
)