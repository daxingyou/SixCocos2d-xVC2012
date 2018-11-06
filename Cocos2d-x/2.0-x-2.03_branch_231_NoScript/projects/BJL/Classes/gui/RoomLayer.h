//
//  roomLayer.h
//  Game
//
//  Created by zhouwei on 13-6-17.
//
//

#ifndef __Game__roomLayer__
#define __Game__roomLayer__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "MenuLayer.h"
#include "WarningLayer.h"
#include "BaseLayer.h"
#include "GroupSprite.h"
#include "CustomerServiceLayer.h"
#include "PromptBox.h"
#include "gui/FMGameItemLayer.h"


#define MASKD_TAG 20          //马上开打按钮的TAG
#define MASKDMENU_TAG 21      //马上开打动画效果TAG
#define PAIDUI_TAG 22         //排队精灵TAG

//class RoomUserInfo;
//class LevelInfo;
//class PopFrame;
//class tableViewLayer;

enum ROOM_TYPE
{
	ROOM_TYPE_Super = 0,
	ROOM_TYPE_Seniy,
	ROOM_TYPE_Middle,
	ROOM_TYPE_Primary
	
};

enum ROOM_LAYER_LEVEL
{
	ROOM_LAYER_LEVEL_HOBBY,
	ROOM_LAYER_LEVEL_GAMETYPE,
	ROOM_LAYER_LEVEL_GAMEROOM
};

class RoomLayer : public BaseLayer
{
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(RoomLayer);
    
    virtual void onEnter();
    virtual void onExit();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){return true;};
    RoomLayer();
    ~RoomLayer();
public:

	void toMenu(CCObject* obj);							//菜单
	void rechangeLittleButton(CCObject* obj);			//冲金币小按钮
	void huodong(CCObject * obj);						//活动
	void geTCHARge(CCObject * obj);                     //领话费
	void onStart(ROOM_TYPE index);               //马上开打

	//底下菜单函数
	void toFriend(CCObject* obj);						//好友系统
	void toShop(CCObject* obj);							//游戏商城
	void onQuickStart(CCObject *pSender);				//快速游戏          add by wangjun
	void mission(CCObject* obj);						//任务
	void toRank(CCObject* obj);							//排行

	//中间菜单
	void toExchange(CCObject* obj);							//兑换
	void toLuckyDraw(CCObject* obj);						//幸运抽奖
	void toVIP(CCObject* obj);						//vip

	//上面菜单函数
	void rechange(CCObject* obj);						//充金币
	void signIn(CCObject* obj);						//每日签到
	void setting(CCObject* obj);						//设置
	void toHelp(CCObject* obj);							//游戏帮助
	void message(CCObject* obj);						//消息
	void pressBack(CCObject* obj);						//返回

	void showPlayerInformation(CCObject *obj);          //人物信息

	//游戏按钮
	void DDZItemCallback(CCObject* pSender);					//斗地主
	void ShakeItemCallback(CCObject* pSender);					//摇摇乐
	void Lucky28ItemCallback(CCObject* pSender);                //幸运28
	void NiuniuItemCallback(CCObject* pSender);                 //牛牛
	void ZajinHuaItemCallback(CCObject* pSender);               //炸金花 

	//中间菜单函数
	void toNormalGame(CCObject* obj);					//普通场
	void toOtherGame(CCObject* obj);					//疯狂炸弹场

	//中间菜单函数
	void toPrimaryGame(CCObject* obj);					//初级场
	void toMiddleGame(CCObject* obj);					//中级场
	void toSeniyGame(CCObject* obj);					//高级场
	void toSuperGame(CCObject* obj);					//顶级场

	void matchBackButton(CCObject* obj);				//比赛返回
	void gameBackButton(CCObject* obj);				    //比赛返回

	//联系客服
	void toPhoneKefu(CCObject* obj);					//联系客服

	void automaticLogin(bool automatic,bool overResgist = false);				//自动登录,注册

	void onLoginFinish();								//登陆完成
	void onLoginFaild(CCObject* obj);					//登陆失败

	string createAccount();								//生成账号
	string createPassword();							//生成密码
	void onRegistSucc(CCObject* obj);					//注册成功
	void onRegisterFaild(CCObject* obj);				//注册失败

	//紫金卡充值结果
	void onlandcardResult(CCObject* obj);
	//电话卡充值结果
	void onPhoneCardResult(CCObject* obj);

	void onLinkResult(CCObject* obj);
    //void shangcheng(CCObject * obj);                      //商城
	
    //void changeSoundState(CCObject * obj);              //音乐开关
    //void backPreScene(CCObject * obj);                  //返回上个场景
    
 //   void onLoingFinish();                               //进入小游戏
 //   void checkUserCannelQueueUp(float dt);              //检测用户是否取消排队
 //   void showTable(float dt);                                   //显示10张桌子
 //   void onMatch(CCObject* obj);                                     //显示匹配框
 //   void onUpdateScore(CCObject* obj);
 //   void onPesent(CCObject* obj);                       //赠送回调
 //   void onCharge(CCObject* obj);                       //领取话费金币回调
 //   void showNotice();                                  //显示公告
 //   void onBankrupt(CCObject* obj);                     //破产回调
 //   void onHideLevel(float dt);                        //等级公告隐藏
 //   void onChargeTips(CCObject* obj);                   //充值提示
	//void onChangeGame(CCObject* obj);					//切换游戏

	void showLoginAward();								//显示连续登陆奖励
	void closeLoginAward();
	void showSelectHead(CCObject* obj);								//选择头像

	void permitButtonClick();							//允许按钮按
	void forbidButtonClick();							//禁止按钮按

	void showMoveingLabelTips(const char* str);			//显示飘字提示	

	void onChange(CCObject* obj);						//赠送，回调
	void onPesent(CCObject* obj);						//领话费回调

	void onUpdateScoreAndGoldEgg(CCObject* obj);		//刷新金币和话费点

	void onRefshTotalNum(CCObject* obj);				//刷新人数

	virtual void keyBackClicked(void);					//手机返回按钮

	void setGoingRoomLayer();

	void gameToBankrupt(bool isBankrupt);			//小游戏返回大厅是是否破产

	void showDailyGetCoins(CCObject* obj);			//显示每日领金币
	void showDailyGetCoinsResult(CCObject* obj);	//每日领取救济金结果
	void setRoomLayerLevel(ROOM_LAYER_LEVEL level);
private:
	//void getUserInfo();
	//void createTable();
private:
	MenuLayer  *mMenuLayer;
	CCLayer *pWarningLayer;
	CustomerServiceLayer *pCustomerService;
private:
	bool       mMenuCloseOrOpen;
	int       isGoingRoomLayer;
	
private:
	CCSize winSize;
	int roomLevel;
	CCMenu* m_pPlayerMenu;			//人物信息按钮
	CCMenu* m_pButtonMenu;			//底下按钮
	CCMenu* m_pStartMenu;			//快速开始按钮
	CCMenu* m_pOtherMenu;				//上面按钮
	CCMenu* m_pTopMenu;				//上面按钮
	GameItemLayer* m_pMiddleHobbyMenu;		//中间按钮（选择游戏）
	CCMenu* m_pMiddleMenu;			//中间按钮（普通疯狂）
	CCMenu* m_pMiddleMenu2;			//中间按钮2（初中高）
	//CCMenu* m_pKefuMenu;			//联系客服按钮
	//CCMenu* m_pchongSMenu;			//小充金币按钮
	//CCMenu* m_pMatchBack;			//选择场次时返回
	//CCMenu* m_pGameBack;			//选择游戏时返回

	CCMenuItemSprite* pPlayerMenuSprite;		//人物头像

	int m_nGameType;						//游戏类型 欢乐什么的

	CCLabelTTF* m_pUserScore;				//金币
	CCLabelTTF* m_pUserPhonePoint;

	CCMenuItemSprite * m_pHappyS;

	CCMenuItemSprite * m_pPrimaryS;
	CCMenuItemSprite * m_pMiddleS;
	CCMenuItemSprite * m_pSeniyS;
	CCMenuItemSprite * m_pSuperS;

	CCSprite* m_MessageNumBack;
	CCLabelTTF* m_MessageNum;

	CCSprite* m_UMMessageNumBack;
	CCLabelTTF* m_UMMessageNum;

	PromptBox* m_pRuberBox;
//    tagGlobalUserData * userInfo;
//    //声音状态
//    bool soundState;
//    CCMenuItemSprite * soundMenuSprite;
//    CCLabelTTF * tishi;
//    RoomTimer * roomTimer;
//    
//    RoomUserInfo* m_pUserInfo;
////    LevelInfo*    m_pLevelInfo;
//    PopFrame*   m_pPopFrame;
//    CCSprite*   m_pLevelSprite;
//    int m_nBGMID;
//
//	GameSelectView* m_pViewLayer;
//	CCMenu*		   m_pBeginPlayCardMenu;			//马上开打
//	CCSprite*	   m_pLightBack;					//灯光背景
//	RoomTable*	   m_pTable[10];					//桌子
//
//	int m_nGameKindId;								//游戏类型
};

#endif /* defined(__Game__roomLayer__) */
