#include "stdafx.h"
#include "ZTerm.h"
#include "sqlite3.h"
#include "secp256k1.h"
#include "secp256k1_ecdh.h"

static constexpr char* default_AI_URL  = "https://zterm.ai/v1";
static constexpr char* default_AI_FONT = "Courier New";
static constexpr char* default_AI_PWD  = "ZTerm@AI";
static constexpr char* default_KB_URL = "http://zterm.ai/kb.en";

/* a fix value to cache the start time point */
static ULONGLONG WT_TIME20000101 = 0;
/* get the time of 2000/01/01 00:00:00 */
static bool Get2000UTCTime64()
{
	bool bRet = false;
	FILETIME ft2000;
	SYSTEMTIME st2000;
	st2000.wYear = 2000;
	st2000.wMonth = 1;
	st2000.wDay = 1;
	st2000.wHour = 0;
	st2000.wMinute = 0;
	st2000.wSecond = 0;
	st2000.wMilliseconds = 0;
	bRet = (bool)SystemTimeToFileTime(&st2000, &ft2000);
	WT_TIME20000101 = ((ULONGLONG)ft2000.dwHighDateTime << 32) + ft2000.dwLowDateTime;
	assert(bRet);
	return bRet;
}

/* get the seconds since 2000/01/01 00:00:00 */
static S64 GetCurrentUTCTime64()
{
	SYSTEMTIME st;
	FILETIME ft;
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	ULONGLONG tm_now = ((ULONGLONG)ft.dwHighDateTime << 32) + ft.dwLowDateTime;
	assert(WT_TIME20000101 > 0);
	S64 tm = (S64)((tm_now - WT_TIME20000101) / 10000000); /* in second */
	return tm;
}

static void GenerateRandom32Bytes(U8* random)
{
	/* generate a 64 bytes random data as the session id from the client */
	if (wt_GenerateRandom32Bytes(random) != WT_OK)
	{
		SYSTEMTIME st;
		FILETIME ft;
		DWORD pid = GetCurrentProcessId();
		DWORD tid = GetCurrentThreadId();
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &ft);
		ULONGLONG tm_now = ((ULONGLONG)ft.dwHighDateTime << 32) + ft.dwLowDateTime;
		U8 rnd[16];

		U8* p = (U8*)(&pid);
		rnd[0] = *p++;
		rnd[1] = *p++;
		rnd[2] = *p++;
		rnd[3] = *p;
		p = (U8*)(&tid);
		rnd[4] = *p++;
		rnd[5] = *p++;
		rnd[6] = *p++;
		rnd[7] = *p;
		p = (U8*)(&tm_now);
		for (U8 k = 0; k < 8; k++) rnd[k + 8] = *p++;
		wt_sha256_hash(rnd, 16, random);
	}
}

static U32 GenerateKeyPair(U8* sec_Key, U8* pub_Key, U8* hash)
{
	U32 ret = WT_FAIL;
	secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
	if (ctx)
	{
		int rc;
		U8  randomize[AI_SEC_KEY_LENGTH];
		U8  sk[AI_SEC_KEY_LENGTH] = { 0 };
		U8 tries = 16;
		secp256k1_pubkey pubkey;

		/* Randomizing the context is recommended to protect against side-channel
		 * leakage See `secp256k1_context_randomize` in secp256k1.h for more
		 * information about it. This should never fail. */
		if (WT_OK == wt_GenerateRandom32Bytes(randomize))
		{
			rc = secp256k1_context_randomize(ctx, randomize);
			assert(rc);
		}
		else
		{
			secp256k1_context_destroy(ctx);
			return WT_RANDOM_NUMBER_ERROR;
		}
		// we try to generate a new secret key
		while (tries)
		{
			ret = wt_GenerateRandom32Bytes(sk);
			rc = secp256k1_ec_seckey_verify(ctx, sk);
			if (WT_OK == ret && 1 == rc)
				break;
			tries--;
		}

		if (0 == tries) // we cannot generate the 32 bytes secret key
		{
			secp256k1_context_destroy(ctx);
			return WT_SK_GENERATE_ERR;
		}

		rc = secp256k1_ec_pubkey_create(ctx, &pubkey, sk);
		if (1 == rc)
		{
			size_t pklen = 33;
			U8  pk[AI_PUB_KEY_LENGTH] = { 0 };
			rc = secp256k1_ec_pubkey_serialize(ctx, pk, &pklen, &pubkey, SECP256K1_EC_COMPRESSED);
			if (pklen == 33 && rc == 1)
			{
				U8 i, iv[16];

				for (i = 0; i < 16; i++) iv[i] = 17 - i;
				wt_sha256_hash((U8*)default_AI_PWD, 8, hash);
				wt_AES256_encrypt32Bytes(hash, iv, sec_Key, sk);

				for (i = 0; i < AI_PUB_KEY_LENGTH; i++) pub_Key[i] = pk[i];
				ret = WT_OK;
			}
		}
		secp256k1_context_destroy(ctx);
	}

	return ret;
}

/*
 * this is the most import point for application's security
 *
 * The sender uses his private key and the receiver's public key to
 * generate the shared key between them.
 * The recevier can use his private key and the sender's public key to
 * generate the same shared key.
 * So they can communicate with each other without transferring the
 * shared key to each other!!!
 *
 * This is the basis of the security of the application.
 */
U32 zx_GetKeyFromSecretKeyAndPlubicKey(U8* sk, U8* pk, U8* key)
{
	assert(sk);
	assert(pk);
	assert(key);

	U32 ret = WT_SECP256K1_CTX_ERROR;
	secp256k1_context* ctx;

	ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
	if (ctx)
	{
		int rc;
		secp256k1_pubkey pubkey;

		rc = secp256k1_ec_pubkey_parse(ctx, &pubkey, pk, AI_PUB_KEY_LENGTH);
		if (1 == rc)
		{
			U8 kp[AI_SEC_KEY_LENGTH] = { 0 };
			rc = secp256k1_ecdh(ctx, kp, &pubkey, sk, NULL, NULL);
			if (1 == rc)
			{
				for (int i = 0; i < AI_SEC_KEY_LENGTH; i++)
				{
					key[i] = kp[i];
					kp[i] = 0;
				}
				ret = WT_OK;
			}
		}
		secp256k1_context_destroy(ctx);
	}
	return ret;
}

void zx_InitZXConfig(ZXConfig* cf)
{
	int i;
	ATLASSERT(cf);
	cf->property = AI_DEFAULT_PROP;

	Get2000UTCTime64();
	for (i = 0; i < strlen(default_AI_URL); i++) cf->serverURL[i] = default_AI_URL[i];
	for (i = 0; i < strlen(default_KB_URL); i++) cf->kbdataURL[i] = default_KB_URL[i];
	for (i = 0; i < strlen(default_AI_FONT); i++) cf->font_Name[i] = default_AI_FONT[i];
	cf->font_Size = 11;
	cf->thread_num = AI_NETWORK_THREAD_MIN;
	cf->networkTimout = AI_NETWORK_TIMEOUT;
	cf->ping_seconds = AI_DEFAULT_PING_SECONDS;
	cf->layoutPercent = AI_DEFAULT_LAYOUT_PERCENT;
	cf->editwHeight = AI_DEFAULT_EDITWIN_HEIGHT;
	cf->typewHeight = AI_DEFAULT_TYPEWIN_HEIGHT;

}

static bool _step0() /* check if AI.DB is exsiting */
{
	bool bAvailable = false;
	WIN32_FILE_ATTRIBUTE_DATA fileInfo = { 0 };
	U8 random[32];
	ZXConfig* cf = &ZXCONFIGURATION;

	GenerateRandom32Bytes(random);
	wt_Raw2HexString(random, 32, cf->sessionId, NULL); /* generate the session ID */

	if (GetFileAttributesExW(g_dbFilePath, GetFileExInfoStandard, &fileInfo) != 0)
	{
		if (!(fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			bAvailable = true;
		}
	}
	Sleep(20);
	return bAvailable;
}

static U32 _step1(sqlite3* db, bool bAvailable)
{
	U32 ret = WT_FAIL;
	if ((db != NULL) && (!bAvailable))
	{
		int rc;
		sqlite3_stmt* stmt = NULL;
		char sql[SQL_STMT_MAX_LEN + 1] = { 0 };
		U8 sk[AI_SEC_KEY_LENGTH];
		U8 pk[AI_PUB_KEY_LENGTH];
		U8 hash[AI_HASH256_LENGTH] = { 0 };
		U8 pkHex[AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH + 1] = { 0 };

		if (GenerateKeyPair(sk, pk, hash) != WT_OK) /* generate key pair failed. It is rare */
		{
			GenerateRandom32Bytes(sk);
			GenerateRandom32Bytes(pk + 1);
			pk[0] = 0x02;
			for (U8 i = 0; i < AI_HASH256_LENGTH; i++) hash[i] = 0;
			ATLASSERT(0);
		}
		wt_Raw2HexString(pk, AI_PUB_KEY_LENGTH, pkHex, NULL);

		/* create the table at first */
		rc = sqlite3_prepare_v2(db,
			(const char*)"CREATE TABLE c(id INTEGER PRIMARY KEY,it INTEGER,tx TEXT,bo BLOB,ft REAL,nm TEXT)",
			-1, &stmt, NULL);
		if (SQLITE_OK != rc) goto _exit_LoadConf;
		rc = sqlite3_step(stmt);
		if (SQLITE_DONE != rc)
		{
			sqlite3_finalize(stmt);
			goto _exit_LoadConf;
		}
		/* table M: message table */
		rc = sqlite3_prepare_v2(db,
			(const char*)"CREATE TABLE m(id INTEGER PRIMARY KEY AUTOINCREMENT,dt INTEGER,rs CHAR(1),px CHAR(150),tx TEXT)",
			-1, &stmt, NULL);
		if (SQLITE_OK != rc) goto _exit_LoadConf;
		rc = sqlite3_step(stmt);
		if (SQLITE_DONE != rc)
		{
			sqlite3_finalize(stmt);
			goto _exit_LoadConf;
		}

		rc = sqlite3_prepare_v2(db,
			(const char*)"CREATE TABLE l(id INTEGER PRIMARY KEY AUTOINCREMENT,dt INTEGER,tx TEXT)",
			-1, &stmt, NULL);
		if (SQLITE_OK != rc) goto _exit_LoadConf;
		rc = sqlite3_step(stmt);
		if (SQLITE_DONE != rc)
		{
			sqlite3_finalize(stmt);
			goto _exit_LoadConf;
		}

		/* table N: notepad information */
		rc = sqlite3_prepare_v2(db,
			(const char*)"CREATE TABLE n(id INTEGER PRIMARY KEY AUTOINCREMENT,dt INTEGER,tx TEXT)",
			-1, &stmt, NULL);
		if (SQLITE_OK != rc) goto _exit_LoadConf;
		rc = sqlite3_step(stmt);
		if (SQLITE_DONE != rc)
		{
			sqlite3_finalize(stmt);
			goto _exit_LoadConf;
		}

		/* insert some initial data into the configuration table */
		rc = sqlite3_prepare_v2(db, (const char*)"INSERT INTO n(dt,tx) VALUES((?),'')", -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			S64 dt = GetCurrentUTCTime64();
			rc = sqlite3_bind_int64(stmt, 1, dt);
			if (SQLITE_OK == rc)
				rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,bo,nm) VALUES(%d,(?),'Private Key')", ZX_PARAM_AI_SEC_KEY);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_bind_blob(stmt, 1, sk, AI_SEC_KEY_LENGTH, SQLITE_TRANSIENT);
			if (SQLITE_OK == rc)
				rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,bo,nm) VALUES(%d,(?),'Public Key')", ZX_PARAM_AI_PUB_KEY);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_bind_blob(stmt, 1, pk, AI_PUB_KEY_LENGTH, SQLITE_TRANSIENT);
			if (SQLITE_OK == rc)
				rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,tx,nm) VALUES(%d,(?),'Public Key String')", ZX_PARAM_AI_PUBKEYSTRING);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_bind_text(stmt, 1, (const char*)pkHex, AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH, SQLITE_TRANSIENT);
			if (SQLITE_OK == rc)
				rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}
#if 0
		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,bo,nm) VALUES(%d,(?),'Password Hash')", ZX_PARAM_AI_PWD_HASH);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_bind_blob(stmt, 1, hash, AI_HASH256_LENGTH, SQLITE_TRANSIENT);
			if (SQLITE_OK == rc)
				rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}
#endif 
		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,tx,nm) VALUES(%d,'%s','Server URL')", ZX_PARAM_AI_URL, default_AI_URL);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,%d,'Property')", ZX_PARAM_AI_PROP, AI_DEFAULT_PROP);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,%d,'Layout Percent')", ZX_PARAM_AI_LAYOUTPERCENT, AI_DEFAULT_LAYOUT_PERCENT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,0,'Window Left Coordination')", ZX_PARAM_AI_WIN_LEFT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,0,'Window Top Coordination')", ZX_PARAM_AI_WIN_TOP);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,0,'Window Right Coordination')", ZX_PARAM_AI_WIN_RIGHT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,0,'Window Bottom Coordination')", ZX_PARAM_AI_WIN_BOTTOM);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,tx,nm) VALUES(%d,'%s','Font Name')", ZX_PARAM_AI_FONT, default_AI_FONT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,%d,'Font Size')", ZX_PARAM_AI_SIZE, AI_FONTSIZE_DEFAULT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,%d,'Network Timeout In Seconds')", ZX_PARAM_AI_TIMEOUT, AI_NETWORK_TIMEOUT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK != rc) goto _exit_LoadConf;
		rc = sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,%d,'Network Thread Number')", ZX_PARAM_AI_THREADS, AI_NETWORK_THREAD_MIN);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,%d,'Ping Interval In Seconds')", ZX_PARAM_AI_PING_SECONDS, AI_DEFAULT_PING_SECONDS);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,%d,'Edit Window Height in Pixel')", ZX_PARAM_AI_EDITW_HEIGHT, AI_DEFAULT_EDITWIN_HEIGHT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,%d,'Type Window Height in Pixel')", ZX_PARAM_AI_TYPEW_HEIGHT, AI_DEFAULT_TYPEWIN_HEIGHT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,it,nm) VALUES(%d,%d,'Proxy Type')", ZX_PARAM_AI_PROXY_TYPE, AI_CURLPROXY_NO_PROXY);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,tx,nm) VALUES(%d,'','Proxy String')", ZX_PARAM_AI_PROXY);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN,
			"INSERT INTO c(id,tx,nm) VALUES(%d,(?),'Knowledge Base URL')", ZX_PARAM_AI_KBURL);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			U32 utf8len = strlen((const char*)default_KB_URL);
			rc = sqlite3_bind_text(stmt, 1, (const char*)default_KB_URL, utf8len, SQLITE_TRANSIENT);
			if (SQLITE_OK == rc)
				rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}
		ret = WT_OK;
	}
	else
	{
		Sleep(20);
	}
_exit_LoadConf:
	return ret;
}

static U32 _step2(sqlite3* db) /* read configuration from AI.DB */
{
	U32 ret = WT_FAIL;
	if (db != NULL)
	{
		int rc;
		sqlite3_stmt* stmt = NULL;
		char sql[SQL_STMT_MAX_LEN + 1] = { 0 };
		U8* utf8Txt;
		U8* blobBuf;
		U32 utf8Len, blobLen;
		ZXConfig* cf = &ZXCONFIGURATION;

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT bo FROM c WHERE id=%d", ZX_PARAM_AI_SEC_KEY);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				blobBuf = (U8*)sqlite3_column_blob(stmt, 0);
				blobLen = (U32)sqlite3_column_bytes(stmt, 0);
				if (blobLen == AI_SEC_KEY_LENGTH)
				{
					U8 hash[AI_HASH256_LENGTH];
					U8 iv[16];
					for (U8 i = 0; i < 16; i++) iv[i] = 17 - i;
					wt_sha256_hash((U8*)default_AI_PWD, 8, hash);
					wt_AES256_decrypt32Bytes(hash, iv, blobBuf, cf->my_secKey);
				}
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT bo FROM c WHERE id=%d", ZX_PARAM_AI_PUB_KEY);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				blobBuf = (U8*)sqlite3_column_blob(stmt, 0);
				blobLen = (U32)sqlite3_column_bytes(stmt, 0);
				if (blobLen == AI_PUB_KEY_LENGTH)
				{
					memcpy_s(cf->my_pubKey, AI_PUB_KEY_LENGTH, blobBuf, AI_PUB_KEY_LENGTH);
				}
			}
			sqlite3_finalize(stmt);
		}

		wt_Raw2HexString(cf->my_pubKey, AI_PUB_KEY_LENGTH, cf->pubKeyHex, NULL);
		cf->pubKeyHex[AI_PUB_KEY_LENGTH + AI_PUB_KEY_LENGTH] = '\0';
#if 0
		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT bo FROM c WHERE id=%d", ZX_PARAM_AI_PWD_HASH);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				blobBuf = (U8*)sqlite3_column_blob(stmt, 0);
				blobLen = (U32)sqlite3_column_bytes(stmt, 0);
				if (blobLen == AI_HASH256_LENGTH)
				{
					memcpy_s(cf->my_sha256, AI_HASH256_LENGTH, blobBuf, AI_HASH256_LENGTH);
				}
			}
			sqlite3_finalize(stmt);
		}
#endif 
		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT tx FROM c WHERE id=%d", ZX_PARAM_AI_KBURL);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				utf8Txt = (U8*)sqlite3_column_text(stmt, 0);
				if (utf8Txt)
				{
					utf8Len = (U32)strlen((const char*)utf8Txt);
					if (utf8Len < AI_NET_URL_LENGTH)
					{
						memcpy_s(cf->kbdataURL, AI_NET_URL_LENGTH, utf8Txt, utf8Len);
						cf->kbdataURL[utf8Len] = '\0';
					}
				}
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT tx FROM c WHERE id=%d", ZX_PARAM_AI_URL);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				utf8Txt = (U8*)sqlite3_column_text(stmt, 0);
				if (utf8Txt)
				{
					utf8Len = (U32)strlen((const char*)utf8Txt);
					if (utf8Len < AI_NET_URL_LENGTH)
					{
						memcpy_s(cf->serverURL, AI_NET_URL_LENGTH, utf8Txt, utf8Len);
						cf->serverURL[utf8Len] = '\0';
					}
				}
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_PROP);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				cf->property = (U32)sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_LAYOUTPERCENT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				cf->layoutPercent = sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
		}
		if (cf->layoutPercent > 90)
			cf->layoutPercent = 90;

		if (cf->layoutPercent < 10)
			cf->layoutPercent = 10;

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_WIN_LEFT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				cf->winLeft = sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_WIN_TOP);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				cf->winTop = sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_WIN_RIGHT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				cf->winRight = sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_WIN_BOTTOM);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				cf->winBottom = sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT tx FROM c WHERE id=%d", ZX_PARAM_AI_FONT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				utf8Txt = (U8*)sqlite3_column_text(stmt, 0);
				if (utf8Txt)
				{
					utf8Len = (U32)strlen((const char*)utf8Txt);
					if (utf8Len < 32)
					{
						memcpy_s(cf->font_Name, 32, utf8Txt, utf8Len);
						cf->font_Name[utf8Len] = '\0';
					}
				}
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_SIZE);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				cf->font_Size = (U8)sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_TIMEOUT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				cf->networkTimout = (U16)sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_EDITW_HEIGHT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				cf->editwHeight = (U16)sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_TYPEW_HEIGHT);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				cf->typewHeight = (U16)sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT it FROM c WHERE id=%d", ZX_PARAM_AI_PROXY_TYPE);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				U8 pxtype = (U8)sqlite3_column_int(stmt, 0);
				if (pxtype >= AI_CURLPROXY_NO_PROXY && pxtype <= AI_CURLPROXY_TYPE_MAX)
					cf->proxy_Type = pxtype;
			}
			sqlite3_finalize(stmt);
		}

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT tx FROM c WHERE id=%d", ZX_PARAM_AI_PROXY);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				utf8Txt = (U8*)sqlite3_column_text(stmt, 0);
				if (utf8Txt)
				{
					utf8Len = (U32)strlen((const char*)utf8Txt);
					if (utf8Len && utf8Len <= AI_NET_URL_LENGTH)
					{
						memcpy_s(cf->proxy_Str, AI_NET_URL_LENGTH, utf8Txt, utf8Len);
						cf->proxy_Str[utf8Len] = '\0';
					}
				}
			}
			sqlite3_finalize(stmt);
		}
		ret = WT_OK;
	}
	else
	{
		Sleep(20);
	}
	return ret;
}

static U32 _step3(sqlite3* db,HWND hWndUI)
{
	U32 ret = WT_FAIL;
	if (db != NULL)
	{
		int rc;
		sqlite3_stmt* stmt = NULL;
		char sql[SQL_STMT_MAX_LEN + 1] = { 0 };
		U8* utf8Txt;
		U32 utf8Len;

		sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "SELECT tx FROM n WHERE id=1");
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			if (SQLITE_ROW == rc)
			{
				utf8Txt = (U8*)sqlite3_column_text(stmt, 0);
				if (utf8Txt)
				{
					utf8Len = (U32)strlen((const char*)utf8Txt);
					if (g_editMax < utf8Len)
					{
						g_editMax = WT_ALIGN_PAGE(utf8Len);
						if (nullptr != g_editBuf)
						{
							VirtualFree(g_editBuf, 0, MEM_RELEASE);
							g_editBuf = nullptr;
						}
						g_editBuf = (U8*)VirtualAlloc(NULL, g_editMax, MEM_COMMIT, PAGE_READWRITE);
					}
					if (g_editBuf && utf8Len)
					{
						ATLASSERT(g_editMax >= utf8Len);
						memcpy_s(g_editBuf, g_editMax, utf8Txt, utf8Len);
						g_editBuf[utf8Len] = '\0';
						g_editLen = utf8Len;
						PostMessage(hWndUI, WM_LOADNOTEPADMSG, (WPARAM)g_editLen, (LPARAM)g_editBuf);
					}
				}
			}
			sqlite3_finalize(stmt);
		}
		ret = WT_OK;
	}
	else
	{
		Sleep(20);
	}

	return ret;
}

#define KB_HASHTAB_SIZE		(1<<16)
bool zx_ParseKBTree(U8* treedata, U32 treesize)
{
	bool bRet = false;
	U32 bytes = sizeof(ZXKBTree*) * KB_HASHTAB_SIZE;
	ZXKBTree** kbTab = (ZXKBTree**)VirtualAlloc(NULL, bytes, MEM_COMMIT, PAGE_READWRITE);

	ATLASSERT(g_kbMemPool);
	g_rootKBNode = nullptr;
	wt_mempool_reset(g_kbMemPool);

	if (kbTab)
	{
		int i;
		ZXKBTree* itemParent; // parent
		ZXKBTree* item; // child
		U32 status, utf16len, utf8len;
		U8 flag;
		U16 idx, pid;
		U16* p16;
		U64* p64;
		U8 *treeEnd, *p;

		ZeroMemory(kbTab, bytes);
		treeEnd = treedata + treesize; // point to the end of the buffer
		p = treedata; // point to the start of the buffer

		/* first scan to establish the hash array */
		while (treeEnd - p > 6)
		{
			flag = p[0];
			utf8len = p[1];
			if (flag & 0x80) // none-leaf. The highest bit is used to indicate leaf or none-leaf
			{
				p16 = (U16*)(p + 2);
				idx = *p16;
				p16 = (U16*)(p + 4);
				pid = *p16;
				if (treeEnd - p >= (6 + utf8len))
				{
					utf16len = 0;
					status = wt_UTF8ToUTF16(p + 6, utf8len, NULL, &utf16len);
					if (status == WT_OK && utf16len)
					{
						if (kbTab[idx] == nullptr)
						{
							kbTab[idx] = (ZXKBTree*)wt_palloc0(g_kbMemPool, sizeof(ZXKBTree));
						}
						item = kbTab[idx];
						if (item)
						{
							if (item->title == nullptr)
							{
								item->title = (WCHAR*)wt_palloc0(g_kbMemPool, sizeof(WCHAR) * (utf16len + 1));
								if (item->title)
								{
									status = wt_UTF8ToUTF16(p + 6, utf8len, (U16*)item->title, NULL);
									ATLASSERT(status == WT_OK);
									item->idx = idx;
									item->pid = pid;
									item->status |= ZX_KBNODE_NONELEAF;
								}
							}
						}
					}
					p += (6 + utf8len);
					continue;
				}
				else goto _exit_parsing; // we have some error, exit the paring process
			}
			break;
		}
		/* second scan to establish the parent-child relationship */
		for (i = 0; i < KB_HASHTAB_SIZE; i++)
		{
			item = kbTab[KB_HASHTAB_SIZE - 1 - i];
			if (item)
			{
				if (item->title)
				{
					if (item->title[0])
					{
						itemParent = kbTab[item->pid];
						if (itemParent)
						{
							item->next = itemParent->child;
							itemParent->child = item;
						}
					}
				}
			}
		}

		p = treedata; // point to the start of the buffer
		while (treeEnd - p > 6)
		{
			flag = p[0];
			utf8len = p[1];
			if (flag & 0x80) // none-leaf. The highest bit is used to indicate leaf or none-leaf
			{
				if (treeEnd - p >= (6 + utf8len))
				{
					p += (6 + utf8len);
				}
				else goto _exit_parsing; // we have some error, exit the paring process
			}
			else
			{
				p16 = (U16*)(p + 2);
				pid = *p16;
				if (treeEnd - p >= (2 + 2 + 8 + utf8len))
				{
					utf16len = 0;
					status = wt_UTF8ToUTF16(p + 2 + 2 + 8, utf8len, NULL, &utf16len);
					if (status == WT_OK && utf16len)
					{
						itemParent = kbTab[pid];
						if (itemParent)
						{
							item = (ZXKBTree*)wt_palloc0(g_kbMemPool, sizeof(ZXKBTree));
							if (item)
							{
								item->title = (WCHAR*)wt_palloc0(g_kbMemPool, sizeof(WCHAR) * (utf16len + 1));
								if (item->title)
								{
									status = wt_UTF8ToUTF16(p + 2 + 2 + 8, utf8len, (U16*)item->title, NULL);
									ATLASSERT(status == WT_OK);
									p64 = (U64*)(p + 4);
									item->docId = *p64;
									item->status = flag;
									item->next = itemParent->child;
									itemParent->child = item;
								}
							}
						}
					}
					p += (2 + 2 + 8 + utf8len);
				}
				else goto _exit_parsing; // we have some error, exit the paring process
			}
		}
		bRet = true;
	}

_exit_parsing: 
	if (kbTab)
	{
		if (bRet)
		{
			g_rootKBNode = kbTab[1];
		}
		VirtualFree(kbTab, 0, MEM_RELEASE);
		kbTab = nullptr;
	}
	return bRet;
}

static U32 _step4(sqlite3* db)
{
	ZXConfig* cf = &ZXCONFIGURATION;
	U32 status, utf16len, bytes, ret = WT_FAIL;
	U8* bindata = nullptr;
	U8* unzipbuf = nullptr;
	U8* kburl = nullptr;

	bytes = 0;
	if (strlen((const char*)cf->kbdataURL))
		kburl = cf->kbdataURL;
	else
		kburl = (U8*)default_KB_URL;

	bindata = zx_GetFileByHTTP((const U8*)kburl, bytes);

	if (bindata && bytes > KB_DATA_PAYLOAD)
	{
		uLongf  zipSize, unzipSize;
		U32* p32 = (U32*)bindata;
		zipSize = *p32;
		if (bytes == (U32)zipSize)
		{
			p32 = (U32*)(bindata + 4);
			unzipSize = *p32;
			unzipbuf = (U8*)malloc(unzipSize);
			if (unzipbuf)
			{
				uLongf destLen = unzipSize;
				uLongf sourceLen = zipSize - KB_DATA_PAYLOAD;
				int rc = uncompress2(unzipbuf, &unzipSize, bindata + KB_DATA_PAYLOAD, &sourceLen);
				if (rc == Z_OK && destLen == unzipSize)
				{
					U8 hash[AI_HASH256_LENGTH];
					wt_sha256_hash(unzipbuf, unzipSize, hash);
					if (memcmp(hash, bindata + 8, AI_HASH256_LENGTH) == 0) // the data looks good 
					{
						ZXConfig* cf = &ZXCONFIGURATION;
						for (U8 i = 0; i < AI_HASH256_LENGTH; i++) cf->my_kbhash[i] = hash[i];
						if (zx_ParseKBTree(unzipbuf, unzipSize))
							ret = WT_OK;
					}
				}
			}
		}
	}

	if (bindata)
	{
		free(bindata);
		bindata = nullptr;
	}
	if (unzipbuf)
	{
		free(unzipbuf);
		unzipbuf = nullptr;
	}
	return ret;
}

DWORD WINAPI LoadingDataThread(LPVOID lpData)
{
	HWND hWndUI = (HWND)(lpData);
	ATLASSERT(::IsWindow(hWndUI));
	U8 count, percentage;
	U32 status = WT_OK;
	int rc;
	sqlite3* db = NULL;
	bool bAvailable;
	InterlockedIncrement(&g_threadCount);

	bAvailable = _step0(); /* check if the AI.DB is available */

	rc = sqlite3_open16(g_dbFilePath, &db);

	percentage = 0;
	count = 0;
	while (0 == g_Quit)
	{
		count++;
		switch (count)
		{
		case 1:
			status = _step1(db, bAvailable);
			break;
		case 2:
			status = _step2(db);
			break;
		case 3:
			status = _step3(db, hWndUI);
			break;
		case 4:
			status = _step4(db);
			break;
		default:
			if (db != NULL)
			{
				sqlite3_close(db);
				db = NULL;
			}
			Sleep(20);
			break;
		}
		PostMessage(hWndUI, WM_LOADPERCENTMSG, (WPARAM)percentage, (LPARAM)count);
		percentage += 4;
		if (percentage >= 108)
		{
			break;
		}
	}

	ATLASSERT(db == NULL);
	InterlockedDecrement(&g_threadCount);
	return 0;
}

void zx_SaveAllBeforeExit()
{
	sqlite3* db = NULL;
	int rc = sqlite3_open16(g_dbFilePath, &db);
	if (SQLITE_OK == rc)
	{
		ZXConfig* cf = &ZXCONFIGURATION;
		sqlite3_stmt* stmt = NULL;
		char sql[64 + 1] = { 0 };

		sprintf_s((char*)sql, 64, "UPDATE c SET it=%llu WHERE id=%d", cf->property, ZX_PARAM_AI_PROP);
		rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		if (g_editBuf && g_editLen)
		{
			rc = sqlite3_prepare_v2(db, (const char*)"UPDATE n SET dt=(?),tx=(?) WHERE id=1", -1, &stmt, NULL);
			if (SQLITE_OK == rc)
			{
				S64 dt = GetCurrentUTCTime64();
				rc = sqlite3_bind_int64(stmt, 1, dt);
				rc = sqlite3_bind_text(stmt, 2, (const char*)g_editBuf, g_editLen, SQLITE_TRANSIENT);
				if (SQLITE_OK == rc)
				{
					rc = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
			}
		}
		sqlite3_close(db);
	}
}

void zx_SaveConfiguration(ZXConfig* cfNew, ZXConfig* cfOld, U8& network, U8& ui, U8& font)
{
	int rc;
	sqlite3* db;

	ATLASSERT(cfNew);
	ATLASSERT(cfOld);

	network = 0;
	ui = 0;
	font = 0;

	rc = sqlite3_open16(g_dbFilePath, &db);
	if (SQLITE_OK == rc)
	{
		bool changed;
		size_t len0, len1;
		sqlite3_stmt* stmt = NULL;
		char sql[SQL_STMT_MAX_LEN + 1] = { 0 };

		if ((cfNew->property & AI_PROP_IS_LEFT) != (cfOld->property & AI_PROP_IS_LEFT))
			ui++;

		if (cfNew->property != cfOld->property)
		{
			sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "UPDATE c SET it=%llu WHERE id=%u", cfNew->property, ZX_PARAM_AI_PROP);
			rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
			rc = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		changed = false;
		len0 = strlen((const char*)cfNew->serverURL);
		len1 = strlen((const char*)cfOld->serverURL);
		if (len0 != len1)
		{
			network++;
			changed = true;
		}
		else if (len0 > 0 && memcmp(cfNew->serverURL, cfOld->serverURL, len0))
		{
			changed = true;
			network++;
		}

		if (changed)
		{
			sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "UPDATE c SET tx=(?) WHERE id=%u", ZX_PARAM_AI_URL);
			rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
			if (SQLITE_OK == rc)
			{
				rc = sqlite3_bind_text(stmt, 1, (const char*)cfNew->serverURL, len0, SQLITE_TRANSIENT);
				if (SQLITE_OK == rc)
					rc = sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}

		changed = false;
		len0 = strlen((const char*)cfNew->kbdataURL);
		len1 = strlen((const char*)cfOld->kbdataURL);
		if (len0 != len1)
			changed = true;
		else if (len0 > 0 && memcmp(cfNew->kbdataURL, cfOld->kbdataURL, len0))
			changed = true;

		if (changed)
		{
			sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "UPDATE c SET tx=(?) WHERE id=%u", ZX_PARAM_AI_KBURL);
			rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
			if (SQLITE_OK == rc)
			{
				rc = sqlite3_bind_text(stmt, 1, (const char*)cfNew->kbdataURL, len0, SQLITE_TRANSIENT);
				if (SQLITE_OK == rc)
					rc = sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}

		if (cfNew->proxy_Type > AI_CURLPROXY_TYPE_MAX)
			cfNew->proxy_Type = AI_CURLPROXY_NO_PROXY;

		if (cfNew->proxy_Type != cfOld->proxy_Type)
		{
			network++;
			sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "UPDATE c SET it=%u WHERE id=%u",
				cfNew->proxy_Type,
				ZX_PARAM_AI_PROXY_TYPE);
			rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
			if (SQLITE_OK == rc)
			{
				rc = sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}

		if (cfNew->proxy_Type != AI_CURLPROXY_NO_PROXY)
		{
			changed = false;
			len0 = strlen((const char*)cfNew->proxy_Str);
			len1 = strlen((const char*)cfOld->proxy_Str);
			if (len0 != len1)
			{
				changed = true;
				network++;
			}
			else if (len0 > 0 && memcmp(cfNew->proxy_Str, cfOld->proxy_Str, len0))
			{
				changed = true;
				network++;
			}
			if (changed)
			{
				sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "UPDATE c SET tx=(?) WHERE id=%u", ZX_PARAM_AI_PROXY);
				rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
				if (SQLITE_OK == rc)
				{
					rc = sqlite3_bind_text(stmt, 1, (const char*)cfNew->proxy_Str, len0, SQLITE_TRANSIENT);
					if (SQLITE_OK == rc)
						rc = sqlite3_step(stmt);
					sqlite3_finalize(stmt);
				}
			}
		}

		changed = false;
		len0 = strlen((const char*)cfNew->font_Name);
		len1 = strlen((const char*)cfOld->font_Name);
		if (len0 != len1)
		{
			font++;
			changed = true;
		}
		else if (len0 > 0 && memcmp(cfNew->font_Name, cfOld->font_Name, len0))
		{
			font++;
			changed = true;
		}

		if (changed)
		{
			sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "UPDATE c SET tx=(?) WHERE id=%u", ZX_PARAM_AI_FONT);
			rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
			if (SQLITE_OK == rc)
			{
				rc = sqlite3_bind_text(stmt, 1, (const char*)cfNew->font_Name, len0, SQLITE_TRANSIENT);
				if (SQLITE_OK == rc)
					rc = sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}

		if (cfNew->font_Size != cfOld->font_Size)
		{
			font++;
			sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "UPDATE c SET it=%u WHERE id=%u", cfNew->font_Size, ZX_PARAM_AI_SIZE);
			rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
			if (SQLITE_OK == rc)
			{
				rc = sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}

		if (cfNew->networkTimout != cfOld->networkTimout)
		{
			network++;
			sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "UPDATE c SET it=%u WHERE id=%u", cfNew->networkTimout, ZX_PARAM_AI_TIMEOUT);
			rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
			if (SQLITE_OK == rc)
			{
				rc = sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}
		if (cfNew->thread_num != cfOld->thread_num)
		{
			network++;
			sprintf_s((char*)sql, SQL_STMT_MAX_LEN, "UPDATE c SET it=%u WHERE id=%u", cfNew->thread_num, ZX_PARAM_AI_THREADS);
			rc = sqlite3_prepare_v2(db, (const char*)sql, -1, &stmt, NULL);
			if (SQLITE_OK == rc)
			{
				rc = sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}
		sqlite3_close(db);
	}
}

U32 zx_WriteInternalLog(const char* message)
{
	if (message)
	{
		U32 length = strlen(message);
		if (length)
		{
			sqlite3* db = NULL;
			int rc = sqlite3_open16(g_dbFilePath, &db);
			if (SQLITE_OK == rc)
			{
				sqlite3_stmt* stmt = NULL;
				rc = sqlite3_prepare_v2(db, (const char*)"INSERT INTO l(dt,tx) VALUES((?),(?))", -1, &stmt, NULL);
				if (SQLITE_OK == rc)
				{
					U8 bind = 0;
					S64 dt = GetCurrentUTCTime64();
					rc = sqlite3_bind_int64(stmt, 1, dt);
					if (SQLITE_OK != rc) bind++;
					rc = sqlite3_bind_text(stmt, 2, message, length, SQLITE_TRANSIENT);
					if (SQLITE_OK != rc) bind++;
					if (0 == bind)
					{
						rc = sqlite3_step(stmt);
					}
					sqlite3_finalize(stmt);
				}
				sqlite3_close(db);
			}
		}
	}
	return WT_OK;
}

void zx_SaveDocumentInformation(U8* docId, U8* docTitle)
{
	sqlite3* db = NULL;
	int rc = sqlite3_open16(g_dbFilePath, &db);
	if (SQLITE_OK == rc)
	{
		bool bAvailable = false;
		S64 dt;
		sqlite3_stmt* stmt = NULL;

		rc = sqlite3_prepare_v2(db, (const char*)"SELECT count(1) FROM t WHERE di=(?)", -1, &stmt, NULL);
		if (SQLITE_OK == rc)
		{
			rc = sqlite3_bind_text(stmt, 1, (const char*)docId, AI_DOCUMENTLENGTH, SQLITE_TRANSIENT);
			if (SQLITE_OK == rc)
			{
				rc = sqlite3_step(stmt);
				if (rc == SQLITE_ROW)
				{
					int count = sqlite3_column_int(stmt, 0);
					if (count > 0)
						bAvailable = true;
				}
			}
			sqlite3_finalize(stmt);
		}

		dt = GetCurrentUTCTime64();
		if (!bAvailable)
		{
			rc = sqlite3_prepare_v2(db, (const char*)"INSERT INTO t(ip,dt,tx,di)VALUES(1,(?),(?),(?))", -1, &stmt, NULL);
			if (SQLITE_OK == rc)
			{
				rc = sqlite3_bind_int64(stmt, 1, dt);
				rc = sqlite3_bind_text(stmt, 2, (const char*)docTitle, strlen((const char*)docTitle), SQLITE_TRANSIENT);
				rc = sqlite3_bind_text(stmt, 3, (const char*)docId, AI_DOCUMENTLENGTH, SQLITE_TRANSIENT);
				rc = sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}
		else
		{
			rc = sqlite3_prepare_v2(db, (const char*)"UPDATE t SET dt=(?),tx=(?) WHERE di=(?)", -1, &stmt, NULL);
			if (SQLITE_OK == rc)
			{
				rc = sqlite3_bind_int64(stmt, 1, dt);
				rc = sqlite3_bind_text(stmt, 2, (const char*)docTitle, strlen((const char*)docTitle), SQLITE_TRANSIENT);
				rc = sqlite3_bind_text(stmt, 3, (const char*)docId, AI_DOCUMENTLENGTH, SQLITE_TRANSIENT);
				rc = sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}
		sqlite3_close(db);
	}
}

U32 zx_Write_LogMessage(const char* message, U32 length, U8 rs)
{
	U32 ret = WT_FAIL;
	if (message && length && length > ZX_MESSAGE_HEAD_SIZE)
	{
		sqlite3* db;
		int rc = sqlite3_open16(g_dbFilePath, &db);
		if (SQLITE_OK == rc)
		{
			sqlite3_stmt* stmt = NULL;
			if (rs == 'S')
			{
				rc = sqlite3_prepare_v2(db, 
					(const char*)"INSERT INTO m(dt,px,tx,rs) VALUES((?),(?),(?),'S')", -1, &stmt, NULL);
			}
			else
			{
				ATLASSERT(rs == 'R');
				rc = sqlite3_prepare_v2(db,
					(const char*)"INSERT INTO m(dt,px,tx,rs) VALUES((?),(?),(?),'R')", -1, &stmt, NULL);
			}
			if (SQLITE_OK == rc)
			{
				U8 i, bind = 0;
				U8 px[ZX_MESSAGE_HEAD_SIZE + 1] = { 0 };
				for (i = 0; i < ZX_MESSAGE_HEAD_SIZE; i++)	px[i] = message[i];

				S64 dt = GetCurrentUTCTime64();
				rc = sqlite3_bind_int64(stmt, 1, dt);
				if (SQLITE_OK != rc) bind++;
				rc = sqlite3_bind_text(stmt, 2, (const char*)px, ZX_MESSAGE_HEAD_SIZE, SQLITE_TRANSIENT);
				if (SQLITE_OK != rc) bind++;
				rc = sqlite3_bind_text(stmt, 3, message + ZX_MESSAGE_HEAD_SIZE, length - ZX_MESSAGE_HEAD_SIZE, SQLITE_TRANSIENT);
				if (SQLITE_OK != rc) bind++;
				if (0 == bind)
				{
					rc = sqlite3_step(stmt);
					if (SQLITE_DONE == rc)
					{
						ret = WT_OK;
					}
				}
				sqlite3_finalize(stmt);
			}
			sqlite3_close(db);
		}
	}
	return ret;
}
