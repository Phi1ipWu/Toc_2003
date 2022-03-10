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



#include "TileFile.h"




//////////////////////////////////////////////////////////////////
//
//  ���ܣ�Ĭ�Ϲ��캯��
//  ���ڣ�2005-09-02
//
CTileFile::CTileFile()
{
    m_nGroundCount = 0;
    m_pGroundPos   = 0;
    m_szFileName   = 0;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ�������������������
//  ���ڣ�2005-09-02
//
CTileFile::~CTileFile()
{
    Close();
}




//////////////////////////////////////////////////////////
//
//  ���ܣ���ȡ�ļ�ͷ�ϵ���Ϣ�������������������ݿ�ʼλ��
//  ������const char* szFileName [in]  �ļ�·��
//  ���ڣ�2005-09-02
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
long CTileFile::Open(const char* szFileName)
{
    if(m_szFileName)
    {
        //  ����ļ���һ���������ٴδ�
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
    //  1. �ȴ򿪴��򿪵��ļ����򿪷�ʽΪֻ��
    //
    if(0 == (fp = fopen(szFileName, "rb")))
    {
        return 0L;
    }


    //
    //  2.��������ļ���ͷ�����ֽڣ��ж��Ƿ��� TF
    //
    if(! CheckFileHead(fp))
    {
        //  �ļ�ͷȷ��ʧ��
        fclose(fp);
        return 0L;
    }


    //
    //  3.  ��ȡ�ļ���Ϣ�������汾��Ϣ������λ���Լ�����Ҫ�ĳ�������
    //
    if(! GetFileInfo(fp))
    {
        //  ��ȡ�ļ���Ϣʧ��
        fclose(fp);
        return 0L;
    }


    //
    //  4.  ��ȡȫ�������������ļ��еĿ�ͷλ��
    //
    if(! GetDataPos(fp))
    {
        //  ��ȡȫ������λ��ʧ��
        fclose(fp);
        return 0L;
    }


    //
    //  5.  �����ļ�·��
    //
    long nLen = (long) strlen(szFileName);
    m_szFileName = new char[nLen + 1];
    strcpy(m_szFileName, szFileName);
    m_szFileName[nLen] = 0;

    //  �ļ�����ͷ��ȡ���
    //  �ر��ļ�
    fclose(fp);

    return 1L;
}




//////////////////////////////////////////////////////////
//
//  ���ܣ��رյ�ǰ�ļ�������ճ�������
//  ������FILE* fp [in]  �ļ�ָ��
//  ���ڣ�2005-09-02
//
void CTileFile::Close()
{
    if(m_pGroundPos)
    {
        //  ��ȫɾ����ų�������ͷ��̬����
        delete[] m_pGroundPos;
        m_pGroundPos = 0;
    }

    if(m_szFileName)
    {
        //  ��ȫɾ������ļ�����̬����
        delete[] m_szFileName;
        m_szFileName = 0;
    }

    //  ������������
    m_nGroundCount = 0;
}




//////////////////////////////////////////////////////////
//
//  ���ܣ��Ӵ���ļ��ж�ȡָ���ĳ�������
//  ������LPGROUND pGround [in]  ����ָ��
//        long nID         [in]  ���ݱ��
//  ���ڣ�2005-09-06
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
//  ע��nID ���԰��� 0���ļ������Ѿ���
//
long CTileFile::ReadGroundData(LPGROUND pGround, long nID)
{
    if(0 == pGround || 0 == m_pGroundPos || 0 > nID || nID >= m_nGroundCount)
    {
        //  ָ�벻�Ϸ������߳��� ID ���Ϸ�
        return 0L;
    }

    FILE* fp;

    unsigned long nLine,    //  ����
                  nColumn;  //  ����

    //  ���ļ�
    if(0 == (fp = fopen(m_szFileName, "rb")))
    {
        return 0L;
    }

    //  ���ļ��α��Ƶ����ݿ�ʼ��
    if(! SeekToDataPos(fp, nID))
    {
        fclose(fp);
        return 0L;
    }

    //  �������ͷ��0 - 1��
    if(! CheckDataHead(fp))
    {
        fclose(fp);
        return 0L;
    }

    //  ��ȡͼ�ؿ�ţ�2 - 3��
    if(! GetWordValue(fp, pGround->nTile))
    {
        fclose(fp);
        return 0L;
    }
    
    //  ��ȡ�������֣�4 - 5��
    if(! GetWordValue(fp, pGround->nBGM))
    {
        fclose(fp);
        return 0L;
    }

    //  ��ȡ������������6 - 7��
    if(! GetWordValue(fp, nLine))
    {
        fclose(fp);
        return 0L;
    }

    //  ��ȡ������������8 - 9��
    if(! GetWordValue(fp, nColumn))
    {
        fclose(fp);
        return 0L;
    }

    //  ���ó������ݴ�С
    if(! pGround->mtAreas.SetSize(nLine, nColumn))
    {
        fclose(fp);
        return 0L;
    }

    //  ������ݣ�0x0D - ??��
    Area32x32 oArea;
    long nCount = nLine * nColumn;
    for(long i = 0; i < nCount; ++i)
    {
        //  ��ȡ��һ����
        if(! ReadArea(fp, oArea))
        {
            pGround->mtAreas.Delete();
            fclose(fp);
            return 0L;
        }

        pGround->mtAreas.Fill(&oArea);
    }

    //  �������β
    if(! CheckDataEnd(fp))
    {
        pGround->mtAreas.Delete();
        fclose(fp);
        return 0L;
    }

    //  ��������û�г���
    fclose(fp);
    return 1L;
}




//////////////////////////////////////////////////////////
//
//  ���ܣ���ȡ���������ļ�������
//  ������char* szFileName [in]  �ļ�·��
//        LPGROUND pGround [in]  ����ָ��
//  ���ڣ�2005-09-19
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
long CTileFile::ReadDataFile(char* szFileName, LPGROUND pGround)
{
    if(0 == szFileName || 0 == pGround)
    {
        //  ָ�벻�Ϸ�
        return 0L;
    }

    FILE* fp;

    unsigned long nLine,    //  ����
                  nColumn;  //  ����

    //  ���ļ�
    if(0 == (fp = fopen(szFileName, "rb")))
    {
        return 0L;
    }

    //  �������ͷ��0 - 1��
    if(! CheckDataHead(fp))
    {
        fclose(fp);
        return 0L;
    }

    //  ��ȡͼ�ؿ�ţ�2 - 3��
    if(! GetWordValue(fp, pGround->nTile))
    {
        fclose(fp);
        return 0L;
    }
    
    //  ��ȡ�������֣�4 - 5��
    if(! GetWordValue(fp, pGround->nBGM))
    {
        fclose(fp);
        return 0L;
    }

    //  ��ȡ������������6 - 7��
    if(! GetWordValue(fp, nLine))
    {
        fclose(fp);
        return 0L;
    }

    //  ��ȡ������������8 - 9��
    if(! GetWordValue(fp, nColumn))
    {
        fclose(fp);
        return 0L;
    }

    //  ���ó������ݴ�С
    if(! pGround->mtAreas.SetSize(nLine, nColumn))
    {
        fclose(fp);
        return 0L;
    }

    //  �������
    Area32x32 oArea;
    long nCount = nLine * nColumn;
    for(long i = 0; i < nCount; ++i)
    {
        //  ��ȡ��һ����
        if(! ReadArea(fp, oArea))
        {
            pGround->mtAreas.Delete();
            fclose(fp);
            return 0L;
        }

        pGround->mtAreas.Fill(&oArea);
    }

    //  �������β
    if(! CheckDataEnd(fp))
    {
        pGround->mtAreas.Delete();
        fclose(fp);
        return 0L;
    }

    //  ��������û�г���
    fclose(fp);
    return 1L;
}




//////////////////////////////////////////////////////////
//
//  ���ܣ�дȡ������������
//  ������const char* szFileName [in] Ҫд����ļ���
//        const LPGROUND pGround [in] ����ָ��
//  ���ڣ�2005-09-14
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
//  ע��nID ���԰��� 0
//
long CTileFile::WriteGroundData(const char* szFileName, const LPGROUND pGround)
{
    //  �ж�ָ���Ƿ���Ч
    if(0 == szFileName || 0 == pGround)
    {
        return 0L;
    }
/*
    //  ���ݲ��Ϸ�
    if(! pGround->mtAreas.IsValid())
    {
        return 0L;
    }
*/
    FILE* fp;

    //
    //  ���ļ�
    //
    if(0 == (fp = fopen(szFileName, "wb+")))
    {
        return 0L;
    }

    unsigned char szBuffer[8] = {0};

    //
    //  д����ͷ DH ���
    //
    szBuffer[0] = 'D';
    szBuffer[1] = 'H';
    fwrite(szBuffer, sizeof(unsigned char), 2, fp);

    //
    //  д����Ϣ������ͼ�ر�ţ����ֱ�ţ�����������
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
    //  д��ȫ��������
    //
    long nBufferSize;
    Area32x32** pAreas = pGround->mtAreas.GetDataRef();
    for(long i = 0; i < pGround->mtAreas.GetLineCount(); ++i)
    {
        for(long j = 0; j < pGround->mtAreas.GetColumnCount(); ++j)
        {
        	//	����������ͷ
            nBufferSize = 2;
            szBuffer[0] = (unsigned char)((pAreas[i][j].nAreaInfo >> 8) & 0xFF);
            szBuffer[1] = (unsigned char)((pAreas[i][j].nAreaInfo) & 0xFF);
            ///szBuffer[1] = 0;

			//	������ש
			///if(pGround->mtAreas[i][j].stTile[0].change) szBuffer[1] |= 0x04;
			///if(pGround->mtAreas[i][j].stTile[1].change) szBuffer[1] |= 0x02;
			///if(pGround->mtAreas[i][j].stTile[2].change) szBuffer[1] |= 0x01;

            //  ��һ����
            if(0 <= pAreas[i][j].stTile[0].line && 0 <= pAreas[i][j].stTile[0].column)
            {
                szBuffer[1] |= 0x40;

                if(0 >= j || pAreas[i][j].stTile[0].line != pAreas[i][j - 1].stTile[0].line ||
                    pAreas[i][j].stTile[0].column != pAreas[i][j - 1].stTile[0].column)
                {
                    //  ��֮ǰһ������Ĳ�һ���ݲ�ͬ
                    szBuffer[1] |= 0x80;
                    szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[0].column);
                    szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[0].line);
                }

            }

            //  �������
            if(0 <= pAreas[i][j].stTile[1].line && 0 <= pAreas[i][j].stTile[1].column)
            {
                szBuffer[1] |= 0x20;
                szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[1].column);
                szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[1].line);
            }

            //  ��������
            if(0 <= pAreas[i][j].stTile[2].line && 0 <= pAreas[i][j].stTile[2].column)
            {
                szBuffer[1] |= 0x10;
                szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[2].column);
                szBuffer[nBufferSize ++] = (unsigned char)(pAreas[i][j].stTile[2].line);
            }

            fwrite(szBuffer, sizeof(unsigned char), nBufferSize, fp);

            //  ê������
            if((szBuffer[0] & 0x40) == 0x40)
            {
                szBuffer[2] = (unsigned char)((pAreas[i][j].nAnchor >> 8) & 0xFF);
                szBuffer[3] = (unsigned char) (pAreas[i][j].nAnchor & 0xFF);
                fwrite(&szBuffer[2], sizeof(unsigned char), 2, fp);
            }
        }
    }

    //
    //  д����β DE ���
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
//  ���ܣ�����������ĳ����ļ�д��һ���ļ����Թ���Ϸ�ж�ȡ
//  ������const char* szTileFileName [in]  ��ƴ�Ӻ���ļ�
//        char** pDataFileNames      [in]  ȫ����ƴ�ӵ��ļ�
//        const long nCount          [in]  ��ƴ���ļ��ĸ���
//  ���ڣ�2005-09-15
//  ����ֵ���ɹ�ƴ���ļ��ĸ���    0 - ʧ��
//
long CTileFile::LinkTileFile(const char* szTileFileName, const char* const* pDataFileNames, const long nCount)
{
    if(0 == szTileFileName || 0 == pDataFileNames || 0 >= nCount)
    {
        return 0L;
    }

    FILE* fpWrite;
    FILE* fpRead;

    //  ���ļ����ݵĻ��壬һ�ο��Ը��� 1K ����
    unsigned char szBuffer[1024];

    //  һ�ζ�ȡ���ֽ���
    long nReadBufLength = 0;
    
    //  �����ļ�ͷ��λ��
    long nBeforeLength  = 8 + nCount * 4;
    
    //  �����ʱ���ļ�ͷλ�õ�����
    long* pDataPos = 0;
    long i;

    //
    //  �򿪴�д����ļ�����д��ʽ��
    //
    if(0 == (fpWrite = fopen(szTileFileName, "wba+")))
    {
        return 0L;
    }

    //
    //  ��д���ļ�ͷ��Ϣ
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
    //  д��յ�����ͷλ��
    //
    memset(szBuffer, 0, sizeof(szBuffer));
    for(i = 0; i < nCount; ++i)
    {
        fwrite(szBuffer, sizeof(unsigned char), 4, fpWrite);
    }

    //
    //  ���ݶԿ�
    //
    try
    {
        //  ����ļ�ǰ�������ݿ�ʼλ��
        pDataPos = new long[nCount];
        memset(pDataPos, 0, sizeof(long) * nCount);

        for(i = 0; i < nCount; ++i)
        {
            //  �ȱ������ݿ�ʼλ��
            pDataPos[i] = nBeforeLength;

            //  ���ļ�
            if(0 == (fpRead = fopen(pDataFileNames[i], "rb")))
            {
                //  ���ļ�ʧ�ܣ�����ͷλ���� 0��������ȡ��һ���ļ�
                pDataPos[i] = 0;
                continue;
            }

            //  �ļ����ƣ�������������д��
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
        //  ��ȡ�����г����ر��ļ��������ѷ����ڴ�
        fclose(fpWrite);
        if(pDataPos)
        {
            delete[] pDataPos;
        }
        return 0L;
    }


    //
    //  ��λ������λ��ͷ��Ҳ���ǵ� 8 ���ֽ�
    //  д������ͷλ��
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
    //  д�ļ��ɹ�
    //  �ļ��Ĺرգ��ѷ����ڴ���ͷ�
    //
    fclose(fpWrite);
    delete[] pDataPos;

    return 1L;
}




//////////////////////////////////////////////////////////
//
//  ���ܣ�����ļ�ͷ�����ͨ������ 1����ͨ������ 0
//  ������FILE* fp [in]  �ļ�ָ��
//  ���ڣ�2005-09-02
//  ����ֵ��1 - �ɹ�    0 - ʧ��
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
        //  �ж��ļ�ͷ�Ƿ��� TF
        if('T' == szBuffer[0] && 'F' == szBuffer[1])
        {
            return 1L;
        }
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  ���ܣ���ȡ�ļ���Ϣ�����ͨ������ 1����ͨ������ 0
//  ������FILE* fp [in]  �ļ�ָ��
//  ���ڣ�2005-09-02
//  ����ֵ��1 - �ɹ�    0 - ʧ��
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
//  ���ܣ���ȡȫ��������ͷλ��
//  ������FILE* fp [in]  �ļ�ָ��
//  ���ڣ�2005-09-02
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
long CTileFile::GetDataPos(FILE* fp)
{
    if(! fp || 0 >= m_nGroundCount)
    {
        //  �����ͼ������Ϊ�������Ļ�ֱ�ӷ���
        return 0L;
    }

    //  �ж�����ͷ�Ƿ��Ѿ���ȡ������Ѷ�ȡ�����ͷ�ԭ�����ڴ�
    if(m_pGroundPos)
    {
        delete[] m_pGroundPos;
        m_pGroundPos = 0;
    }

    //  ���·�������ͷ�Ĵ�С
    m_pGroundPos = new unsigned long[m_nGroundCount];

    for(long i = 0; i < m_nGroundCount; ++i)
    {
        //  ��ȡ�ĸ��ֽڵ�����ͷλ��
        if(GetDWordValue(fp, m_pGroundPos[i]))
        {
            //  �ж�����ͷλ���Ƿ�Ϸ�
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
//  ���ܣ���λ���ļ���ǰλ��
//  ������FILE* fp [in]  �ļ�ָ��
//  ���ڣ�2005-09-07
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
//  ע��nID ���ܰ��� 0
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
//  ���ܣ��������ͷ�����ͨ������ 1����ͨ������ 0
//  ������FILE* fp [in]  �ļ�ָ��
//  ���ڣ�2005-09-07
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
long CTileFile::CheckDataHead(FILE* fp)
{
    char szBuffer[2];

    if(2 == fread(szBuffer, sizeof(char), 2, fp))
    {
        //  �ж�����ͷ�Ƿ��� DH (DataHead)
        if('D' == szBuffer[0] && 'H' == szBuffer[1])
        {
            return 1L;
        }
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  ���ܣ��������β�����ͨ������ 1����ͨ������ 0
//  ������FILE* fp [in]  �ļ�ָ��
//  ���ڣ�2005-09-07
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
long CTileFile::CheckDataEnd(FILE* fp)
{
    char szBuffer[2];

    if(2 == fread(szBuffer, sizeof(char), 2, fp))
    {
        //  �ж�����ͷ�Ƿ��� DH (DataEnd)
        if('D' == szBuffer[0] && 'E' == szBuffer[1])
        {
            return 1L;
        }
    }

    return 0L;
}




//////////////////////////////////////////////////////////
//
//  ���ܣ���ȡ��ͼ�Ĵ�С�����ͨ������ 1����ͨ������ 0
//  ������FILE* fp     [in]      �ļ�ָ��
//        long& line   [in out]  ����������
//        long& column [in out]  ����������
//  ���ڣ�2005-09-07
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
long CTileFile::GetGroundSize(FILE* fp, long& line, long& column)
{
    unsigned char szBuffer[4];

    //  ��ȡ����������
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
//  ���ܣ���ȡ��һ�� 32x32 �������������
//  ������FILE* fp         [in]      �ļ�ָ��
//        Area32x32& oArea [in out]  �������������
//  ���ڣ�2005-09-07
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
long CTileFile::ReadArea(FILE* fp, Area32x32& oArea)
{
    if(! fp)
    {
        return 0L;
    }

    if(! GetWordValue(fp, oArea.nAreaInfo)) return 0L;

    //  ��ȡ��һ������
    if(oArea.nAreaInfo & 0x40)
    {
        //  ��ʾ��ǰ�Ĳ�һ�����ݺ�֮ǰ��һ��
        if(oArea.nAreaInfo & 0x80)
        {
            if(! GetByteValue(fp, oArea.stTile[0].column)) return 0L;
            if(! GetByteValue(fp, oArea.stTile[0].line))   return 0L;
        }

		//	��������
		///oArea.stTile[0].change = oArea.nAreaInfo & 0x04 ? 4 : 0;
    }
    else
    {
        oArea.stTile[0].line   = -1;
        oArea.stTile[0].column = -1;
		///oArea.stTile[0].change = 0;
    }

    //  ��ȡ���������
    if(oArea.nAreaInfo & 0x20)
    {
        if(! GetByteValue(fp, oArea.stTile[1].column)) return 0L;
        if(! GetByteValue(fp, oArea.stTile[1].line))   return 0L;

		//	��������
		///oArea.stTile[1].change = oArea.nAreaInfo & 0x02 ? 4 : 0;
	}
    else
    {
        oArea.stTile[1].line   = -1;
        oArea.stTile[1].column = -1;
		///oArea.stTile[1].change = 0;
    }

    //  ��ȡ����������
    if(oArea.nAreaInfo & 0x10)
    {
        if(! GetByteValue(fp, oArea.stTile[2].column)) return 0L;
        if(! GetByteValue(fp, oArea.stTile[2].line))   return 0L;

		//	��������
		///oArea.stTile[2].change = oArea.nAreaInfo & 0x01 ? 4 : 0;
    }
    else
    {
        oArea.stTile[2].line   = -1;
        oArea.stTile[2].column = -1;
		///oArea.stTile[2].change = 0;
    }

    //  ��ȡê����
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
//  ���ܣ���ȡ�ĸ��ֽڣ�˫�֣�����ת������ֵ
//  ������FILE* fp              [in]      �ļ�ָ��
//        unsigned long& nValue [in out]  ��ֵ
//  ���ڣ�2005-09-13
//  ����ֵ��1 - �ɹ�    0 - ʧ��
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
//  ���ܣ���ȡ�����ֽڣ����֣�����ת������ֵ
//  ������FILE* fp     [in]      �ļ�ָ��
//        long& nValue [in out]  ��ֵ
//  ���ڣ�2005-09-13
//  ����ֵ��1 - �ɹ�    0 - ʧ��
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
//  ���ܣ���ȡ�����ֽڣ����֣�����ת������ֵ
//  ������FILE* fp              [in]      �ļ�ָ��
//        unsigned long& nValue [in out]  ��ֵ
//  ���ڣ�2005-09-13
//  ����ֵ��1 - �ɹ�    0 - ʧ��
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
//  ���ܣ���ȡһ���ֽڣ��ֽڣ�����ת������ֵ
//  ������FILE* fp     [in]      �ļ�ָ��
//        long& nValue [in out]  ��ֵ
//  ���ڣ�2005-09-13
//  ����ֵ��1 - �ɹ�    0 - ʧ��
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
//  ���ܣ���ȡһ���ֽڣ��ֽڣ�����ת������ֵ
//  ������FILE* fp              [in]      �ļ�ָ��
//        unsigned long& nValue [in out]  ��ֵ
//  ���ڣ�2005-09-13
//  ����ֵ��1 - �ɹ�    0 - ʧ��
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
//  ���ܣ���ȡһ���ֽڣ��ֽڣ�����ת������ֵ
//  ������FILE* fp              [in]      �ļ�ָ��
//        short& nValue [in out]  ��ֵ
//  ���ڣ�2005-09-13
//  ����ֵ��1 - �ɹ�    0 - ʧ��
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




//  ����̨ʹ�ã��������һЩ������Ϣ
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
//  ��������������������ļ�ƴ��
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
//  ���Զ�ȡЧ��
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
//  ����һ���Զ����С�ĳ����ļ���������ɲ������
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
