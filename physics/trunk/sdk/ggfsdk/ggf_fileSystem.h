#ifndef _GGF_FILESYSTEM
#define _GGF_FILESYSTEM

enum GGFFSComptype {
	GGFFS_COMP_NO = 0, //무압축
	GGFFS_COMP_RLE, //런랭스압축
	GGFFS_COMP_ZIP,  //zip포멧압축
	GGFFS_COMP_USER
};

enum GGFFS_Attr {
	GGFFS_ATTR_FILE=0, //파일
	GGFFS_ATTR_FOLDER, //폴더
	GGFFS_ATTR_ROOT,   //루트
	GGFFS_ATTR_IODEVICE //입출력장치
	
};

//파일정보
typedef struct _S_GGFFS_FileInfo
{	
	DWORD m_nName;	       //이름
	DWORD m_nAttr;	       //파일속성,파일,폴더,루트,입출력장치...
	DWORD m_nCompressType; //압축형식
	DWORD m_nPos;          //위치
	DWORD m_nSize;         //크기

}S_GGFFS_FileInfo, *PS_GGFFS_FileInfo;

typedef struct _S_GGFFS_FileInfo_Rec
{
	S_GGFFS_FileInfo m_fi;
	unsigned char m_rev[64 - sizeof(S_GGFFS_FileInfo)];

}S_GGFFS_FileInfo_Rec,*PS_GGFFS_FileInfo_Rec;

typedef struct _S_GGFFS_HEADER
{
	BYTE  m_szId[4];
	DWORD m_nFileNameSize;
	DWORD m_nFileNum;

}S_GGFFS_HEADER,*PS_GGFFS_HEADER;

typedef struct _S_GGFFS_HEADER_Rec
{
	S_GGFFS_HEADER m_Data;
	BYTE m_rev[256 - sizeof(S_GGFFS_HEADER)];

}S_GGFFS_HEADER_Rec,*PS_GGFFS_HEADER_Rec;

typedef struct _S_GGFFS
{
	S_GGFFS_HEADER_Rec m_Header;

	char *m_pszNameTable;
	PS_GGFFS_FileInfo_Rec m_pFAT;

}S_GGFFS,PS_GGFFS;

typedef struct _S_GGFFS_OBJ
{
	DWORD m_hHandle; //파일핸들 각운영체제에따라 공용적으로 사용하도록한다.외부적으로 노출되지않도록한다.
	S_GGFFS m_FS;

}S_GGFFS_OBJ,*PS_GGFFS_OBJ;

#ifdef __cplusplus
extern "C" {
#endif

/*
패키지파일을 연다.
*/
DWORD GGFFS_Open(const char *szPackFn);

/*
파일이오픈된상태에서만가능
팩캐지에서 주어진 파일이름이 있는곳으로 파일포인터를옮긴다.
*/
int GGFFS_SelectFile(const char *szFn);

/*
아카이브 파일싸이즈를구한다
*/
DWORD GGFFS_GetFileSize(const char *szFn);

/*
패키지파일을닫는다.
*/
int GGFFS_Close();

/*
	파일정보테이블위치인덱스를받아서 이름이 저장되있는 버퍼를반환한다.
*/
char *GGFFS_ID2NameTablePtr(int nID);


/*
	이름을 받아서 파일정보테이블위치인덱스로...
*/
DWORD GGFFS_Name2ID(char *szName);


/*
빈 패키지 파일 시스템을 생성한다.
*/
DWORD GGFFS_Create(char *szID,DWORD nMaxFileName,DWORD nFileNum);

/*
파일시스템 객체를구한다.
*/
PS_GGFFS_OBJ	GGFFS_GetFileSystemPtr();

/*
파일내용을 nLen 만큼 버퍼에 넣어보내준다. 버퍼는 함수호출전에 할당되어야한다.
*/
void			GGFFS_Read(char *szFName,DWORD nLen,BYTE *pBuf);

/*
파일시스템기준으로 헤더를 포함하여 주어진파일이 위치한 오프셋을 구한다.
seek함수오 같이 사용가능하다.
실패하면 0
*/
DWORD			GGFFS_GetOffset(char *szFName);

/*
패커파일의 헤더부분싸이즈를구한다.
*/
DWORD GGFFS_GetFATSize();


#ifdef __cplusplus
}
#endif


//공용 파일 입출력함수

#define GGF_MAXFILE 5

enum GGF_FileError {
	GGFFERROR_OPEN = GGF_MAXFILE,
	GGFFERROR_ETC
};

#ifdef __cplusplus
extern "C" {
#endif

	enum GGFF_ACCESS_MODE {
		GGFFMODE_READ = 0,
		GGFFMODE_WRITE
	};	

	DWORD	GGFFile_Read(DWORD hHandle,BYTE *pBuf,DWORD nLen);
	DWORD	GGFFile_Write(DWORD hHandle,BYTE *pBuf,DWORD nLen);	
	DWORD	GGFFile_SeekSet(DWORD hHandle,int nPos);
	DWORD	GGFFile_SeekCur(DWORD hHandle,int nPos);
	DWORD	GGFFile_Close(DWORD hHandle);
	DWORD	GGFFile_GetFileSize(DWORD hHandle);
	/*
	성공이면 GGFFERROR_OPEN 이하의 값을반환한다.
	에러 검사는 
	if(GGFFile_Open(...) >= GGFFERROR_OPEN)
	{
	//에러처리
	}
	*/
	DWORD	GGFFile_Open(const char *szFile,DWORD nMode);
	void	GGFFile_Init();	

	/*
	0 이면 no error
	아니면 에러
	*/
	int GGFFile_GetError(DWORD hHandle);
	
	
	void *GGFFile_GetSystemHandle(DWORD nHandle);


#ifdef __cplusplus
}
#endif



#endif

