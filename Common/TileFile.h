////////////////////////////////////////////////////////////////////////////
//
//  ���������ļ�������Ϸ���촫˵��һ���֣�ȫ����Ȩ���� COKUCO ����������
//        ������Ȩ��COKUCO �����ұ�����������Ȩ��
//
//  ������CTileFile 
//  ������רע�ڱ���Ͷ�ȡ���� TILE ���ļ���
//        �����ռ������ʷ��ӵ��˼��� :)
//
//  ���ߣ�Kudoxinyiji
//  ���ڣ�2005-09-13
//
//  �ļ���ʽ���£�
//
//  �ļ�ͷ��
//  MF      �ļ���ʶ        2 Byte      0000 ~ 0001
//  00      �汾            2 Byte      0002 ~ 0003
//  00      ������          2 Byte      0004 ~ 0005
//  ??      ��ͼ����        2 Byte      0006 ~ 0007
//  ????    ������ͼ����ͷ  4 Byte      0008 ~ 000B
//  ...
//  ...
//  ????    ������ͼ����ͷ  4 Byte      ???? ~ ��ͼ���� x 4 + 7
//
//  ��������ͷ��
//  DH      ����ͷ          2 Byte
//  ??      ͼ�ؿ��        2 Byte
//  ??      ���ֱ��        2 Byte
//  ??      ����            2 Byte
//  ??      ����            2 Byte
//  ....
//  ������һ����������
//  ....
//  DE      ����β          2 Byte
//
////////////////////////////////////////////////////////////////////////////



#ifndef _TILEFILE_H
#define _TILEFILE_H


//  ���ݴ���ĺ궨�壬���ֽڴ���ʽ���ѷ���
//#define LineNum(num)        ((num) & 31)
//#define ColumnNum(num)      (((num) >> 5) & 7)
//#define MakeByte(col, line) (((col) & 7) << 5 | (line) & 31)
#define IsAreaCanStand(objArea) (objArea.nAreaInfo & 0x8000)



//  ������ C ͷ�ļ�
#include <stdio.h>
#include <string.h>

//  ����ͼ��������
#include "TMatrix.cpp"

//  ���� .h ���ӳ���Ҫ�ĳɰ��� .cpp �ļ����ɣ�������
//  http://www.comeaucomputing.com/techtalk/templates/#book



//  ���� 32 x 32 �ĵ�ש
struct Tile32x32
{
	//	��
    short line;

    //	��
    short column;

    //	״̬�仯(����״̬�л������ڵ�ǰ��ש���ұ�)
    ///long change;

    Tile32x32()
    {   line = -1, column = -1/*, change = 0*/;  }

    inline long IsValid()
    {
        return (line >= 0 && column >= 0) ? 1 : 0;
    }
};



//  һƬ 32 x 32 �����򣬿��ܻ���� N ��� Tile32x32 
struct Area32x32
{
    //  ������Ϣ
    long nAreaInfo;
    
    //  ê����
    long nAnchor;

    //  ������������
    Tile32x32 stTile[3];

	//	���õ�ש�Ƿ�״̬�仯(����״̬�л������ڵ�ǰ��ש���ұ�)
	void SetChangeTile(long nLayer, long nEnable)
	{
		if(0 == nEnable)
		{
			//	��Ч
			switch(nLayer)
			{
			case 0:
				nAreaInfo &= 0xFFFB;
				break;
			case 1:
				nAreaInfo &= 0xFFFD;
				break;
			case 2:
				nAreaInfo &= 0xFFFE;
				break;
			}
		}
		else
		{
			//	��Ч
			switch(nLayer)
			{
			case 0:
				nAreaInfo |= 0x0004;
				break;
			case 1:
				nAreaInfo |= 0x0002;
				break;
			case 2:
				nAreaInfo |= 0x0001;
				break;
			}
		}
	}
};



//  һ��������ȫ�����ݣ��Ժ��п���Ҫ���䣩
typedef struct GROUND
{
    //  ͼ�ر��
    long nTile;

    //  ���ֱ��
    long nBGM;

    //  ��������
    CTMatrix<Area32x32> mtAreas;

} GROUND, *LPGROUND;




class CTileFile
{
public:

    //  Ĭ�Ϲ��캯��
    CTileFile();

    //  ��������
    virtual ~CTileFile();

    //  �Ӵ����ļ��д��ļ�������ȡ�ļ���һЩ��Ϣ���������������ݣ�
    long Open(const char* szFileName);

    //  ���ָ����ŵĿ�ʼ�ĳ�������
    void Close();

    //  ��ȡ��������ļ���ָ����ŵ�����
    long ReadGroundData(LPGROUND pGround, long nID);

    //  ��ȡ���������ļ�������
    long ReadDataFile(char* szFileName, LPGROUND pGround);

    //  ��������������д�ļ������Ƿ�ʽ��
    long WriteGroundData(const char* szFileName, const LPGROUND pGround);

    //  ����������ĳ����ļ�д��һ�� TLM �ļ����Թ���Ϸ�ж�ȡ
    long LinkTileFile(const char* szTlmFileName, const char* const* pDataFileNames, const long nCount);

    //  ���޿���̨������
    void PrintInfo();

protected:

    //  ����ļ�ͷ�����ͨ������ 1����ͨ������ 0
    long CheckFileHead(FILE* fp);

    //  ��ȡ��ǰ�ļ���Ϣ
    long GetFileInfo(FILE* fp);

    //  ��ȡȫ�����ݵĿ�ʼλ
    long GetDataPos(FILE* fp);

    //  ��λ����Ӧ�����Ŀ�ʼλ��
    long SeekToDataPos(FILE* fp, long nID);

    //  �������ͷ�����ͨ������ 1����ͨ������ 0
    long CheckDataHead(FILE* fp);

    //  ��ȡ�����Ĵ�С������������������������
    long GetGroundSize(FILE* fp, long& line, long& column);

    //  ��ȡ��һ������
    long ReadArea(FILE* fp, Area32x32& oArea);

    //  ������ݽ�����־�����ͨ������ 1����ͨ������ 0
    long CheckDataEnd(FILE* fp);

    //  ��ȡ�ĸ��ֽڣ�˫�֣�����ת������ֵ
    static long GetDWordValue(FILE* fp, unsigned long& nValue);

    //  ��ȡ�����ֽڣ����֣�����ת������ֵ
    static long GetWordValue(FILE* fp, long& nValue);
    static long GetWordValue(FILE* fp, unsigned long& nValue);

    //  ��ȡһ���ֽڣ��ֽڣ�����ת������ֵ
    static long GetByteValue(FILE* fp, long& nValue);
    static long GetByteValue(FILE* fp, unsigned long& nValue);
    static long GetByteValue(FILE* fp, short& nValue);

protected:

    //  �ļ��汾
    long m_nVersion;

    //  ������
    long m_nKeep;

    //  ��ǰ�򿪵ĳ������
    long m_nGroundNo;

    //  �Ѿ��򿪵ĳ������� TILE ����
    long m_nGroundWidth;

    //  �Ѿ��򿪵ĳ������� TILE ����
    long m_nGroundHeight;

    //  �ļ���
    char* m_szFileName;

    //  ��������
    long m_nGroundCount;

    //  ÿ���������ļ��е�λ��
    unsigned long* m_pGroundPos;
};



#endif
