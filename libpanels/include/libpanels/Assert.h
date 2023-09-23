#pragma once

#ifndef libpanels_assert
#ifdef LIBPANELS_NO_DEBUG
#define libpanels_assert(x)
#else
#define libpanels_assert(x) assert(x)
#endif
#endif
