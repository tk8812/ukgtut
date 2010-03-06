#define __GGFFILE_C__

#if _MSC_VER > 1000
//#pragma once
#define _CRT_SECURE_NO_DEPRECATE 
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif

#include "ggf_def.h"

#ifdef GGF_WIN32
#include <stdlib.h>
#include "string.h"
#include "windows.h"

#endif



#ifdef GGF_WIPIC

#include "WIPIHeader.h"
#include "MCGame.h"
#include "MHhandset.h"

#endif


#include "ggf_fileSystem.h"
//#include "GGF_sdk.h"
//#include "GGF_file.h"




typedef struct _S_GGFFileInfo {
	int m_nIsActive;
	void *m_hHandle;
	DWORD m_nAttr;
	DWORD m_nSize;
} S_GGFFileInfo;

S_GGFFileInfo g_FileInfo[GGF_MAXFILE];



void GGFFile_Init()
{
	int i;
	for(i=0;i<GGF_MAXFILE;i++)
	{
		memset(&(g_FileInfo[i]),0,sizeof(S_GGFFileInfo));
		//g_FileInfo[i].m_hHandle = -1;

	}

}



DWORD GGFFile_Open(const char *szFile,DWORD nMode)
{
	int nHandle;
	int i;

	for(i=0;i<GGF_MAXFILE;i++)
	{	
		if(g_FileInfo[i].m_nIsActive == 0)
		{
			nHandle = i;
			break;
		}
	}

	if(i >= GGF_MAXFILE) return GGF_MAXFILE;

#ifdef GGF_WIN32
	{
		HANDLE hFile = 0;

		if(nMode == GGFFMODE_READ)
		{
			if(	(hFile = CreateFile(szFile,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL)) != INVALID_HANDLE_VALUE) 
			{				
				g_FileInfo[nHandle].m_nSize = GetFileSize (hFile, NULL);
				g_FileInfo[nHandle].m_nAttr = 0;
				g_FileInfo[nHandle].m_hHandle = hFile;
				g_FileInfo[nHandle].m_nIsActive = 1;
			}
			else
				return GGF_MAXFILE;
		}
		else if(nMode == GGFFMODE_WRITE)
		{
			if(	(hFile = CreateFile(szFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL)) != INVALID_HANDLE_VALUE) 
			{
				g_FileInfo[nHandle].m_nSize = 0;
				g_FileInfo[nHandle].m_nAttr = 0;
				g_FileInfo[nHandle].m_hHandle = hFile;    
				g_FileInfo[nHandle].m_nIsActive = 1;

			}
		}
		else
		{
			return GGF_MAXFILE;
		}

	}
#endif

#ifdef GGF_WIPIC
	{		
		M_Int32 file;
		int ret;
		MC_FileInfo fi;

		ret = MC_fsFileAttribute( szFile, &fi, MC_DIR_PRIVATE_ACCESS );

		if( ret == M_E_ERROR )
			return -1;
		/*
		if( fi.size > MYXMLPHASER_DATA_MAXLEN )
			return -1;
			*/


		file = MC_fsOpen( szFile, MC_FILE_OPEN_RDONLY, MC_DIR_PRIVATE_ACCESS );	
		if( file < 0 )
			return -1;

		g_FileInfo[nHandle].m_nSize = fi.size;
		g_FileInfo[nHandle].m_nAttr = 0;
		g_FileInfo[nHandle].m_nHandle = file;
	}
#endif

	//nHandle = g_nFileCount;
	//g_nFileCount++;

	return nHandle;
}

DWORD GGFFile_GetFileSize(DWORD nHandle)
{

	return g_FileInfo[nHandle].m_nSize;
}


DWORD GGFFile_Close(DWORD nHandle)
{
#ifdef GGF_WIPIC
	{		
		MC_fsClose( g_FileInfo[nHandle].m_nHandle );
		g_FileInfo[nHandle].m_nHandle = -1;
	}
#endif

#ifdef GGF_WIN32

	{
		CloseHandle(g_FileInfo[nHandle].m_hHandle);

		memset(&(g_FileInfo[nHandle]),0,sizeof(S_GGFFileInfo));
		//g_FileInfo[nHandle].m_hHandle = 0;
		//g_FileInfo[nHandle].m_nIsActive = 0;
	}

#endif

	return 0;
}

/*
성공이면 0 아니면 다른값
*/
DWORD GGFFile_Read(DWORD nHandle,BYTE *pBuf,DWORD nLen)
{	
	int error = 0;

#ifdef GGF_WIN32

	{
		ReadFile(g_FileInfo[nHandle].m_hHandle,
			pBuf,
			nLen,
			&error, //읽어들인크기
			NULL);	

		if(error == 0)
			return 1;

	}

#endif

#ifdef GGF_WIPIC
	{				
		int nRead = 0;
		int ret;
		int cnt = 0;

		while( nRead < nLen )
		{
			ret = MC_fsRead( g_FileInfo[nHandle].m_nHandle, pBuf + nRead, nLen - nRead );

			if( ret < 0 && cnt <= 100)
			{
				cnt++;
				continue;
			}

			if( cnt > 100 )
			{
				error = -10000;
				break;
			}

			nRead += ret;
			cnt++;
		}
	}	
#endif

	return 0;
	//return error;
	
}

DWORD GGFFile_Write(DWORD hHandle,BYTE *pBuf,DWORD nLen)
{
	

#ifdef GGF_WIN32

	{	
		DWORD nResult;
		nResult = WriteFile(g_FileInfo[hHandle].m_hHandle,pBuf,nLen,&nResult,NULL);	

		if(nResult ==0)
			return 1;
	}

#endif

#ifdef GGF_WIPIC
	{	
		
		//MC_fsWrite( g_FileInfo[nHandle].m_nHandle, pBuf, nLen);
		M_Int32 hFile;
		unsigned char *p;
		int len;
		
		int error = 0;
		int nWrite = 0;
		int ret;
		int cnt = 0;

		hFile = g_FileInfo[nHandle].m_nHandle;
		len = nLen;
		p = pBuf;

		while( nWrite < len )
		{
			ret = MC_fsWrite( hFile, p + nWrite, len - nWrite );

			if( ret < 0 && cnt <= 100)
			{
				cnt++;
				continue;
			}

			if( cnt > 100 )
			{
				error = -10000;
				break;
			}

			nWrite += ret;
			cnt++;
		}

	}
#endif

	return 0;
}

/*
파일처음위치를기준으로 파일포인터를옮긴다.
*/
DWORD GGFFile_SeekSet(DWORD hHandle,int nPos)
{


#ifdef GGF_WIN32

	{		
		SetFilePointer (g_FileInfo[hHandle].m_hHandle, nPos, NULL, FILE_BEGIN); 
	}

#endif

#ifdef GGF_WIPIC
	{		
		MC_fsSeek(g_FileInfo[nHandle].m_nHandle,nPos,MC_FILE_SEEK_SET);
	}
#endif
	return 0;
}

/*
파일처음현제위치를기준으로 파일포인터를옮긴다.
*/
DWORD GGFFile_SeekCur(DWORD hHandle,int nPos)
{

#ifdef GGF_WIN32

	{		
		SetFilePointer (g_FileInfo[hHandle].m_hHandle, nPos, NULL, FILE_CURRENT); 
	}

#endif

#ifdef GGF_WIPIC
	{		
		MC_fsSeek(g_FileInfo[nHandle].m_nHandle,nPos,MC_FILE_SEEK_CUR);
	}
#endif
	return 0;
}



void *GGFFile_GetSystemHandle(DWORD hHandle)
{
	return g_FileInfo[hHandle].m_hHandle;
}

/*
0 이면 no error

아니면 에러

*/
int GGFFile_GetError(DWORD hHandle)
{
	if(hHandle >= GGFFERROR_OPEN)
		return 1;

	return 0;
}





/**********************************************
패키지 파일 시스템 모듈
*///////////////////////////////////////////////

S_GGFFS_OBJ g_FileSystem;

/*
szFn파일에대한데이타저장된부분의 파일포인터로 변경된 파일핸들을반환함
*/
DWORD GGFFS_Open(const char *szPackFn)
{
	DWORD nHandle;

	nHandle = GGFFile_Open(szPackFn,GGFFMODE_READ);

	GGFFile_Read(nHandle,(unsigned char *)(&g_FileSystem.m_FS.m_Header),sizeof(S_GGFFS_HEADER_Rec));

	if(!strcmp(g_FileSystem.m_FS.m_Header.m_Data.m_szId,"GPQ"))
	{		
		DWORD nNameTableSize = g_FileSystem.m_FS.m_Header.m_Data.m_nFileNameSize * g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum;
		DWORD nFATSize       = sizeof(S_GGFFS_FileInfo_Rec) * g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum;

		g_FileSystem.m_FS.m_pszNameTable = malloc(g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum * g_FileSystem.m_FS.m_Header.m_Data.m_nFileNameSize);
		g_FileSystem.m_FS.m_pFAT = malloc(sizeof(S_GGFFS_FileInfo_Rec)*g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum);

		GGFFile_Read(nHandle,(BYTE *)(g_FileSystem.m_FS.m_pszNameTable),nNameTableSize);
		GGFFile_Read(nHandle,(BYTE *)(g_FileSystem.m_FS.m_pFAT),nFATSize);

		g_FileSystem.m_hHandle = (DWORD)nHandle;		
	}
	else
	{
		return GGFFERROR_OPEN;
	}

	return nHandle;
}




char *GGFFS_ID2NameTablePtr(int nID)
{
	return g_FileSystem.m_FS.m_pszNameTable + (g_FileSystem.m_FS.m_Header.m_Data.m_nFileNameSize*nID);

}

/*
실패시 파일최대갯수보다 큰수 반환
*/
DWORD GGFFS_Name2ID(char *szName)
{
	DWORD i;

	for(i=0;i<g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum;i++) //순차검색 ㅡ.ㅡ;;;
	{
		if(	!strcmp(GGFFS_ID2NameTablePtr(i) ,	szName)	)
		{
			break;
		}
	}

	if(i < g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum) //찾아졌으면
	{
		return i;
	}
	else //실패면..
	{
		return g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum+1;
	}
}

/*
파일이오픈된상태에서만가능
팩캐지에서 주어진 파일이름이 있는곳으로 파일포인터를옮긴다.
실패하면 0 을반환
성공하면 실데이터가들어있는 오프셋 위치반환
*/
int GGFFS_SelectFile(const char *szFn)
{
	unsigned long i;

	for(i=0;i<g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum;i++) //순차검색 ㅡ.ㅡ;;;
	{
		if(	!strcmp(GGFFS_ID2NameTablePtr(i) ,	szFn)	)
		{
			break;
		}
	}

	if(i < g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum) //찾아졌으면
	{		
		
		GGFFile_SeekSet((int)(g_FileSystem.m_hHandle),g_FileSystem.m_FS.m_pFAT[i].m_fi.m_nPos);
		//GGFFile_SeekSet(g_FileSystem.m_hHandle,1216);
		return g_FileSystem.m_FS.m_pFAT[i].m_fi.m_nPos;
	}
	else //실패면..
	{
		return 0;
	}

	return 0;
}

unsigned long GGFFS_GetFileSize(const char *szFn)
{
	unsigned long i;

	for(i=0;i<g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum;i++) //순차검색 ㅡ.ㅡ;;;
	{
		if(	!strcmp(GGFFS_ID2NameTablePtr(i),szFn)	)
		{
			break;
		}
	}

	if(i < g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum) //찾아졌으면
	{
		return g_FileSystem.m_FS.m_pFAT[i].m_fi.m_nSize;
	}
	else //실패면..
	{
		return 0;
	}
	
	return 0;
}

/*
현제열려진파일핸들을닫는다.
*/
int GGFFS_Close()
{
	GGFFile_Close((int)g_FileSystem.m_hHandle);

	free(g_FileSystem.m_FS.m_pFAT);
	free(g_FileSystem.m_FS.m_pszNameTable);	

	return 1;
}

/*
빈 파일시스템헤더를 생성한다. 반환값은 구조체크기
*/

DWORD GGFFS_Create(char *szID,DWORD nMaxFileName,DWORD nFileNum)
{
	strcpy(g_FileSystem.m_FS.m_Header.m_Data.m_szId,szID);
	g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum = nFileNum;
	g_FileSystem.m_FS.m_Header.m_Data.m_nFileNameSize = nMaxFileName;


	g_FileSystem.m_FS.m_pszNameTable = malloc(nFileNum * nMaxFileName);
	memset(g_FileSystem.m_FS.m_pszNameTable,0,nFileNum * nMaxFileName);
	
	g_FileSystem.m_FS.m_pFAT = malloc(sizeof(S_GGFFS_FileInfo_Rec)*nFileNum);
	memset(g_FileSystem.m_FS.m_pFAT,0,sizeof(S_GGFFS_FileInfo_Rec)*nFileNum);

	g_FileSystem.m_hHandle = 0;		

	return (nFileNum * nMaxFileName) + (sizeof(S_GGFFS_FileInfo_Rec)*nFileNum) + sizeof(S_GGFFS_HEADER);
}

PS_GGFFS_OBJ GetFileSystemPtr()
{
	return &g_FileSystem;
}

void GGFFS_Read(char *szFName,DWORD nLen,BYTE *pBuf)
{
	GGFFS_SelectFile(szFName);
	GGFFile_Read(g_FileSystem.m_hHandle,pBuf,nLen);
}

DWORD GGFFS_GetOffset(char *szName)
{
	DWORD dwIndex;

	dwIndex = GGFFS_Name2ID(szName);

	if(dwIndex < g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum)
	{		
		return g_FileSystem.m_FS.m_pFAT[dwIndex].m_fi.m_nPos;		
	}
	return 0;
}

DWORD GGFFS_GetFATSize()
{
	return sizeof(S_GGFFS_HEADER_Rec) + 
		(sizeof(S_GGFFS_FileInfo_Rec) * g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum) + 
		(g_FileSystem.m_FS.m_Header.m_Data.m_nFileNameSize * g_FileSystem.m_FS.m_Header.m_Data.m_nFileNum);
}

PS_GGFFS_OBJ	GGFFS_GetFileSystemPtr()
{
	return &g_FileSystem;
}

