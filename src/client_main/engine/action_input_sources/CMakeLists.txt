target_sources(
    "${CLIENT_PROJECT_NAME}"
    PUBLIC
        action_input_source_keyboard.h
        key_mapping.h
    PRIVATE
        action_input_source_keyboard.cpp
        key_mapping.cpp
)