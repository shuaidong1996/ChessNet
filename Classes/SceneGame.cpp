#include "SceneGame.h"
#include "Net.h"
#include "Game.h"

int SceneGame::_randomSelf = -1;
int SceneGame::_randomOther = -1;

bool SceneGame::init()
{
	CCLayer::init();

	_menu = CCMenu::create();
	addChild(_menu);

	//��ʼ��������ť
	initStartServerButton();

	//���ӷ�������ť
	initConnectButton();

	//���ùرհ�ť
	CCMenuItem* closeItem = CCMenuItemImage::create("CloseNormal.png", "CloseSelected.png", this, menu_selector(SceneGame::closeGame));
	closeItem->setPosition(ccp(370, -200));
	_menu->addChild(closeItem);

	//��������  �� box �Ͳ���Ҫ������
	//setTouchEnabled(true);
	//setTouchMode(kCCTouchesOneByOne);

	//���������
	_randomSelf = CCRANDOM_0_1() * 100;

	return true;
}
//������������ʼ��ť
void SceneGame::initStartServerButton()
{
	serverItem = CCMenuItemImage::create("zhuji.png", "zhuji2.png");
	_menu->addChild(serverItem);
	serverItem->setTarget(this, menu_selector(SceneGame::startServer));//���ô�������
	moveBy(ccp(200, 100), serverItem);//����λ��
}
//�����ͷ������Ӱ�ť
void SceneGame::initConnectButton()
{
	clientItem = CCMenuItemImage::create("kefuji.png","kefuji2.png");
	_menu->addChild(clientItem);
	clientItem->setTarget(this, menu_selector(SceneGame::ConnectServer));//���ô�������
	moveBy(ccp(200, -100), clientItem);//����λ��
}
//����������
void SceneGame::startServer(CCObject*  sender)
{
	//ConnectServer��ťֻ�ܵ�һ��
	serverItem->setEnabled(false);//������ˣ��Ժ���Ҳ���ܵ���
	clientItem->setEnabled(false);

	getHostIP();

	if (Net::getInstance()->startServer(6543))
		scheduleUpdate();//��ʱ��
}
//�ͷ��� ����
void SceneGame::ConnectServer(CCObject* sender)
{
	serverItem->setEnabled(false);//������ˣ��Ժ���Ҳ���ܵ���
	clientItem->setEnabled(false);

	//�����ı�  ���� box ���������text
	//_text = CCTextFieldTTF::textFieldWithPlaceHolder("Please input ip", "Arial", 30);
	//addChild(_text);
	//_text->setPosition(ccp(250, 225));

	//����һ�����Ա༭�� box
	box = CCEditBox::create(CCSize(250, 50), CCScale9Sprite::create("green_edit.png"));
	addChild(box);
	box->setPosition(ccp(240, 225));
	//������ʾ
	box->setPlaceHolder("please input:IP");
	//������ʾ����
	box->setFont("Arial", 30);
	//���������������ɫ
	box->setFontColor(ccc3(0, 0, 255));
	//���ÿ�������ĳ���
	box->setMaxLength(15);
	//��������̡����һ�������ǣ��س���������
	box->setReturnType(kKeyboardReturnTypeDone);
	//���ü��̵���Ϊ���ּ��̻�����
	box->setInputMode(kEditBoxInputModeAny);

	schedule(schedule_selector(SceneGame::pingIP));
}
//bool SceneGame::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
//{
//	if (_text!=NULL && _text->boundingBox().containsPoint(pTouch->getLocation()))
//		_text->attachWithIME();//���������
//
//	return false;
//}
void SceneGame::pingIP(float)
{
	/*if (_text == NULL)
		return;
	const char* endIP= _text->getString();*/
	static char _IP[30];

	const char* IP = box->getText();
	if (strlen(IP) >15 || strlen(IP)<8)
		return;

	if (strcmp(_IP, IP) == 0)
		return;
	strcpy(_IP, IP);

	//if (Net::getInstance()->Connect(6543, "127.0.0.1"))//������ӳɹ�
	if (Net::getInstance()->Connect(6543, IP))//������ӳɹ�
	{
		if (_randomOther == -1)
		{
			Net::getInstance()->Send((char*)&_randomSelf, 4);//ע��,�ȷ�,��Ȼ�����������

			//������һ����ʱ������ʵû�б�Ҫ���͵�ѧϰһ��
			schedule(schedule_selector(SceneGame::RecvRadom));
		}
		unschedule(schedule_selector(SceneGame::pingIP));
		//setTouchEnabled(false);
		box->setVisible(false);
	}
}
//�����
void SceneGame::update(float)
{
	if (Net::getInstance()->Accept())
	{
		if (_randomOther == -1)//��û�н��յ��ͷ��˷������������
		{
			Net::getInstance()->Send((char*)&_randomSelf, 4);
			
			schedule(schedule_selector(SceneGame::RecvRadom));
		}
		unscheduleUpdate();
		CCLog("Connect OK");
	}
}
void SceneGame::RecvRadom(float)
{
	static int recvLen = 0;
	char* buf = (char*)&_randomOther;//�����
	int ret = Net::getInstance()->Recv(buf + recvLen, 4 - recvLen);

	if (ret <= 0)
		return;

	recvLen += ret;
	if (recvLen != 4)
		return;

	if (_randomSelf > _randomOther || (_randomSelf == _randomOther && Net::getInstance()->isServer()))
	{
		Game::_redStart = true;
	}
	else
	{
		Game::_redStart = false;
	}
	_game = Game::create();
	addChild(_game);
	unschedule(schedule_selector(SceneGame::RecvRadom));
}

void SceneGame::moveBy(CCPoint pt, CCNode* node)
{
	node->setPosition(pt + node->getPosition());
}

void SceneGame::getHostIP()
{
#ifdef WIN32
	char szHost[256];//����������Ļ�����  
	gethostname(szHost, 256);//ȡ�ñ�����������  
	hostent *pHost = ::gethostbyname(szHost);//ͨ���������õ���ַ��Ϣ  

	//һ�����������ж�����������IP������Ĵ���������е�IP��ַ  
	in_addr addr;
	for (int i = 0;; i++)
	{
		//��õ�ַ(�����ֽ�)  
		char *p = pHost->h_addr_list[i];
		if (NULL == p)
			break;//�˳�ѭ��  

		//����ַ������in_addr�ṹ����  
		memcpy(&addr.S_un.S_addr, p, pHost->h_length);

		//��in_addrת��Ϊ�����ֽ���  
		char *IP = inet_ntoa(addr);

		CCLabelTTF* label = CCLabelTTF::create(IP, "Arial", 30);
		addChild(label);
		moveBy(ccp(250, 280 - i * 30), label);//����λ��
	}
#else
	int sockfd;
	struct ifconf ifconf;
	struct ifreq *ifreq;
	char buf[512];//������
	//��ʼ��ifconf
	ifconf.ifc_len = 512;
	ifconf.ifc_buf = buf;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0)
		return;
	ioctl(sockfd, SIOCGIFCONF, &ifconf); //��ȡ���нӿ���Ϣ

	//������һ��һ���Ļ�ȡIP��ַ
	ifreq = (struct ifreq*)ifconf.ifc_buf;
	for (int i = (ifconf.ifc_len / sizeof(struct ifreq)); i>0; i--)
	{
		if (ifreq->ifr_flags == AF_INET){ //for ipv4
			char* IP = inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
			ifreq++;
			if(!strcmp(IP,"127.0.0.1"))
				continue;
			CCLabelTTF* label = CCLabelTTF::create(IP, "Arial", 30);
			addChild(label);
			moveBy(ccp(250, 280 - i * 30), label);//����λ��
		}
	}
#endif
}
void SceneGame::closeGame(CCObject*)
{
	if (Net::getInstance()->isServer())
#ifdef WIN32
		closesocket(Net::getInstance()->getServerSocket());
#else
		close(Net::getInstance()->getServerSocket());
#endif
	else
#ifdef WIN32
		closesocket(Net::getInstance()->getClientSocket());
#else
		close(Net::getInstance()->getClientSocket());
#endif
	exit(0);
}

