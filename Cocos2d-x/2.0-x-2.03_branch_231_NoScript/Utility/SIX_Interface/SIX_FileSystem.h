/***********************************************
Name��SIX_FileSystem.h
Desc��ѹ��/��ѹ������/���ܡ��ļ���д
Auth��Cool.Cat@2013-05-03
***********************************************/
#pragma once
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <SIX_EntityMgr.h>

USING_NS_CC;

class SIX_FileSystem:public SIX_EntityMgr,public TSingleton<SIX_FileSystem>
{
public:
	virtual int ParseXML(SIX_DSTPL<SIX_XmlDataSet> *pXmlDataSet){return 0;}
	virtual int Run(){return 0;}
	virtual int Release(){return 0;}
public:
	bool CreateFolder(const char *folder);

	bool IsPathExists(const char *path);

	bool LoadDataFromFile(char *pData,const char *file);
	bool SaveDataToFile(char *pData,const char *file);

	bool ZipFile(const char *pSrcFile,char *pDstFile);
	bool UnZipFile(const char *pSrcFile,char *pDstFile);

	bool Encrypt(const char *pSrc,int slen,char *pDst,int *dlen);
	bool Decrypt(const char *pSrc,int slen,char *pDst,int *dlen);

	bool EncryptFile(const char *pSrcFile,char *pDstFile);
	bool DecryptFile(const char *pSrcFile,char *pDstFile);
};