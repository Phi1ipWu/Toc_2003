//---------------------------------
//	类名：	CGround
//	描述：	场景地板类
//	模式：	单例
//---------------------------------


#include "CGround.h"
#include "..\DxWrapper\DMusic.h"
#include "..\DxWrapper\DInput.h"
using namespace TocCore;
using namespace DxWrapper;

CGround* CGround::m_pGround = 0;


bool CGround::InitInstance()
{
	CPc* pPc = CPc::Instance();
	if(0 == pPc)
	{
		return false;
	}

	if(pPc)
	{
		m_pGround = new CGround(pPc);
		return true;
	}
	else
	{
		return false;
	}
}


void CGround::ExitInstance()
{
	if(m_pGround)
	{
		delete m_pGround;
		m_pGround = 0;
	}
}


CGround* CGround::Instance()
{
	return m_pGround;
}



//	2005-09-24
bool CGround::ChangeByAnchor(const long& nAnchor)
{
	if(0 > nAnchor)
	{
		//	不用转换锚点
		return true;
	}

	FILE* fp;
	string strFileName  = CSetting::Instance()->GetDataPath() + "TileAnchor.toc";
	
	//	打开文件
	if(0 == (fp = fopen(strFileName.c_str(), "rb")))
	{
		return false;
	}

	//	定位到对应编号的锚点数据开始位置
	if(0 != fseek(fp, nAnchor << 3, SEEK_CUR))
	{
		return false;
	}

	//	读取锚点数据
	unsigned char szBuffer[8];
	if(8 != fread(szBuffer, sizeof(unsigned char), sizeof(szBuffer), fp))
	{
		fclose(fp);
		return false;
	}

	fclose(fp);

	//	分析锚点数据头是否合法
	if(0xFF != szBuffer[0])
	{
		return false;
	}

	//	解析数据
	long nFace = szBuffer[1];
	long nGdID = (szBuffer[2] << 8) | szBuffer[3];
	long nX    = (szBuffer[4] << 8) | szBuffer[5];
	long nY    = (szBuffer[6] << 8) | szBuffer[7];

	//	设置角色面向以及所在坐标
	m_pPc->SetFace(nFace);
	m_pPc->SetDrawPos((nX << 5), (nY << 5) - 20);

	//	设置场景数据
	return SetGround(nGdID);
}


//	2005-09-24
//	返回锚点编号
long CGround::GetAnchor()
{
	RECT rect = m_pPc->GetCharRect();
	rect.top    >>= 5;
	rect.bottom >>= 5;
	rect.left   >>= 5;
	rect.right  >>= 5;

	//	左上点
	if(0 > m_stGround.mtAreas[rect.top][rect.left].nAnchor)
	{
		return -1;
	}

	//	左下点
	if(0 > m_stGround.mtAreas[rect.bottom][rect.left].nAnchor)
	{
		return -1;
	}

	//	右上点
	if(0 > m_stGround.mtAreas[rect.top][rect.right].nAnchor)
	{
		return -1;
	}

	//	右下点
	if(0 > m_stGround.mtAreas[rect.bottom][rect.right].nAnchor)
	{
		return -1;
	}

	//	判断角色踩的位置是不是全部都是同一锚点编号
	if(m_stGround.mtAreas[rect.top][rect.left].nAnchor  == m_stGround.mtAreas[rect.bottom][rect.left].nAnchor &&
	   m_stGround.mtAreas[rect.top][rect.left].nAnchor  == m_stGround.mtAreas[rect.top][rect.right].nAnchor &&
	   m_stGround.mtAreas[rect.top][rect.right].nAnchor == m_stGround.mtAreas[rect.bottom][rect.right].nAnchor)
	{
		return m_stGround.mtAreas[rect.top][rect.left].nAnchor;
	}

	return -1;
}



//	2005-09-24
bool CGround::SetGround(const long& nGroundID)
{
	long nTile = m_stGround.nTile;

	string strFileName = CSetting::Instance()->GetDataPath() + "TileData.toc";
	if(! m_TileFile.Open(strFileName.c_str()))
	{
		return false;
	}

	if(! m_TileFile.ReadGroundData(&m_stGround, nGroundID))
	{
		return false;
	}

	m_nLineCnt   = m_stGround.mtAreas.GetLineCount();
	m_nColumnCnt = m_stGround.mtAreas.GetColumnCount();

	CDMusic::Instance()->PlayBGM(m_stGround.nBGM);

	//	刷新绘图时左上角的位置
	RefreshTopLeft();


	if(0 < m_stGround.nTile && m_stGround.nTile <= 50 && nTile != m_stGround.nTile)
		CDDraw::Instance()->TermSurface(m_stGround.nTile);

	return CDDraw::Instance()->InitSurface(m_stGround.nTile);
/*
	//	添加 NPC，单元测试用
	CChar CharObj(51);
	srand(GetTickCount());
	for(long i = 0; i < 10; ++i)
	{
		CharObj.SetDrawPos(rand() % (m_nColumnCnt * 32 - 32), rand() % (m_nLineCnt * 32 - 48));
		m_naChar.push_back(CharObj);
	}
*/
	return true;
/*
	//	单元测试用
	this->m_nTop = 16;
	this->m_nLeft = 64;

	//	以下代码为单元测试用
	m_nLineCnt = 300;
	m_nColumnCnt = 380;
	CTile tile;
	tile.SetType(CDDraw::Ground);
	tile.SetNo(43);

	m_stGround.mtAreas.clear();
	TileArray ta;
	long i, j;

	for(i = 0; i < m_nLineCnt; ++ i)
	{
		ta.clear();
		for(j = 0; j < m_nColumnCnt; ++ j)
		{
			tile.SetColumn(j % 8);
			tile.SetLine(i % 33);
			ta.push_back(tile);
		}

		m_stGround.mtAreas.push_back(ta);
	}

	if(m_stGround.mtAreas.size() != m_nLineCnt) return false;
	if(ta.size() != m_nColumnCnt) return false;

	return true;
*/
}


//	5/14
void CGround::OnBlt()
{
	long i, j;//, nLen;
	long nLnCnt, nColCnt;	//	获取从开始绘图的行列起需要绘图的行列数
	long nX, nY;			//	能在屏幕上绘图的最左上角的那块图素的左上点
	long nLn, nCol;			//	能在屏幕上绘图的最左上角的那块图素
	POINT ptPos;			//	游戏角色的位置
	CDDraw* pDraw;

	pDraw = CDDraw::Instance();

	//if(0 == pDraw) return;

	//	刷新绘图时左上顶点
	RefreshTopLeft();

	//	计算出最上面需要绘制的地砖序号
	if(m_nTop >= 0)
	{
		nY  = m_nTop;
		nLn = 0;
	}
	else
	{
//		nY  = abs(m_nTop) % 32 * -1;
//		nLn = (nY - m_nTop) / 32;
		nY  = (abs(m_nTop) & 31) * -1;
		nLn = ((nY - m_nTop) >> 5);
	}

	//	计算出最左面需要绘制的地砖序号
	if(m_nLeft >= 0)
	{
		nX   = m_nLeft;
		nCol = 0;
	}
	else
	{
//		nX   = abs(m_nLeft) % 32 * -1;
//		nCol = (nX - m_nLeft) / 32;
		nX   = (abs(m_nLeft) & 31) * -1;
		nCol = (nX - m_nLeft) >> 5;
	}

	//	获得需要绘制地砖行和列的最右面和最下面
//	nLnCnt  = m_nScreenHeight / 32 + 1 + nLn;
//	nColCnt = m_nScreenWidth  / 32 + 1 + nCol;

	//	2005-09-30 考虑到可能分辨率不是 32 的倍数则改成 + 2
	nLnCnt  = (m_nScreenHeight >> 5) + 2 + nLn;
	nColCnt = (m_nScreenWidth  >> 5) + 2 + nCol;

	++nLnCnt;
//	if(nLn  + nLnCnt  > m_nLineCnt)   nLnCnt  -= nLn  + nLnCnt  - m_nLineCnt   - 1;
//	if(nCol + nColCnt > m_nColumnCnt) nColCnt -= nCol + nColCnt - m_nColumnCnt - 1;

	long nNowCnt = timeGetTime();
	nNowCnt >>= 7;
	nNowCnt &= 3;

	//	先绘制层一和层二
	for(i = nLn; i < nLnCnt; ++ i)
	{
		for(j = nCol; j < nColCnt; ++ j)
		{
			if(i < m_nLineCnt && j < m_nColumnCnt)
			{
//				m_stGround.mtAreas[i][j].DrawTile(j * 32 + m_nLeft, i * 32 + m_nTop, 1);
//				m_stGround.mtAreas[i][j].DrawTile(j * 32 + m_nLeft, i * 32 + m_nTop, 2);
//				m_stGround.mtAreas[i][j].DrawTile((j << 5) + m_nLeft, (i << 5) + m_nTop, 1);
//				m_stGround.mtAreas[i][j].DrawTile((j << 5) + m_nLeft, (i << 5) + m_nTop, 2);

//				pDraw->BltTile(m_stGround.nTile, m_stGround.mtAreas[i][j].stTile[0].line, m_stGround.mtAreas[i][j].stTile[0].column, (j << 5) + m_nLeft, (i << 5) + m_nTop);
//				pDraw->BltTile(m_stGround.nTile, m_stGround.mtAreas[i][j].stTile[1].line, m_stGround.mtAreas[i][j].stTile[1].column, (j << 5) + m_nLeft, (i << 5) + m_nTop);

				//	判定层一是否是动画地砖
				///if(m_stGround.mtAreas[i][j].stTile[0].change)
				if(m_stGround.mtAreas[i][j].nAreaInfo & 0x04)
					pDraw->BltTile(m_stGround.nTile, m_stGround.mtAreas[i][j].stTile[0].column + nNowCnt, m_stGround.mtAreas[i][j].stTile[0].line, (j << 5) + m_nLeft, (i << 5) + m_nTop);
				else
					pDraw->BltTile(m_stGround.nTile, m_stGround.mtAreas[i][j].stTile[0].column, m_stGround.mtAreas[i][j].stTile[0].line, (j << 5) + m_nLeft, (i << 5) + m_nTop);

				//	判定层二是否是动画地砖
				///if(m_stGround.mtAreas[i][j].stTile[1].change)
				if(m_stGround.mtAreas[i][j].nAreaInfo & 0x02)
					pDraw->BltTile(m_stGround.nTile, m_stGround.mtAreas[i][j].stTile[1].column + nNowCnt, m_stGround.mtAreas[i][j].stTile[1].line, (j << 5) + m_nLeft, (i << 5) + m_nTop);
				else
					pDraw->BltTile(m_stGround.nTile, m_stGround.mtAreas[i][j].stTile[1].column, m_stGround.mtAreas[i][j].stTile[1].line, (j << 5) + m_nLeft, (i << 5) + m_nTop);
			}
		}
	}

	//	单元测试用，绘制角色
	ptPos = m_pPc->GetDrawPos();
	ptPos.x += m_nLeft;
	ptPos.y += m_nTop;

	RECT rc = m_pPc->GetTileRect();
	pDraw->Blt(m_pPc->GetTileNo(), &rc, ptPos.x, ptPos.y);
/*
	//	先绘制下半身，因为有可能会被挡住
	RECT r;
	r = m_pPc->GetTileRect();
	r.top += 24;
	pDraw->BltChar(m_pPc->GetTileNo(), ptPos.x, ptPos.y + 24, r);

	//	绘制 NPC
	nLen = (long)m_naChar.size();
	for(i = 0; i < nLen; ++i)
	{
		ptPos = m_naChar[i].GetDrawPos();
		ptPos.x += m_nLeft;
		ptPos.y += m_nTop;

		pDraw->BltChar(m_naChar[i].GetTileNo(), ptPos.x, ptPos.y, m_naChar[i].GetTileRect());
	}

	r.top -= 24;
	r.bottom -= 24;
	ptPos = m_pPc->GetDrawPos();
	ptPos.x += m_nLeft;
	ptPos.y += m_nTop;
	pDraw->BltChar(m_pPc->GetTileNo(), ptPos.x, ptPos.y, r);
*/
	//	最后绘制层三
	for(i = nLn; i < nLnCnt; ++ i)
	{
		for(j = nCol; j < nColCnt; ++ j)
		{
			if(i < m_nLineCnt && j < m_nColumnCnt)
			{
//				m_stGround.mtAreas[i][j].DrawTile(j * 32 + m_nLeft, i * 32 + m_nTop, 3);
//				m_stGround.mtAreas[i][j].DrawTile((j << 5) + m_nLeft, (i << 5) + m_nTop, 3);
//				pDraw->BltTile(m_stGround.nTile, m_stGround.mtAreas[i][j].stTile[2].line, m_stGround.mtAreas[i][j].stTile[2].column, (j << 5) + m_nLeft, (i << 5) + m_nTop);

				//	判定层二是否是动画地砖
				///if(m_stGround.mtAreas[i][j].stTile[2].change)
				if(m_stGround.mtAreas[i][j].nAreaInfo & 0x01)
					pDraw->BltTile(m_stGround.nTile, m_stGround.mtAreas[i][j].stTile[2].column + nNowCnt, m_stGround.mtAreas[i][j].stTile[2].line, (j << 5) + m_nLeft, (i << 5) + m_nTop);
				else
					pDraw->BltTile(m_stGround.nTile, m_stGround.mtAreas[i][j].stTile[2].column, m_stGround.mtAreas[i][j].stTile[2].line, (j << 5) + m_nLeft, (i << 5) + m_nTop);
			}
		}
	}

	//	绘制半透明方块
	if(m_isLock) this->DrawBox();
/*
	if(m_bText)
	{
		pDraw->BltText(m_pStr, 0, 0);
	}

	//	显示角色位置（速度明显变慢）
	ptPos = m_pPc->GetDrawPos();
	char a[100] = {0};
	sprintf(a, "x = %d   y = %d", ptPos.x, ptPos.y);  
	CDDraw::Instance()->BltText(a, 0, 0);
*/
}



void CGround::OnIdle()
{
	//	接受角色的移动和处理是否可以移动
	m_pPc->FreeDo();
	m_pPc->Move(IsRectValid(m_pPc->GetNewCharRect()));

	//	判定是否要切换场景
	ChangeByAnchor(GetAnchor());

	//	脚柄代码的位置要改
/*
	//	CDInput 类里的测试脚柄代码（调查）
	if(CDInput::Instance()->IsJoyPress(1))
	{
		this->OnCheck();
	}

	//	CDInput 类里的测试脚柄代码（退出）
	if(CDInput::Instance()->IsJoyPress(2))
	{
		this->OnEscape();
	}
*/
	long i;
	for(i = 0; i < (long)m_naChar.size(); ++i)
	{
		m_naChar[i].FreeDo();
	}
}


//	判定玩家控制角色所占区域是否合法
bool CGround::IsRectValid(const RECT& rect)
{
	//	单元测试用
	//return true;

	//	角色被锁定
	//if(m_isLock) return false;

	//	先判定玩家控制角色是否已经走出场景边界
//	if(rect.left < 0 || rect.top < 0 || rect.bottom >= m_nLineCnt * 32 || rect.right >= m_nColumnCnt * 32)
	if(rect.left < 0 || rect.top < 0 || rect.bottom >= m_nLineCnt << 5 || rect.right >= m_nColumnCnt << 5)
	{
		//	以后可以在此加入力回馈功能
		return false;
	}

	if((m_stGround.mtAreas[rect.top    >> 5][rect.left  >> 5].nAreaInfo & 0x8000) == 0x8000) return false;
	if((m_stGround.mtAreas[rect.top    >> 5][rect.right >> 5].nAreaInfo & 0x8000) == 0x8000) return false;
	if((m_stGround.mtAreas[rect.bottom >> 5][rect.left  >> 5].nAreaInfo & 0x8000) == 0x8000) return false;
	if((m_stGround.mtAreas[rect.bottom >> 5][rect.right >> 5].nAreaInfo & 0x8000) == 0x8000) return false;
/*
	if((m_stGround.mtAreas[rect.top    >> 5][rect.left  >> 5].stTile[2].IsValid()) &&
	   (m_stGround.mtAreas[rect.bottom >> 5][rect.left  >> 5].stTile[3].IsValid()))   return false;

	if((m_stGround.mtAreas[rect.top    >> 5][rect.right >> 5].stTile[2].IsValid()) &&
	   (m_stGround.mtAreas[rect.bottom >> 5][rect.right >> 5].stTile[3].IsValid()))   return false;
*/

	long i, nLen;
	nLen = (long)m_naChar.size();

	//	再与 NPC 比较
	for(i = 0; i < nLen; ++i)
	{
		if(false == m_naChar[i].IsRectValid(rect))
		{
			return false;
		}
	}

	return true;
}


void CGround::OnEscape()
{
	//MessageBox(0, "CGround.cpp  Ln443", 0, MB_OK);
	CGame::Instance()->Quit();
}


void CGround::OnCheck()
{
	m_isLock = !m_isLock;

	//	单元测试用
//	this->ChangeByAnchor(0);
//	return;

/*
	//	老的代码，用来测试 NPC
	long i, nLen;
	POINT pt = m_pPc->GetCheckPoint();

	nLen = (long)m_naChar.size();
	for(i = 0; i < nLen; ++i)
	{
		if(false == m_naChar[i].IsPointValid(pt))
		{
			break;
		}
	}

	if(i < nLen)
	{
		sprintf(m_pStr, "NPC编号： %d", i + 1);
		m_bText = !m_bText;
	}
*/
}


void CGround::RefreshTopLeft()
{
	POINT ptPos;					//	角色现在在场景中的左上角点坐标
	long nWidth, nHeight;			//	当前场景的大小（单位：像素）

	nWidth  = m_nColumnCnt << 5;
	nHeight = m_nLineCnt   << 5;

	//	获取绝对坐标（可能会大于屏幕大小）
	ptPos = m_pPc->GetDrawPos();

	//	角色左上角坐标转换角色中心坐标
	ptPos.x += 16;
	ptPos.y += 24;

	if(nHeight <= m_nScreenHeight - 32)
	{
		//	场景上下过小，无需上下卷屏
		m_nTop = (m_nScreenHeight - nHeight) >> 1;
	}
	else
	{
		if(ptPos.y > nHeight - (m_nScreenHeight >> 1) + 16)
		{
			//	角色在屏幕在下边界处
			m_nTop = m_nScreenHeight - 16 - nHeight;
		}
		else if(ptPos.y < (m_nScreenHeight >> 1) - 16)
		{
			//	角色处在屏幕上边界处
			m_nTop = 16;
		}
		else
		{
			//	未到上下边界处
			m_nTop = (m_nScreenHeight >> 1) - ptPos.y;
		}
	}

	if((m_nColumnCnt << 5) <= m_nScreenWidth - 32)
	{
		//	场景左右过小，无需左右卷屏
		m_nLeft = (m_nScreenWidth - nWidth) >> 1;
	}
	else
	{
		if(ptPos.x > nWidth - (m_nScreenWidth >> 1) + 16)
		{
			m_nLeft = m_nScreenWidth - 16 - nWidth;
		}
		else if(ptPos.x < (m_nScreenWidth >> 1) - 16)
		{
			m_nLeft = 16;
		}
		else
		{
			m_nLeft = (m_nScreenWidth >> 1) - ptPos.x;
		}
	}
}


//	半透明框
//	2005-10-21
bool CGround::DrawBox()
{
	CDDraw* pDraw = CDDraw::Instance();
	LPDIRECTDRAWSURFACE7 pBackSurface = CDDraw::Instance()->GetBackSurface();

	float fBox  = 0.5;
	float fBack = 1 - fBox;

	long nX      = (long)(m_nScreenWidth  * 0.2);
	long nY      = (long)(m_nScreenHeight * 0.6);
	long nWidth  = (long)(m_nScreenWidth  * 0.6);
	long nHeight = (long)(m_nScreenHeight * 0.3);

	DDSURFACEDESC2 ddsd;
	memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);

	//	锁定后台面板全部区域
	if (FAILED(pBackSurface->Lock(0, &ddsd,
                   DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
                   NULL)))
	{
		return false;
	}

	unsigned char *pBuffer = (unsigned char *)ddsd.lpSurface;
	unsigned char nBkRed, nBkGreen, nBkBlue;	//	后台颜色（已绘制）
	unsigned char nBxRed, nBxGreen, nBxBlue;	//	前台颜色（蓝色）
	long nPos = 0;
	long i, j;

	nBxRed   = 0;
	nBxGreen = 0;
	nBxBlue  = 48;

	for(i = nX; i <= nX + nWidth; ++i)
	{
		for(j = nY; j <= nY + nHeight; ++j)
		{
//			nBxRed = (nHeight - j) / (nHeight >> 4) + 48;

			switch(pDraw->GetBPP())
			{

			//	16位色 RGB565 处理（不支持 RGB555）
			case 16:
				nPos = j * ddsd.lPitch + (i << 1);
				if(i == nX || i == nX + nWidth || j == nY || j == nY + nHeight)
				{
					pBuffer[nPos + 0] = 255;
					pBuffer[nPos + 1] = 255;
				}
				else
				{
					nBkRed   = ((pBuffer[nPos + 1] & 0xF8) >> 3) >> 1;
					nBkGreen = (((pBuffer[nPos + 1] & 0x07) << 3) | ((pBuffer[nPos + 0] & 0xE0) >> 5)) >> 1;
					nBkBlue  = (pBuffer[nPos + 0] & 0x1F) >> 1;

					pBuffer[nPos + 1] = ((nBkRed   + nBxRed) & 0x1F) << 3;
					pBuffer[nPos + 0] = ((nBkBlue  + nBxBlue) & 0x1F);// << 3;
					//	绿色最难弄。TNND
					pBuffer[nPos + 1] |= ((nBkGreen + nBxGreen) & 0x1B >> 3);
					pBuffer[nPos + 0] |= ((nBkGreen + nBxGreen) & 0x07 << 5);
				}
				break;

			//	24位色处理
			case 24:
				nPos = j * ddsd.lPitch + (i * 3);

				if(i == nX || i == nX + nWidth || j == nY || j == nY + nHeight)
				{
					pBuffer[nPos + 0] = 255;
					pBuffer[nPos + 1] = 255;
					pBuffer[nPos + 2] = 255;
				}
				else
				{
					nBkBlue  = pBuffer[nPos + 1] >> 1;
					nBkGreen = pBuffer[nPos + 2] >> 1;
					nBkRed   = pBuffer[nPos + 3] >> 1;
					pBuffer[nPos + 0] = nBkBlue  + nBxBlue;
					pBuffer[nPos + 1] = nBkGreen + nBxGreen;
					pBuffer[nPos + 2] = nBkRed   + nBxRed;
				}
				break;

			//	32位色处理
			case 32:
				nPos = j * ddsd.lPitch + (i << 2);
				if(i == nX || i == nX + nWidth || j == nY || j == nY + nHeight)
				{
					pBuffer[nPos + 0] = 255;
					pBuffer[nPos + 1] = 255;
					pBuffer[nPos + 2] = 255;
					pBuffer[nPos + 3] = 255;
				}

			default:
				break;
			}
		}
	}

	//	显存修改完成，后台面板解锁
	if(FAILED(pBackSurface->Unlock(0)))
	{
		return false;
	}

	return true;
}


CGround::CGround(CPc* pPc)
{
	m_nLineCnt = 0;
	m_nColumnCnt = 0;

	m_nTop = 0;
	m_nLeft = 0;

	//	设置角色未锁定
	m_isLock = 0;

	//this->m_pStr[0] = 0;

	m_nScreenWidth  = CSetting::Instance()->GetScreenWidth();
	m_nScreenHeight = CSetting::Instance()->GetScreenHeight();

	m_pPc = pPc;
}


CGround::~CGround()
{
	m_stGround.mtAreas.Delete();
}
