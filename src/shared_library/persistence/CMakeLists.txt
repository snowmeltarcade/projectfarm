target_sources(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PRIVATE
        database.cpp
        statement.cpp
    PUBLIC
        database.h
        statement.h
)