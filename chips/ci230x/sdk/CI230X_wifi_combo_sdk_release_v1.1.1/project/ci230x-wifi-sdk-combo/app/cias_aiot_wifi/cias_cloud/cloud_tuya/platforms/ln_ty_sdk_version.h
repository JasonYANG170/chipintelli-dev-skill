/*---------------------------------------------------------------------------*
 | @author  : LightningSemi WLAN Software Team                               |
 |---------------------------------------------------------------------------|
 |                      COPYRIGHT(c) 2018 LightningSemi                      |
 *---------------------------------------------------------------------------*/

#ifndef __LN_TY_SDK_VERSION_H__
#define __LN_TY_SDK_VERSION_H__

/**
 * @brief Tuya and LightningSemi mixed SDK version
 * 
 * LN_TY_SDK_MAJOR_V:    0-255
 * LN_TY_SDK_MINOR_V:    0-255
 * LN_TY_SDK_REVISION_V: 0-255
 *
 * LN_TY_SDK_RC_V:       0:     development
 *                       255:   release
 *                       1-254: release candidates
 *
 * Display style:
 * For the development version, shown like "1.0.0_alpha";
 * For the release     version, shown link "1.0.0";
 * For the RC          version, shown link "1.0.0_rc1".
 */
#define LN_TY_SDK_MAJOR_V        0
#define LN_TY_SDK_MINOR_V        0
#define LN_TY_SDK_REVISION_V     4

/**
 * LN_TY_SDK_RC_V :
 * 0: development, 255: release, 1-254: release candidates
*/
#define LN_TY_SDK_RC_V           3


#define LN_TY_SDK_VERSTR2(x)     #x
#define LN_TY_SDK_VERSTR(x)      LN_TY_SDK_VERSTR2(x)

#define LN_TY_SDK_RC_RELEASE     255
#define LN_TY_SDK_RC_DEVELOPMENT 0

#undef  VERSION_IS_RELEASE
#undef  VERSION_IS_DEVELOPMENT
#undef  VERSION_IS_RC
#define VERSION_IS_RELEASE     (LN_TY_SDK_RC_V == LN_TY_SDK_RC_RELEASE)
#define VERSION_IS_DEVELOPMENT (LN_TY_SDK_RC_V == LN_TY_SDK_RC_DEVELOPMENT)
#define VERSION_IS_RC          ((LN_TY_SDK_RC_V != LN_TY_SDK_RC_RELEASE) &&    \
                                (LN_TY_SDK_RC_V != LN_TY_SDK_RC_DEVELOPMENT))

#if VERSION_IS_RELEASE
#define LN_TY_SDK_VERSION_STRING_SUFFIX  ""
#elif VERSION_IS_DEVELOPMENT
#define LN_TY_SDK_VERSION_STRING_SUFFIX  "_alpha"
#else
#define LN_TY_SDK_VERSION_STRING_SUFFIX  "_rc" LN_TY_SDK_VERSTR(LN_TY_SDK_RC_V)
#endif

#define LN_TY_SDK_VERSION    ((uint32_t)(LN_TY_SDK_MAJOR_V) << 24 |            \
                              (uint32_t)(LN_TY_SDK_MINOR_V) << 16 |            \
                              (uint32_t)(LN_TY_SDK_REVISION_V) << 8 |          \
                              (uint32_t)(LN_TY_SDK_RC_V))

#define LN_TY_SDK_VERSION_STRING    LN_TY_SDK_VERSTR(LN_TY_SDK_MAJOR_V) "."    \
                                    LN_TY_SDK_VERSTR(LN_TY_SDK_MINOR_V) "."    \
                                    LN_TY_SDK_VERSTR(LN_TY_SDK_REVISION_V)     \
                                    LN_TY_SDK_VERSION_STRING_SUFFIX

#define LN_TY_SDK_BUILD_DATE_TIME  "2022-11-04_11:36:16"
#define LN_TY_SDK_GIT_INFO         "_"

#endif /* __LN_TY_SDK_VERSION_H__ */
