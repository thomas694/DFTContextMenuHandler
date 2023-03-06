#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <WinSDKVer.h>

#undef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_LONGHORN
#define _WIN32_IE      0x0700

#include <SDKDDKVer.h>
