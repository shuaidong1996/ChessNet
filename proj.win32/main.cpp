#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"

USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	//��ʼ��windows socket ����
	WSADATA data;
	DWORD ver = MAKEWORD(2, 2);//����windows���õ���ʲô�汾
	WSAStartup(ver, &data);

    // create the application instance
    AppDelegate app;
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setViewName("ChessNet");
    eglView->setFrameSize(800, 450);
    return CCApplication::sharedApplication()->run();
}
