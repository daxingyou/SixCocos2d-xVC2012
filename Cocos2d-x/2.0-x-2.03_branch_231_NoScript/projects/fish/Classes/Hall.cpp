#include "Hall.h"
#include "SkinControls.h"
#include "Player.h"
#include "Fish.h"

static int sp_ProcessBar_Tag = 0;
static int sp_ProcessAnimation_Tag = 1;

CHall::CHall(void)
{
	setTouchEnabled(true);
}


CHall::~CHall(void)
{
}

void CHall::onEnter()
{
	CCLayer::onEnter();

    CCSize s = CCDirector::sharedDirector()->getWinSize();

	CCSprite * bg = CCSprite::create("bg/bg1.jpg");
	bg->setPosition(ccp(s.width/2, s.height/2));
	addChild(bg);

	CCMenuItemImage * pItemHelp = CCMenuItemImage::create("button/normal1.png", "button/select1.png", "button/hightlight1.png", this, menu_selector(CHall::OnHelp) );
	CCMenuItemImage * pItemSet = CCMenuItemImage::create("button/normal1.png", "button/select1.png", "button/hightlight1.png", this, menu_selector(CHall::OnHelp) );
	CCMenuItemImage * pItemSupplement = CCMenuItemImage::create("button/normal.png", "button/select.png", "button/hightlight.png", this, menu_selector(CHall::OnHelp) );
	CCMenuItemImage * pItemMyDepot = CCMenuItemImage::create("button/normal.png", "button/select.png", "button/hightlight.png", this, menu_selector(CHall::OnHelp) );
	CCMenuItemImage * pItemGam = CCMenuItemImage::create("button/normal.png", "button/select.png", "button/hightlight.png", this, menu_selector(CHall::OnHelp) );
	CCMenuItemImage * pItemShop = CCMenuItemImage::create("button/normal.png", "button/select.png", "button/hightlight.png", this, menu_selector(CHall::OnHelp) );
	CCMenuItemImage * pItemBullet = CCMenuItemImage::create("button/normal.png", "button/select.png", "button/hightlight.png", this, menu_selector(CHall::OnHelp) );
	CCMenuItemImage * pItemMission = CCMenuItemImage::create("button/normal.png", "button/select.png", "button/hightlight.png", this, menu_selector(CHall::OnHelp) );
	CCMenuItemImage * pItemSuccess = CCMenuItemImage::create("button/normal.png", "button/select.png", "button/hightlight.png", this, menu_selector(CHall::OnHelp) );
	CCMenuItemImage * pItemExit = CCMenuItemImage::create("button/normal.png", "button/select.png", "button/hightlight.png", this, menu_selector(CHall::OnHelp) );

	float fX=0, fY=0;
	setMenuItemEx( pItemHelp, fX, fY);
	setMenuItemEx( pItemSet, fX, fY);
	setMenuItemEx( pItemSupplement, fX, fY);
	setMenuItemEx( pItemMyDepot, fX, fY);
	setMenuItemEx( pItemGam, fX, fY);
	setMenuItemEx( pItemShop, fX, fY);
	setMenuItemEx( pItemBullet, fX, fY);
	setMenuItemEx( pItemMission, fX, fY);
	setMenuItemEx( pItemSuccess, fX, fY);
	setMenuItemEx( pItemExit, fX, fY);
	CCMenu * pMenu = CCMenu::create(pItemHelp, pItemSet, pItemSupplement, pItemMyDepot, pItemGam, pItemShop, pItemBullet, pItemMission, pItemSuccess, pItemExit, NULL);
	addChild( pMenu, 100 );
	pMenu->setAnchorPoint(ccp(0, 0));
	pMenu->setPosition( ccp(0, 0) );

	// 初始鱼群
	CFashManage::Init( this );

	// 初始玩家
	PlayerManage::create(this, 3);
	PlayerManage::create(this, 2, true);
	PlayerManage::create(this, 1);
	PlayerManage::create(this, 0);

	// 更新捕鱼情况
	schedule(schedule_selector(CHall::Update), 0.05f);
}

void CHall::onExit()
{
	CFashManage::Release();
}

void CHall::Update(float)
{
	CCObject* pBulletObject = NULL;
	CCObject* pFishObject = NULL;
	Player *pPlayer=PlayerManage::GetMyPlayer();
	CBulletManage *pManage=PlayerManage::GetMyPlayer()->getBulletManage();
	CCSpriteBatchNode *pNode = PlayerManage::GetMyPlayer()->getBulletManage()->sharedBullet();
	unsigned int aa=pNode->getChildrenCount();

	CCARRAY_FOREACH(PlayerManage::GetMyPlayer()->getBulletManage()->sharedBullet()->getChildren(), pBulletObject)
	{
		CBullet *pBullet = (CBullet *)pBulletObject;
		if ( pBullet->getIsCatching() )
			continue;

		CCARRAY_FOREACH(CFashManage::sharedFish()->getChildren(), pFishObject)
		{
			CFish *pFish = (CFish *)pFishObject;
			if (pFish->getIsCatching())
				continue;

			if ( pFish->boundingBox().containsPoint( pBullet->getPosition() ) )
			{
				pFish->setIsCatching( true );
				pBullet->setIsCatching( true );

				CCArray* pArray = CCArray::create();
				for(int i = 1; i < 3; i++)
				{
					pArray->addObject(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(CCString::createWithFormat("fish0%d_catch_0%d.png",pFish->getTag(),i)->getCString()));
				}

				CCPoint ptHit( pBullet->getPositionX(), pBullet->getPositionY() );
				CCRepeat* pStruggleAnimation = CCRepeat::create(CCAnimate::create(CCAnimation::createWithSpriteFrames(pArray, 0.2f)), 2); // 鱼挣扎
				CCFiniteTimeAction *pReleaseFishAnimation = CCCallFunc::create(pFish, callfunc_selector(CFish::removeSelf)); // 释放鱼
				CCFiniteTimeAction *pBulletNetAnimation = CCCallFuncND::create(pBullet, callfuncND_selector(CBullet::FinishAnimation), (void *)&ptHit ); // 子弹隐藏，鱼网打开
				
				pBullet->stopAllActions();
				pBullet->setVisible( false );
				//CCActionInterval *pCastingNetAnimation = pBullet->castingNetAnimation();
				//CCFiniteTimeAction *pReleaseButtleAnimation = CCCallFunc::create(pBullet, callfunc_selector(CBullet::Release)); // 释放鱼

				CCFiniteTimeAction* catchSequence = CCSequence::create(pBulletNetAnimation, pStruggleAnimation, /*pReleaseButtleAnimation, */pReleaseFishAnimation, NULL);
				pFish->stopAllActions();
				pFish->runAction( catchSequence );
				
				//pBullet->runAction( pBulletNetAnimation );
			}
		}
	}

	// 保持鱼数目维持在 d_Fish_Max_Count
	while( CFashManage::sharedFish()->getChildrenCount() < d_Fish_Max_Count )
	{
		CFish::Fish_Type fishTytpe = (CFish::Fish_Type)(rand() % (CFish::Fish_Type_Count-1) + 1);
		CFashManage::addOneFish( fishTytpe );
	}
}

CCAnimate * CHall::CreateFrameAnimate(CCSpriteFrameCache * pCache, char * pSigleName, int iSize, float fTime)
{
	CCArray * pArray1 = CCArray::createWithCapacity( iSize );
	char cBuffer[100]="";
	for (int i=0; i<iSize; i++)
	{
		sprintf(cBuffer, pSigleName, i);
		CCSpriteFrame *pSpriteFrame = pCache->spriteFrameByName(cBuffer);
		pArray1->addObject( pSpriteFrame );
	}

	CCAnimation* animation = CCAnimation::createWithSpriteFrames(pArray1, fTime);
	return CCAnimate::create(animation);
}

void CHall::OnHelp(CCObject* pSender)
{
	;
}

void CHall::setMenuItemEx(CCMenuItemImage * pItem, float &fX, float &fY)
{
	pItem->setAnchorPoint(ccp(0, 0));
	pItem->setPosition( fX, fY );
	fX += pItem->getContentSize().width;
}

void CHall::scene()
{
	CCScene * pScene = CCScene::create();
	CHall * pLoading = new CHall;
	pScene->addChild( pLoading );
	pLoading->release();

	CCDirector::sharedDirector()->replaceScene( CCTransitionFade::create(0.5f, pScene, ccBLACK) );
}