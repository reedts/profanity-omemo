set (
	PROF_OMEMO_SRC
	omemo_device.h
	omemo_device.c
)

add_library (profanity-omemo SHARED ${PROF_OMEMO_SRC})
target_link_libraries (profanity-omemo "${LIBSIGNAL}")
