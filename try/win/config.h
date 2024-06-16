#pragma once

#include "wt_types.h"

#define SQL_STMT_MAX_LEN	128

#define INPUT_BUF_INPUT_MAX     (1<<16)
#define EDITW_DEFAULT_BLOCK     (1<<16)

typedef struct MessageTask
{
    struct MessageTask* next;
    volatile LONG  msg_state;
    U8* msg_body;
    U32 msg_length;
} MessageTask;

/*
 * libCurl Proxy type. Please check: https://curl.se/libcurl/c/CURLOPT_PROXYTYPE.html
 * 0 - No Proxy
 * 1 - CURLPROXY_HTTP
 * 2 - CURLPROXY_HTTPS
 * 3 - CURLPROXY_HTTPS2
 * 4 - CURLPROXY_HTTP_1_0
 * 5 - CURLPROXY_SOCKS4
 * 6 - CURLPROXY_SOCKS4A
 * 7 - CURLPROXY_SOCKS5
 * 8 - CURLPROXY_SOCKS5_HOSTNAME
 */
#define AI_CURLPROXY_NO_PROXY          0
#define AI_CURLPROXY_HTTP              1
#define AI_CURLPROXY_HTTPS             2
#define AI_CURLPROXY_HTTPS2            3
#define AI_CURLPROXY_HTTP_1_0          4
#define AI_CURLPROXY_SOCKS4            5
#define AI_CURLPROXY_SOCKS4A           6
#define AI_CURLPROXY_SOCKS5            7
#define AI_CURLPROXY_SOCKS5_HOSTNAME   8
#define AI_CURLPROXY_TYPE_MAX          AI_CURLPROXY_SOCKS5_HOSTNAME

#define AI_DOCUMENTLENGTH   16
#define AI_PUB_KEY_LENGTH   33
#define AI_SEC_KEY_LENGTH   32
#define AI_HASH256_LENGTH   32
#define AI_SESSION_LENGTH   64
#define AI_NET_URL_LENGTH   256
#define AI_DATA_CACHE_LEN   MAX_PATH
#define AI_FONTNAMELENGTH   32

#define AI_NETWORK_TIMEOUT  60
#define AI_FONTSIZE_DEFAULT 11

#define AI_NETWORK_IS_BAD   0
#define AI_NETWORK_IS_GOOD  1

#define AI_NETWORK_THREAD_MIN   2
#define AI_NETWORK_THREAD_MAX   8

#define AI_DEFAULT_PING_SECONDS 30
#define AI_DEFAULT_LAYOUT_PERCENT 30

#define AI_DEFAULT_EDITWIN_HEIGHT   160
#define AI_DEFAULT_TYPEWIN_HEIGHT   60

#define AI_TIMER_ASKROB     999

#define AI_NETWORK_TIMEOUT_MIN_IN_SECONDS   5
#define AI_NETWORK_TIMEOUT_MAX_IN_SECONDS   600

#define AI_LAYOUT_LEFT      0
#define AI_LAYOUT_RIGHT     1

#define ZX_MESSAGE_HEAD_SIZE       150

typedef struct TTYConfig
{
    U8* anything;
} TTYConfig;

#define AI_PROP_STARTUP     0x00000001      /* is the AI window showing at the beginning? */
#define AI_PROP_IS_LEFT     0x00000002      /* is the AI in the left side or right side */
#define AI_PROP_SSCREEN     0x00000004      /* do we need to share the screen ? */
#define AI_PROP_AUTOLOG     0x00000008      /* do we need to log all the message? */
#define AI_PROP_TOOLBAR     0x00000010      /* do we show the toolbar ? */
#define AI_PROP_STATBAR     0x00000020      /* do we show the status buar? */
#define AI_PROP_EDITWIN     0x00000040      /* do we show the edit window? */
#define AI_PROP_TYPEWIN     0x00000080      /* do we show the input window? */

#define AI_DEFAULT_PROP    (AI_PROP_STARTUP | AI_PROP_SSCREEN | AI_PROP_AUTOLOG | AI_PROP_STATBAR | AI_PROP_TYPEWIN)

typedef struct ZXConfig
{
    U64 property;
    U8  my_pubKey[AI_PUB_KEY_LENGTH];
    U8  my_secKey[AI_SEC_KEY_LENGTH];
    U8  my_kbhash[AI_HASH256_LENGTH];

    U8  documentId[AI_DOCUMENTLENGTH + 1];
    U8  sessionId[AI_SESSION_LENGTH + 1];

    U8  pubKeyHex[AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH + 1]; /* cache */
    U8  serverURL[AI_NET_URL_LENGTH + 1];
    U8  kbdataURL[AI_NET_URL_LENGTH + 1];
    U8  proxy_Str[AI_NET_URL_LENGTH + 1];
    U8  font_Name[AI_FONTNAMELENGTH + 1];
    U8  font_Size;
    U8  ping_seconds;
    U16 networkTimout;
    U8  proxy_Type;
    U8  thread_num;
    U8  layoutPercent;
    int editwHeight;   /* the height in pixel of Edit window*/
    int typewHeight;   /* the height in pixel of type window*/
    int winLeft;
    int winTop;
    int winRight;
    int winBottom;
    TTYConfig ttyConf;
} ZXConfig;

#define ZX_PARAM_AI_URL                         1
#define ZX_PARAM_AI_PROP                        2
#define ZX_PARAM_AI_FONT                        3
#define ZX_PARAM_AI_SIZE                        4
#define ZX_PARAM_AI_TIMEOUT                     5
#define ZX_PARAM_AI_PROXY_TYPE                  6
#define ZX_PARAM_AI_PROXY                       7
#define ZX_PARAM_AI_PUB_KEY                     8
#define ZX_PARAM_AI_SEC_KEY                     9
#define ZX_PARAM_AI_PWD_HASH                    10
#define ZX_PARAM_AI_KBURL                       11
#define ZX_PARAM_AI_THREADS                     12
#define ZX_PARAM_AI_PING_SECONDS                13
#define ZX_PARAM_AI_PUBKEYSTRING                14
#define ZX_PARAM_AI_EDITW_HEIGHT                15
#define ZX_PARAM_AI_TYPEW_HEIGHT                16
#define ZX_PARAM_AI_WIN_LEFT                    17
#define ZX_PARAM_AI_WIN_TOP                     18
#define ZX_PARAM_AI_WIN_RIGHT                   19
#define ZX_PARAM_AI_WIN_BOTTOM                  20
#define ZX_PARAM_AI_LAYOUTPERCENT               21

#define ZX_KBNODE_TXT       0x01
#define ZX_KBNODE_WEB       0x02
#define ZX_KBNODE_RDO       0x03

#define KB_DATA_PAYLOAD    (4 + 4 + 32)

#define ZX_KBNODE_NONELEAF  0x0100
#define ZX_KBNODE_PROCESSE  0x0200

typedef struct ZXKBTree
{
    ZXKBTree* next;
    ZXKBTree* child;
    U16 idx;
    U16 pid;
    WCHAR* title;
    U16 status;  
    U64 docId;
} ZXKBTree;

void zx_SaveAllBeforeExit();

void zx_SaveConfiguration(ZXConfig* cfNew, ZXConfig* cfOld, U8& network, U8& ui, U8& font);

U32 zx_WriteInternalLog(const char* message);

U32 zx_GetKeyFromSecretKeyAndPlubicKey(U8* sk, U8* pk, U8* key);

bool zx_ParseKBTree(U8* treedata, U32 treesize);
