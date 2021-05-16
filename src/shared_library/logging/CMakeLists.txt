target_sources(
	"${SHARED_LIBRARY_PROJECT_NAME}"
	PRIVATE
		logger.cpp
	PUBLIC
		logger.h
		consume_logger.h
)