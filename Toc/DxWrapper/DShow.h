//-----------------------------------------------------
//  ������  CDShow
//  ������  DirectShow ��װ��
//  ���ߣ�  KudoXinyiji
//
//  ģʽ��  Singleton
//-----------------------------------------------------



#ifndef _CDSHOW_H
#define _CDSHOW_H


#include <dshow.h>
#pragma comment(lib, "strmiids.lib")


namespace DxWrapper
{

    class CDShow
    {
    public:
        bool Init(HWND hWnd);
        void Term();

        bool Play(char*);
        bool Play(const long&);

        static bool InitInstance();
        static void ExitInstance();
        static CDShow*  Instance();

    private:
        CDShow();
        ~CDShow();

        HWND m_hWnd;

        IGraphBuilder* m_pGraph;        //IGraphBuilder �ӿ��ṩ������Filter Graph��//�صķ���
        IMediaControl* m_pMediaControl; //IMediaControl �ӿ��ṩ�˿�������Filter //Graph����������ط���
        IMediaEventEx* m_pEvent;        //IMediaEventEx �̳���IMediaEvent���ṩ�˴�//Filter Graph ��������ȡ�¼���Ϣ�ķ���
        IMediaSeeking* m_pMediaSeeking; //IMediaSeeking �ṩ�˿������Ĳ���λ�úͲ���//�ٶȵķ���

        char m_szFilePath[MAX_PATH];    //��ǰ���ŵ�ý���ļ�������
        bool m_isPlaying;               //��ǰ�Ĳ���״̬


        long m_nComInit;    //  COM �Ƿ��Ѿ���ʼ�� 
                            //  0 - δ��ʼ��
                            //  1 - �ѳ�ʼ��

        long m_nDDState;    //  DirectDraw ��״̬
                            //  0 - �����ⲿ��ʼ������δ��ʼ��������Ҫ�������
                            //  1 - ���� InitDDraw ��ʼ������Ҫ�������

        static CDShow* m_pDShow;    //  CDShow Ψһʵ��������

    };

};


#endif

