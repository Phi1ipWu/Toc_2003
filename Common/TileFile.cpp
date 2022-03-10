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



#include "TileFile.h"




//////////////////////////////////////////////////////////////////
//
//  功能：默认构造函数
//  日期：2005-09-02
//
CTileFile::CTileFile()
{
    m_nGroundCount = 0;
    m_pGroundPos   = 0;
    m_szFileName   = 0;
}




//////////////////////////////////////////////////////////////////
//
//  功能：析构函数（虚析构）
//  日期：2005-09-02
//
CTileFile::~CTileFile()
{
    Close();
}




//////////////////////////////////////////////////////////
//
//  功能：获取文件头上的信息（场景个数，场景数据开始位）
//  参数：const char* szFileName [in]  文件路径
//  日期：2005-09-02
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::Open(const char* szFileName)
{
    if(m_szFileName)
    {
        //  如果文件名一样，则不用再次打开
        if(0 == strcmp(szFileName, m_szFileName))
        {
            return 1L;
        }
        else
        {
            delete[] m_szFileName;
            m_szFileName = 0;
        }
    }

    FILE* fp;

    //
    //  1. 先打开待打开的文件，打开方式为只读
    //
    if(0 == (fp = fopen(szFileName, "rb")))
    {
        return 0L;
    }


    //
    //  2.检查数据文件的头两个字节，判断是否是 TF
    //
    if(! CheckFileHead(fp))
    {
        //  文件头确认失败
        fclose(fp);
        return 0L;
    }


    //
    //  3.  读取文件信息，包含版本信息，保留位，以及最重要的场景个数
    //
    if(! GetFileInfo(fp))
    {
        //  获取文件信息失败
        fclose(fp);
        return 0L;
    }


    //
    //  4.  读取全部场景数据在文件中的开头位置
    //
    if(! GetDataPos(fp))
    {
        //  获取全部数据位置失败
        fclose(fp);
        return 0L;
    }


    //
    //  5.  保存文件路径
    //
    long nLen = (long) strlen(szFileName);
    m_szFileName = new char[nLen + 1];
    strcpy(m_szFileName, szFileName);
    m_szFileName[nLen] = 0;

    //  文件数据头读取完成
    //  关闭文件
    fclose(fp);

    return 1L;
}




//////////////////////////////////////////////////////////
//
//  功能：关闭当前文件，将清空场景数据
//  参数：FILE* fp [in]  文件指针
//  日期：2005-09-02
//
void CTileFile::Close()
{
    if(m_pGroundPos)
    {
        //  安全删除存放场景数据头动态数组
        delete[] m_pGroundPos;
        m_pGroundPos = 0;
    }

    if(m_szFileName)
    {
        //  安全删除存放文件名动态数组
        delete[] m_szFileName;
        m_szFileName = 0;
    }

    //  场景个数清零
    m_nGroundCount = 0;
}




//////////////////////////////////////////////////////////
//
//  功能：从打包文件中读取指定的场景数据
//  参数：LPGROUND pGround [in]  数据指针
//        long nID         [in]  数据编号
//  日期：2005-09-06
//  返回值：1 - 成功    0 - 失败
//
//  注：nID 可以包含 0，文件必须已经打开
//
long CTileFile::ReadGroundData(LPGROUND pGround, long nID)
{
    if(0 == pGround || 0 == m_pGroundPos || 0 > nID || nID >= m_nGroundCount)
    {
        //  指针不合法，或者场景 ID 不合法
        return 0L;
    }

    FILE* fp;

    unsigned long nLine,    //  行数
                  nColumn;  //  列数

    //  打开文件
    if(0 == (fp = fopen(m_szFileName, "rb")))
    {
        return 0L;
    }

    //  将文件游标移到数据开始处
    if(! SeekToDataPos(fp, nID))
    {
        fclose(fp);
        return 0L;
    }

    //  检查数据头（0 - 1）
    if(! CheckDataHead(fp))
    {
        fclose(fp);
        return 0L;
    }

    //  读取图素库号（2 - 3）
    if(! GetWordValue(fp, pGround->nTile))
    {
        fclose(fp);
        return 0L;
    }
    
    //  读取背景音乐（4 - 5）
    if(! GetWordValue(fp, pGround->nBGM))
    {
        fclose(fp);
        return 0L;
    }

    //  读取场景的行数（6 - 7）
    if(! GetWordValue(fp, nLine))
    {
        fclose(fp);
        return 0L;
    }

    //  读取场景的列数（8 - 9）
    if(! GetWordValue(fp, nColumn))
    {
        fclose(fp);
        return 0L;
    }

    //  设置场景数据大小
    if(! pGround->mtAreas.SetSize(nLine, nColumn))
    {
        fclose(fp);
        return 0L;
    }

    //  填充数据（0x0D - ??）
    Area32x32 oArea;
    long nCount = nLine * nColumn;
    for(long i = 0; i < nCount; ++i)
    {
        //  读取下一场景
        if(! ReadArea(fp, oArea))
        {
            pGround->mtAreas.Delete();
            fclose(fp);
            return 0L;
        }

        pGround->mtAreas.Fill(&oArea);
    }

    //  检查数据尾
    if(! CheckDataEnd(fp))
    {
        pGround->mtAreas.Delete();
        fclose(fp);
        return 0L;
    }

    //  整个过程没有出错
    fclose(fp);
    return 1L;
}




//////////////////////////////////////////////////////////
//
//  功能：读取单独数据文件的数据
//  参数：char* szFileName [in]  文件路径
//        LPGROUND pGround [in]  数据指针
//  日期：2005-09-19
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::ReadDataFile(char* szFileName, LPGROUND pGround)
{
    if(0 == szFileName || 0 == pGround)
    {
        //  指针不合法
        return 0L;
    }

    FILE* fp;

    unsigned long nLine,    //  行数
                  nColumn;  //  列数

    //  打开文件
    if(0 == (fp = fopen(szFileName, "rb")))
    {
        return 0L;
    }

    //  检查数据头（0 - 1）
    if(! CheckDataHead(fp))
    {
        fclose(fp);
        return 0L;
    }

    //  读取图素库号（2 - 3）
    if(! GetWordValue(fp, pGround->nTile))
    {
        fclose(fp);
        return 0L;
    }
    
    //  读取背景音乐（4 - 5）
    if(! GetWordValue(fp, pGround->nBGM))
    {
        fclose(fp);
        return 0L;
    }

    //  读取场景的行数（6 - 7）
    if(! GetWordValue(fp, nLine))
    {
        fclose(fp);
        return 0L;
    }

    //  读取场景的列数（8 - 9）
    if(! GetWordValue(fp, nColumn))
    {
        fclose(fp);
        return 0L;
    }

    //  设置场景数据大小
    if(! pGround->mtAreas.SetSize(nLine, nColumn))
    {
        fclose(fp);
        return 0L;
    }

    //  填充数据
    Area32x32 oArea;
    long nCount = nLine * nColumn;
    for(long i = 0; i < nCount; ++i)
    {
        //  读取下一场景
        if(! ReadArea(fp, oArea))
        {
            pGround->mtAreas.Delete();
            fclose(fp);
            return 0L;
        }

        pGround->mtAreas.Fill(&oArea);
    }

    //  检查数据尾
    if(! CheckDataEnd(fp))
    {
        pGround->mtAreas.Delete();
        fclose(fp);
        return 0L;
    }

    //  整个过程没有出错
    fclose(fp);
    return 1L;
}




//////////////////////////////////////////////////////////
//
//  功能：写取单独场景数据
//  参数：const char* szFileName [in] 要写入的文件名
//        const LPGROUND pGround [in] 数据指针
//  日期：2005-09-14
//  返回值：1 - 成功    0 - 失败
//
//  注：nID 可以包含 0
//
long CTileFile::WriteGroundData(const char* szFileName, const LPGROUND pGround)
{
    //  判定指针是否有效
    if(0 == szFileName || 0 == pGround)
    {
        return 0L;
    }
/*
    //  数据不合法
    if(! pGround->mtAreas.IsValid())
    {
        return 0L;
    }
*/
    FILE* fp;

    //
    //  打开文件
    //
    if(0 == (fp = fopen(szFileName, "wb+")))
    {
        return 0L;
    }

    unsigned char szBuffer[8] = {0};

    //
    //  写数据头 DH 标记
    //
    szBuffer[0] = 'D';
    szBuffer[1] = 'H';
    fwrite(szBuffer, sizeof(unsigned char), 2, fp);

    //
    //  写入信息，包括图素编号，音乐编号，行数，列数
    //
    szBuffer[0] = (unsigned char)((pGround->nTile >> 8) & 0xFF);
    szBuffer[1] = (unsigned char)(pGround->nTile & 0xFF);
    szBuffer[2] = (unsigned char)((pGround->nBGM >> 8) & 0xFF);
    szBuffer[3] = (unsigned char)(pGround->nBGM & 0xFF);
    szBuffer[4] = (unsigned char)((pGround->mtAreas.GetLineCount() >> 8) & 0xFF);
    szBuffer[5] = (unsigned char)(pGround->mtAreas.GetLineCount() & 0xFF);
    szBuffer[6] = (unsigned char)((pGround->mtAreas.GetColumnCount() >> 8) & 0xFF);
    szBuffer[7] = (unsigned char)(pGround->mtAreas.GetColumnCount() & 0xFF);
    fwrite(szBuffer, sizeof(unsigned char), 8, fp);

    //
    //  写入全部的数据
    //
    long nBufferSize;
    Area32x32** pAreas = pGround->mtAreas.GetDataRef();
    for(long i = 0; i < pGround->mtAreas.GetLineCount(); ++i)
    {
        for(long j = 0; j < pGround->mtAreas.GetColumnCount(); ++j)
        {
        	//	先设置数据头
            nBufferSize = 2;
            szBuffer[0] = (unsigned char)((pAreas[i][j].nAreaInfo >> 8) & 0xFF);
            szBuffer[1] = (unsigned char)((pAreas[i][j].nAreaInfo) & 0xFF);
            ///szBuffer[1] = 0;

			//	动画地砖
			///if(pGround->mtAreas[i][j].stTile[0].change) szBuffer[1] |= 0x04;
			///if(pGround->mtAreas[i][j].stTile[1].change) szBuffer[1] |= 0x02;
			///if(pGround->mtAreas[i][j].stTile[2].change) szBuffer[1] |= 0x01;

            //  层一数据
            if(0 <= pAreas[i][j].stTile[0].line && 0 <= pAreas[i][j].stTile[0].column)
            {
                szBuffer[1] |= 0x40;

                if(0 >= j || pAreas[i][j].stTile[0].line != pAreas[i][j - 1].stTile[0].line ||
                    pAreas[i][j].stTile[0].column != pAreas[i][j - 1].stTile[0].column)
                {
                    //  和之前一块区域的层一数据不同
                    szBuffer[1] |= 0x80;
                    szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[0].column);
                    szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[0].line);
                }

            }

            //  层二数据
            if(0 <= pAreas[i][j].stTile[1].line && 0 <= pAreas[i][j].stTile[1].column)
            {
                szBuffer[1] |= 0x20;
                szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[1].column);
                szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[1].line);
            }

            //  层三数据
            if(0 <= pAreas[i][j].stTile[2].line && 0 <= pAreas[i][j].stTile[2].column)
            {
                szBuffer[1] |= 0x10;
                szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[2].column);
                szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[2].line);
            }

            fwrite(szBuffer, sizeof(unsigned char), nBufferSize, fp);

            //  锚点数据
            if((szBuffer[0] & 0x40) == 0x40)
            {
                szBuffer[2] = (unsigned char)((pAreas[i][j].nAnchor >> 8) & 0xFF);
                szBuffer[3] = (unsigned char) (pAreas[i][j].nAnchor & 0xFF);
                fwrite(&szBuffer[2], sizeof(unsigned char), 2, fp);
            }
        }
    }

    //
    //  写数据尾 DE 标记
    //
    szBuffer[0] = 'D';
    szBuffer[1] = 'E';
    szBuffer[2] = 0;
    szBuffer[3] = 0;
    fwrite(szBuffer, sizeof(unsigned char), 4, fp);

    fclose(fp);
    return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  功能：将多个单独的场景文件写成一个文件，以供游戏中读取
//  参数：const char* szTileFileName [in]  新拼接后的文件
//        char** pDataFileNames      [in]  全部待拼接的文件
//        const long nCount          [in]  待拼接文件的个数
//  日期：2005-09-15
//  返回值：成功拼接文件的个数    0 - 失败
//
long CTileFile::LinkTileFile(const char* szTileFileName, const char* const* pDataFileNames, const long nCount)
{
    if(0 == szTileFileName || 0 == pDataFileNames || 0 >= nCount)
    {
        return 0L;
    }

    FILE* fpWrite;
    FILE* fpRead;

    //  读文件数据的缓冲，一次可以复制 1K 数据
    unsigned char szBuffer[1024];

    //  一次读取的字节数
    long nReadBufLength = 0;
    
    //  距离文件头的位置
    long nBeforeLength  = 8 + nCount * 4;
    
    //  存放临时的文件头位置的数据
    long* pDataPos = 0;
    long i;

    //
    //  打开待写入的文件，以写方式打开
    //
    if(0 == (fpWrite = fopen(szTileFileName, "wba+")))
    {
        return 0L;
    }

    //
    //  先写入文件头信息
    //
    szBuffer[0] = 'T';
    szBuffer[1] = 'F';
    szBuffer[2] = 0;
    szBuffer[3] = 0;
    szBuffer[4] = 0;
    szBuffer[5] = 0;
    szBuffer[6] = (unsigned char)((nCount >> 8) & 0xFF);
    szBuffer[7] = (unsigned char)(nCount & 0xFF);
    fwrite(szBuffer, sizeof(unsigned char), 8, fpWrite);

    //
    //  写入空的数据头位置
    //
    memset(szBuffer, 0, sizeof(szBuffer));
    for(i = 0; i < nCount; ++i)
    {
        fwrite(szBuffer, sizeof(unsigned char), 4, fpWrite);
    }

    //
    //  数据对拷
    //
    try
    {
        //  存放文件前部的数据开始位置
        pDataPos = new long[nCount];
        memset(pDataPos, 0, sizeof(long) * nCount);

        for(i = 0; i < nCount; ++i)
        {
            //  先保存数据开始位置
            pDataPos[i] = nBeforeLength;

            //  打开文件
            if(0 == (fpRead = fopen(pDataFileNames[i], "rb")))
            {
                //  打开文件失败，数据头位置置 0，继续读取下一个文件
                pDataPos[i] = 0;
                continue;
            }

            //  文件复制，读出来后立刻写入
            while(nReadBufLength = (long)fread(szBuffer, sizeof(unsigned char), sizeof(szBuffer), fpRead))
            {
                fwrite(szBuffer, sizeof(unsigned char), nReadBufLength, fpWrite);
                nBeforeLength += nReadBufLength;
            }

            fclose(fpRead);
        }
    }
    catch(...)
    {
        //  读取过程中出错。关闭文件，销毁已分配内存
        fclose(fpWrite);
        if(pDataPos)
        {
            delete[] pDataPos;
        }
        return 0L;
    }


    //
    //  定位到数据位置头，也就是第 8 个字节
    //  写入数据头位置
    //
    fseek(fpWrite, -(nBeforeLength - 8), SEEK_CUR);

    for(i = 0; i < nCount; ++i)
    {
        szBuffer[0] = (unsigned char)((pDataPos[i] & 0xFF000000) >> 24);
        szBuffer[1] = (unsigned char)((pDataPos[i] & 0x00FF0000) >> 16);
        szBuffer[2] = (unsigned char)((pDataPos[i] & 0x0000FF00) >> 8);
        szBuffer[3] = (unsigned char) (pDataPos[i] & 0x000000FF);
        fwrite(szBuffer, sizeof(unsigned char), 4, fpWrite);
    }

    //
    //  写文件成功
    //  文件的关闭，已分配内存的释放
    //
    fclose(fpWrite);
    delete[] pDataPos;

    return 1L;
}




//////////////////////////////////////////////////////////
//
//  功能：检查文件头，如果通过返回 1，不通过返回 0
//  参数：FILE* fp [in]  文件指针
//  日期：2005-09-02
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::CheckFileHead(FILE* fp)
{
    if(! fp)
    {
        return 0L;
    }

    char szBuffer[2];
    if(2 == fread(szBuffer, sizeof(char), 2, fp))
    {
        //  判断文件头是否是 TF
        if('T' == szBuffer[0] && 'F' == szBuffer[1])
        {
            return 1L;
        }
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  功能：读取文件信息，如果通过返回 1，不通过返回 0
//  参数：FILE* fp [in]  文件指针
//  日期：2005-09-02
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::GetFileInfo(FILE* fp)
{
    if(! fp)
    {
        return 0L;
    }

    unsigned char szBuffer[6];
    if(6 == fread(szBuffer, sizeof(unsigned char), 6, fp))
    {
        m_nVersion     = (szBuffer[0] << 8) | (szBuffer[1]);
        m_nKeep        = (szBuffer[2] << 8) | (szBuffer[3]);
        m_nGroundCount = (szBuffer[4] << 8) | (szBuffer[5]);

        return 1L;
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  功能：获取全部的数据头位置
//  参数：FILE* fp [in]  文件指针
//  日期：2005-09-02
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::GetDataPos(FILE* fp)
{
    if(! fp || 0 >= m_nGroundCount)
    {
        //  如果地图个数不为正整数的话直接返回
        return 0L;
    }

    //  判断数据头是否已经读取，如果已读取则先释放原来的内存
    if(m_pGroundPos)
    {
        delete[] m_pGroundPos;
        m_pGroundPos = 0;
    }

    //  重新分配数据头的大小
    m_pGroundPos = new unsigned long[m_nGroundCount];

    for(long i = 0; i < m_nGroundCount; ++i)
    {
        //  读取四个字节的数据头位置
        if(GetDWordValue(fp, m_pGroundPos[i]))
        {
            //  判断数据头位置是否合法
            if(12 > m_pGroundPos[i])
            {
                m_pGroundPos[i] = 0;

            }
        }
        else
        {
            return 0L;
        }
    }

    return 1L;
}




//////////////////////////////////////////////////////////
//
//  功能：定位到文件当前位置
//  参数：FILE* fp [in]  文件指针
//  日期：2005-09-07
//  返回值：1 - 成功    0 - 失败
//
//  注：nID 可能包含 0
//
long CTileFile::SeekToDataPos(FILE* fp, long nID)
{
    if(0 == m_pGroundPos || 0 == fp || 0 > nID || nID >= m_nGroundCount)
    {
        return 0L;
    }

    return !fseek(fp, m_pGroundPos[nID], SEEK_CUR);
}




//////////////////////////////////////////////////////////
//
//  功能：检查数据头，如果通过返回 1，不通过返回 0
//  参数：FILE* fp [in]  文件指针
//  日期：2005-09-07
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::CheckDataHead(FILE* fp)
{
    char szBuffer[2];

    if(2 == fread(szBuffer, sizeof(char), 2, fp))
    {
        //  判断数据头是否是 DH (DataHead)
        if('D' == szBuffer[0] && 'H' == szBuffer[1])
        {
            return 1L;
        }
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  功能：检查数据尾，如果通过返回 1，不通过返回 0
//  参数：FILE* fp [in]  文件指针
//  日期：2005-09-07
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::CheckDataEnd(FILE* fp)
{
    char szBuffer[2];

    if(2 == fread(szBuffer, sizeof(char), 2, fp))
    {
        //  判断数据头是否是 DH (DataEnd)
        if('D' == szBuffer[0] && 'E' == szBuffer[1])
        {
            return 1L;
        }
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  功能：获取地图的大小，如果通过返回 1，不通过返回 0
//  参数：FILE* fp     [in]      文件指针
//        long& line   [in out]  行数，引用
//        long& column [in out]  列数，引用
//  日期：2005-09-07
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::GetGroundSize(FILE* fp, long& line, long& column)
{
    unsigned char szBuffer[4];

    //  读取行数和列数
    if(4 == fread(szBuffer, sizeof(unsigned char), 4, fp))
    {
        line   = (szBuffer[0] << 8) | (szBuffer[1]);
        column = (szBuffer[2] << 8) | (szBuffer[3]);
    }
    else
    {
        return 0L;
    }

    if(0 >= line && 0 >= column)
    {
        return 0L;
    }

    return 1L;
}




//////////////////////////////////////////////////////////
//
//  功能：读取下一块 32x32 包含三层的区域
//  参数：FILE* fp         [in]      文件指针
//        Area32x32& oArea [in out]  单块区域的引用
//  日期：2005-09-07
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::ReadArea(FILE* fp, Area32x32& oArea)
{
    if(! fp)
    {
        return 0L;
    }

    if(! GetWordValue(fp, oArea.nAreaInfo)) return 0L;

    //  读取层一的数据
    if(oArea.nAreaInfo & 0x40)
    {
        //  表示当前的层一的数据和之前不一样
        if(oArea.nAreaInfo & 0x80)
        {
            if(! GetByteValue(fp, oArea.stTile[0].column)) return 0L;
            if(! GetByteValue(fp, oArea.stTile[0].line))   return 0L;
        }

		//	动画区域
		///oArea.stTile[0].change = oArea.nAreaInfo & 0x04 ? 4 : 0;
    }
    else
    {
        oArea.stTile[0].line   = -1;
        oArea.stTile[0].column = -1;
		///oArea.stTile[0].change = 0;
    }

    //  读取层二的数据
    if(oArea.nAreaInfo & 0x20)
    {
        if(! GetByteValue(fp, oArea.stTile[1].column)) return 0L;
        if(! GetByteValue(fp, oArea.stTile[1].line))   return 0L;

		//	动画区域
		///oArea.stTile[1].change = oArea.nAreaInfo & 0x02 ? 4 : 0;
	}
    else
    {
        oArea.stTile[1].line   = -1;
        oArea.stTile[1].column = -1;
		///oArea.stTile[1].change = 0;
    }

    //  读取层三的数据
    if(oArea.nAreaInfo & 0x10)
    {
        if(! GetByteValue(fp, oArea.stTile[2].column)) return 0L;
        if(! GetByteValue(fp, oArea.stTile[2].line))   return 0L;

		//	动画区域
		///oArea.stTile[2].change = oArea.nAreaInfo & 0x01 ? 4 : 0;
    }
    else
    {
        oArea.stTile[2].line   = -1;
        oArea.stTile[2].column = -1;
		///oArea.stTile[2].change = 0;
    }

    //  读取锚点编号
    if((oArea.nAreaInfo & 0x4000) == 0x4000)
    {
        if(! GetWordValue(fp, oArea.nAnchor)) return 0L;
    }
    else
    {
        oArea.nAnchor = -1;
    }

    return 1L;
}




//////////////////////////////////////////////////////////
//
//  功能：读取四个字节（双字），并转换成数值
//  参数：FILE* fp              [in]      文件指针
//        unsigned long& nValue [in out]  数值
//  日期：2005-09-13
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::GetDWordValue(FILE* fp, unsigned long& nValue)
{
    unsigned char szBuffer[4] = {0};
    if(4 == fread(szBuffer, sizeof(unsigned char), 4, fp))
    {
        nValue = ((szBuffer[0] << 24) | (szBuffer[1] << 16) | (szBuffer[2] << 8) | szBuffer[3]);
        return 1L;
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  功能：读取二个字节（单字），并转换成数值
//  参数：FILE* fp     [in]      文件指针
//        long& nValue [in out]  数值
//  日期：2005-09-13
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::GetWordValue(FILE* fp, long& nValue)
{
    unsigned char szBuffer[2] = {0};
    if(2 == fread(szBuffer, sizeof(unsigned char), 2, fp))
    {
        nValue = (szBuffer[0] << 8) | (szBuffer[1]);
        return 1L;
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  功能：读取二个字节（单字），并转换成数值
//  参数：FILE* fp              [in]      文件指针
//        unsigned long& nValue [in out]  数值
//  日期：2005-09-13
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::GetWordValue(FILE* fp, unsigned long& nValue)
{
    unsigned char szBuffer[2] = {0};
    if(2 == fread(szBuffer, sizeof(unsigned char), 2, fp))
    {
        nValue = (szBuffer[0] << 8) | (szBuffer[1]);
        return 1L;
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  功能：读取一个字节（字节），并转换成数值
//  参数：FILE* fp     [in]      文件指针
//        long& nValue [in out]  数值
//  日期：2005-09-13
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::GetByteValue(FILE* fp, long& nValue)
{
    unsigned char cByte;

    if(1 == fread(&cByte, sizeof(unsigned char), 1, fp))
    {
        nValue = cByte;
        return 1L;
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  功能：读取一个字节（字节），并转换成数值
//  参数：FILE* fp              [in]      文件指针
//        unsigned long& nValue [in out]  数值
//  日期：2005-09-13
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::GetByteValue(FILE* fp, unsigned long& nValue)
{
    unsigned char cByte;

    if(1 == fread(&cByte, sizeof(unsigned char), 1, fp))
    {
        nValue = cByte;
        return 1L;
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  功能：读取一个字节（字节），并转换成数值
//  参数：FILE* fp              [in]      文件指针
//        short& nValue [in out]  数值
//  日期：2005-09-13
//  返回值：1 - 成功    0 - 失败
//
long CTileFile::GetByteValue(FILE* fp, short& nValue)
{
    unsigned char cByte;

    if(1 == fread(&cByte, sizeof(unsigned char), 1, fp))
    {
        nValue = cByte;
        return 1L;
    }

    return 0L;
}




//  控制台使用，用来输出一些调试信息
void CTileFile::PrintInfo()
{
    printf("\nFileInfo:\n");
    printf("    Version = %d \n", m_nVersion);
    printf("    Keep    = %d \n", m_nKeep);
    printf("    Count   = %d \n", m_nGroundCount);
    printf("    File    = %s \n", m_szFileName);
    printf("\nDataPos:\n");

    for(long i = 0; i < m_nGroundCount; ++i)
    {
        printf("    %d - %d\n", i + 1, m_pGroundPos[i]);
    }

    printf("\n\n");
}



/*
//  将多个单独场景的数据文件拼合
#include <stdlib.h>
int main(int argc, char* argv[])
{
    if(3 > argc || 32768 <= argc)
    {
        printf("  Link Failed. Not enough count file to link.\n\n");
    }
    else
    {
        for(int i = 2; i < argc; ++i)
        {
            printf("  %05d  %s\n", i - 1, argv[i]);
        }

        CTileFile tf;
        if(tf.LinkTileFile(argv[1], &argv[2], argc - 2))
        {
            printf("  ===>>  %s\n\n", argv[1]);
        }
        else
        {
            printf("  Link Failed. Write file error.\n\n");
        }
    }

    system("pause");
}
*/


/*
//  测试读取效果
int main()
{
    CTileFile tf;

    //if(! tf.Open("D:\\Program\\Game\\TileFile\\Ground.tlm"))
    //if(! tf.Open("G:\\OwnSrc\\TileFile\\Ground.tlm"))
    if(! tf.Open("D:\\Link.tlm"))
    {
        printf("CTileFile::Open failed.\n");
        return 0;
    }

    GROUND stGround;
    if(! tf.ReadGroundData(&stGround, 0))
    {
        printf("ReadTileData failed.\n");
        return 0;
    }

//    tf.WriteGroundData("G:\\Test.dat", &stGround);

    const long nTestLayer = 1;
    Area32x32** pArea = stGround.mtAreas.GetDataRef();
    for(long i = 0; i < stGround.mtAreas.GetLineCount(); ++i)
    {
        for(long j = 0; j < stGround.mtAreas.GetColumnCount(); ++j)
        {
            printf("[%ld,%ld]", pArea[i][j].stTile[nTestLayer].column, pArea[i][j].stTile[nTestLayer].line);
        }
        printf("\n");
    }
}
*/



/*
//  生成一个自定义大小的场景文件，随机生成层二层三
#include <stdlib.h>
#include <windows.h>
int main()
{
    const long nLineCnt   = 3000;
    const long nColumnCnt = 3000;

    GROUND gd;
    gd.nTile = 1;
    gd.nBGM = 13;
    if(! gd.mtAreas.SetSize(nLineCnt, nColumnCnt))
    {
        printf("SetSize Failed !!! \n");
        return 0;
    }

    Area32x32 oArea;
    oArea.nAreaInfo = 0x8000;
    oArea.stTile[0].line   = 0;
    oArea.stTile[0].column = 0;

    srand(::GetTickCount());
    for(int i = 0; i < nLineCnt * nColumnCnt; ++i)
    {
        if(rand() % 2)
        {
            oArea.stTile[1].line   = rand() % 40;
            oArea.stTile[1].column = rand() % 8;
        }
        else
        {
            oArea.stTile[1].line   = -1;
            oArea.stTile[1].column = -1;
        }

        if(rand() % 2)
        {
            oArea.stTile[2].line   = rand() % 40;
            oArea.stTile[2].column = rand() % 8;
        }
        else
        {
            oArea.stTile[2].line   = -1;
            oArea.stTile[2].column = -1;
        }
        
        gd.mtAreas.Fill(&oArea);
    }

    CTileFile tf;
    tf.WriteGroundData("D:\\Test.dat", &gd);
}
*/
