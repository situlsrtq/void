#ifndef VOID_TRACY_WRAPPER_H
#define VOID_TRACY_WRAPPER_H

#ifdef TRACY_ENABLE
	#include <tracy/Tracy.hpp>
	#define ZONE_SCOPED ZoneScoped;
#else
	#define ZONE_SCOPED
#endif

#endif
