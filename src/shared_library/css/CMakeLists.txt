target_sources(
    "${SHARED_LIBRARY_PROJECT_NAME}"
    PRIVATE
        css.cpp
        css_document.cpp
        tokenizer.cpp
        tokenizer.h # this isn't public
        css_class.cpp
        css_selector.cpp
        css_attribute.cpp
    PUBLIC
        css.h
        css_document.h
        css_class.h
        css_selector.h
        css_selector_types.h
        css_attribute.h
)