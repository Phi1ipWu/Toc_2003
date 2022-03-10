////////////////////////////////////////////////////////////////////////////
//
//  声明：此文件属于游戏创造传说的一部分，全部版权归属 COKUCO 工作室所有
//        如有侵权，COKUCO 工作室保留法律起诉权利
//
//  类名：CTileFile 
//  描述：专注于保存和读取基于 TILE 的文件，
//        并将空间利用率发挥到了极致 :)
//
//  作者：Kudoxinyiji
//  日期：2005-09-13
//
//  文件格式如下：
//
//  文件头：
//  MF      文件标识        2 Byte      0000 ~ 0001
//  00      版本            2 Byte      0002 ~ 0003
//  00      保留字          2 Byte      0004 ~ 0005
//  ??      地图个数        2 Byte      0006 ~ 0007
//  ????    单个地图数据头  4 Byte      0008 ~ 000B
//  ...
//  ...
//  ????    单个地图数据头  4 Byte      ???? ~ 地图个数 x 4 + 7
//
//  单个数据头：
//  DH      数据头          2 Byte
//  ??      图素库号        2 Byte
//  ??      音乐编号        2 Byte
//  ??      行数            2 Byte
//  ??      列数            2 Byte
//  ....
//  单独的一块区域数据
//  ....
//  DE      数据尾          2 Byte
//
////////////////////////////////////////////////////////////////////////////



#ifndef _TILEFILE_H
#define _TILEFILE_H


//  数据处理的宏定义，单字节处理方式，已放弃
//#define LineNum(num)        ((num) & 31)
//#define ColumnNum(num)      (((num) >> 5) & 7)
//#define MakeByte(col, line) (((col) & 7) << 5 | (line) & 31)
#define IsAreaCanStand(objArea) (objArea.nAreaInfo & 0x8000)



//  包含标 C 头文件
#include <stdio.h>
#include <string.h>

//  包含图素数据类
#include "TMatrix.cpp"

//  包含 .h 链接出错，要改成包含 .cpp 文件即可，出处：
//  http://www.comeaucomputing.com/techtalk/templates/#book



//  单块 32 x 32 的地砖
struct Tile32x32
{
	//	行
    short line;

    //	列
    short column;

    //	状态变化(四种状态切换，均在当前地砖的右边)
    ///long change;

    Tile32x32()
    {   line = -1, column = -1/*, change = 0*/;  }

    inline long IsValid()
    {
        return (line >= 0 && column >= 0) ? 1 : 0;
    }
};



//  一片 32 x 32 的区域，可能会包含 N 层的 Tile32x32 
struct Area32x32
{
    //  区域信息
    long nAreaInfo;
    
    //  锚点编号
    long nAnchor;

    //  区域三层数据
    Tile32x32 stTile[3];

	//	设置地砖是否状态变化(四种状态切换，均在当前地砖的右边)
	void SetChangeTile(long nLayer, long nEnable)
	{
		if(0 == nEnable)
		{
			//	无效
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
			//	有效
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



//  一个场景的全部数据（以后还有可能要扩充）
typedef struct GROUND
{
    //  图素编号
    long nTile;

    //  音乐编号
    long nBGM;

    //  场景数据
    CTMatrix<Area32x32> mtAreas;

} GROUND, *LPGROUND;




class CTileFile
{
public:

    //  默认构造函数
    CTileFile();

    //  析构函数
    virtual ~CTileFile();

    //  从磁盘文件中打开文件，并读取文件的一些信息（不包含场景数据）
    long Open(const char* szFileName);

    //  清空指定编号的开始的场景数据
    void Close();

    //  读取打包数据文件的指定编号的数据
    long ReadGroundData(LPGROUND pGround, long nID);

    //  读取单独数据文件的数据
    long ReadDataFile(char* szFileName, LPGROUND pGround);

    //  将单个场景数据写文件（覆盖方式）
    long WriteGroundData(const char* szFileName, const LPGROUND pGround);

    //  将多个单独的场景文件写成一个 TLM 文件，以供游戏中读取
    long LinkTileFile(const char* szTlmFileName, const char* const* pDataFileNames, const long nCount);

    //  仅限控制台调试用
    void PrintInfo();

protected:

    //  检查文件头，如果通过返回 1，不通过返回 0
    long CheckFileHead(FILE* fp);

    //  读取当前文件信息
    long GetFileInfo(FILE* fp);

    //  读取全部数据的开始位
    long GetDataPos(FILE* fp);

    //  定位到对应场景的开始位置
    long SeekToDataPos(FILE* fp, long nID);

    //  检查数据头，如果通过返回 1，不通过返回 0
    long CheckDataHead(FILE* fp);

    //  读取场景的大小，横向区域数和纵向区域数
    long GetGroundSize(FILE* fp, long& line, long& column);

    //  读取下一个场景
    long ReadArea(FILE* fp, Area32x32& oArea);

    //  检查数据结束标志，如果通过返回 1，不通过返回 0
    long CheckDataEnd(FILE* fp);

    //  读取四个字节（双字），并转换成数值
    static long GetDWordValue(FILE* fp, unsigned long& nValue);

    //  读取二个字节（单字），并转换成数值
    static long GetWordValue(FILE* fp, long& nValue);
    static long GetWordValue(FILE* fp, unsigned long& nValue);

    //  读取一个字节（字节），并转换成数值
    static long GetByteValue(FILE* fp, long& nValue);
    static long GetByteValue(FILE* fp, unsigned long& nValue);
    static long GetByteValue(FILE* fp, short& nValue);

protected:

    //  文件版本
    long m_nVersion;

    //  保留字
    long m_nKeep;

    //  当前打开的场景编号
    long m_nGroundNo;

    //  已经打开的场景横向 TILE 个数
    long m_nGroundWidth;

    //  已经打开的场景纵向 TILE 个数
    long m_nGroundHeight;

    //  文件名
    char* m_szFileName;

    //  场景个数
    long m_nGroundCount;

    //  每个场景在文件中的位置
    unsigned long* m_pGroundPos;
};



#endif
