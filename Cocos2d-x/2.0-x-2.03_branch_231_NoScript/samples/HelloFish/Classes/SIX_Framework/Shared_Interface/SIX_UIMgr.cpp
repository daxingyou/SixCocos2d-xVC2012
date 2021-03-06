#include "SIX_UIMgr.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <exception>
#include <cxxabi.h>
#endif

//USING_NS_CC_EXT;

SIX_UIMgr::SIX_UIMgr()
{
	m_bDirty = false;
}
SIX_UIMgr::~SIX_UIMgr()
{
	UnloadFrameCache();
	UnloadAnimationCache();
}

template <typename T>
static bool ParseSingleAttribute(SIX_UIMgr *pThis,T *pObject,const char *attrName,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (!pThis || !pObject || !attrName || !pDataElement)
		return false;

	const char *typeName = 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	typeName = typeid(pObject).name();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	int status=0;
	const std::type_info &ti = typeid(pObject);
	typeName = abi::__cxa_demangle(ti.name(),0,0,&status);
#endif

	//SIXLog("ParseSingleAttribute.typeid(pObject).name()=%s",typeName);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if (!STRCMP(typeName,"struct cocos2d::_ccColor4B *"))
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (!STRCMP(typeName,"cocos2d::_ccColor4B *"))
#endif
	{
		SIX_KV *pKV = new SIX_KV();
		int pos = pThis->GetAttributeAndPos(pDataElement,attrName,pKV);
		if (pos>=0)
		{
			MP *mp=new MP();
			int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
			if (count==3 || count==4)
			{
				((ccColor4B*)pObject)->r = (GLubyte)atoi(mp->at(0).c_str());
				((ccColor4B*)pObject)->g = (GLubyte)atoi(mp->at(1).c_str());
				((ccColor4B*)pObject)->b = (GLubyte)atoi(mp->at(2).c_str());

				if (count==4)
				{
					((ccColor4B*)pObject)->a = (GLubyte)atoi(mp->at(3).c_str());
				}
			}
	#ifdef WIN32
			mp->swap(MP());
	#endif
			mp->clear();
			CC_SAFE_DELETE(mp);

			// 移除指定的属性
			pDataElement->Del(pos);
		}
		CC_SAFE_DELETE(pKV);
		return true;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	else if (!STRCMP(typeName,"class cocos2d::CCPoint *"))
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	else if (!STRCMP(typeName,"cocos2d::CCPoint *"))
#endif
	{
		SIX_KV *pKV = new SIX_KV();
		int pos = pThis->GetAttributeAndPos(pDataElement,attrName,pKV);
		if (pos>=0)
		{
			MP *mp=new MP();
			int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
			if (count==2)
			{
				((CCPoint*)pObject)->setPoint(atof(mp->at(0).c_str()),atof(mp->at(1).c_str()));
			}
	#ifdef WIN32
			mp->swap(MP());
	#endif
			mp->clear();
			CC_SAFE_DELETE(mp);

			// 移除指定的属性
			pDataElement->Del(pos);
		}
		CC_SAFE_DELETE(pKV);
		return true;
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	else if (!STRCMP(typeName,"class cocos2d::CCSize *"))
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	else if (!STRCMP(typeName,"cocos2d::CCSize *"))
#endif
	{
		SIX_KV *pKV = new SIX_KV();
		int pos = pThis->GetAttributeAndPos(pDataElement,attrName,pKV);
		if (pos>=0)
		{
			MP *mp=new MP();
			int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
			if (count==2)
			{
				((CCSize*)pObject)->setSize(atof(mp->at(0).c_str()),atof(mp->at(1).c_str()));
			}
	#ifdef WIN32
			mp->swap(MP());
	#endif
			mp->clear();
			CC_SAFE_DELETE(mp);

			// 移除指定的属性
			pDataElement->Del(pos);
		}
		CC_SAFE_DELETE(pKV);
		return true;
	}

	SIXLog("ParseSingleAttribute.typeid(pObject).name()=%s.Unknown!",typeName);
	return false;
}

template <typename T>
static bool LoadControl(SIX_UIMgr *pThis,T *pControl,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (!pThis || !pControl || !pDataElement)
		return false;

	// 有些控件加载时要先初始化某些属性，比如：AnchorPoint
	const char *typeName = 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	typeName = typeid(pControl).name();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	int status=0;
	const std::type_info &ti = typeid(pControl);
	typeName = abi::__cxa_demangle(ti.name(),0,0,&status);
#endif

	//SIXLog("LoadControl.typeid(pControl).name()=%s",typeName);

	// 以下控件描点默认为(0,0)
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if (!STRCMP(typeName,"class cocos2d::CCLabelTTF *") ||
		!STRCMP(typeName,"class cocos2d::CCSprite *") ||
		!STRCMP(typeName,"class cocos2d::CCProgressTimer *")
	)
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (!STRCMP(typeName,"cocos2d::CCLabelTTF *") ||
		!STRCMP(typeName,"cocos2d::CCSprite *") ||
		!STRCMP(typeName,"cocos2d::CCProgressTimer *")
	)
#endif
	{
		// 描点默认为(0,0)
		pControl->setAnchorPoint(ccp(0,0));
	}

	SIX_KV *pKV = 0;
	unsigned int j=0;
	CCARRAY_FOREACH_T(pDataElement->GetChildren(),pKV,SIX_KV*)
	{
		j++;
		//SIXLog("SIX_UIMgr.pKV[%d].key[%s].value[%s]",j,pKV->getKey().getCString(),pKV->getValue().getCString());

		if (!pThis->ParseAttribute(pControl,pKV))
			return false;
	}

	ccDirectorProjection mProjection = CCDirector::sharedDirector()->getProjection();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if (!STRCMP(typeName,"class cocos2d::CCLabelTTF *"))
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (!STRCMP(typeName,"cocos2d::CCLabelTTF *"))
#endif
	{
		SIX_Utility::GetInstance()->FixedTexParameters(((CCLabelTTF*)pControl)->getTexture());

	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	else if (!STRCMP(typeName,"class cocos2d::CCSprite *"))
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	else if (!STRCMP(typeName,"cocos2d::CCSprite *"))
#endif
	{
		SIX_Utility::GetInstance()->FixedTexParameters(((CCSprite*)pControl)->getTexture());
	}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	else if (!STRCMP(typeName,"class cocos2d::CCSpriteBatchNode *"))
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	else if (!STRCMP(typeName,"cocos2d::CCSpriteBatchNode *"))
#endif
	{
		SIX_Utility::GetInstance()->FixedTexParameters(((CCSpriteBatchNode*)pControl)->getTexture());
	}

	return true;
}

// -----------------------------------------------------
// LoadControl与CreateControl区别：
// 如果XML节点没有组合属性，直接先创建控件，再用LoadControl
// 就可以解析了，比如Layer等；否则需要用CreateControl里解析
// 组合属性，控件由CreateControl内部创建，比如CCSprite等。
// -----------------------------------------------------
// Cool.Cat@2013-04-5
// -----------------------------------------------------
template <typename T>
static T *CreateControl(SIX_UIMgr *pThis,const char *name,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (!pThis)
		return 0;

	SIX_DSTPL<SIX_KV> *pDataElementKV = pThis->ParseKV(name,pDataElement);
	if (!pDataElementKV)
		return 0;

	T *pControl = 0;

	// 有些控件创建时要先处理特殊属性，比如：file=''
	const char *typeName = 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	typeName = typeid(T).name();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	int status=0;
	const std::type_info &ti = typeid(T);
	typeName = abi::__cxa_demangle(ti.name(),0,0,&status);
#endif

	//SIXLog("CreateControl.typeid(%s).name()=%s",name,typeName);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if (!STRCMP(typeName,"class cocos2d::CCSprite") ||
		!STRCMP(typeName,"class cocos2d::extension::CCScale9Sprite")
	)
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (!STRCMP(typeName,"cocos2d::CCSprite") ||
		!STRCMP(typeName,"cocos2d::extension::CCScale9Sprite")
	)
#endif
	{
		// 先加载图片
		SIX_KV *pKVFile = new SIX_KV();
		int pos = pThis->GetAttributeAndPos(pDataElementKV,"file",pKVFile);
		if (pos<0)
		{
			pos = pThis->GetAttributeAndPos(pDataElementKV,"Frame",pKVFile);
			if (pos<0)
			{
				CC_SAFE_DELETE(pKVFile);
				CC_SAFE_DELETE(pDataElementKV);
				return 0;
			}
		}

		CCRect rc = CCRectMake(0,0,0,0);
		SIX_KV *pKVRect = new SIX_KV();
		int rcpos = pThis->GetAttributeAndPos(pDataElementKV,"rect",pKVRect);
		if (rcpos>=0)
		{
			float x = 0.0f;
			float y = 0.0f;
			float width = 0.0f;
			float height = 0.0f;
			MP *mp=new MP();
			int count = SIX_Utility::GetInstance()->Split(",",(char*)pKVRect->getValue().getCString(),mp);
			if (count==4)
			{
				x = atof(mp->at(0).c_str());
				y = atof(mp->at(1).c_str());
				width = atof(mp->at(2).c_str());
				height = atof(mp->at(3).c_str());
			}
#ifdef WIN32
			mp->swap(MP());
#endif
			mp->clear();
			CC_SAFE_DELETE(mp);

			rc = CCRect(x,y,width,height);
			pDataElementKV->Del(rcpos);
		}

		CC_SAFE_DELETE(pKVRect);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		if (!STRCMP(typeName,"class cocos2d::CCSprite"))
			pControl = (T*)CCSprite::create(pKVFile->getValue().getCString());
		else if (!STRCMP(typeName,"class cocos2d::extension::CCScale9Sprite"))
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		if (!STRCMP(typeName,"cocos2d::CCSprite"))
			pControl = (T*)CCSprite::create(pKVFile->getValue().getCString());
		else if (!STRCMP(typeName,"cocos2d::extension::CCScale9Sprite"))
#endif
		{
			if (0 == pKVFile->getKey().compareIgnoreCase("file"))
			{
				if (rc.equals(CCRectZero))
					pControl = (T*)CCScale9Sprite::create(pKVFile->getValue().getCString());
				else
					pControl = (T*)CCScale9Sprite::create(pKVFile->getValue().getCString(),rc);
			}
			else
			{
				if (rc.equals(CCRectZero))
					pControl = (T*)CCScale9Sprite::createWithSpriteFrameName(pKVFile->getValue().getCString());
				else
					pControl = (T*)CCScale9Sprite::createWithSpriteFrameName(pKVFile->getValue().getCString(),rc);
			}
		}
		
		CC_SAFE_DELETE(pKVFile);

		// 去除file属性
		pDataElementKV->Del(pos);
	}
	else
	{
		pControl = T::create();
	}

	if (!LoadControl<T>(pThis,pControl,pDataElementKV))
	{
		CC_SAFE_DELETE(pDataElementKV);
		CC_SAFE_RELEASE(pControl);
		return 0;
	}
	CC_SAFE_DELETE(pDataElementKV);
	return pControl;
}

// 遍历查找指定属性并返回属性偏移
// 返回-1为属性不存在
int SIX_UIMgr::GetAttributeAndPos(SIX_DSTPL<SIX_KV> *pDataElement,const char *attribute,SIX_KV *pResult)
{
	int pos=-1;
	SIX_KV *pKV = 0;
	unsigned int j=0;
	if (!attribute || !pDataElement || !pResult)
		goto clean;

	CCARRAY_FOREACH_T(pDataElement->GetChildren(),pKV,SIX_KV*)
	{
		if (!pKV->getKey().compareIgnoreCase(attribute))
		{
			pResult->setKey(pKV->getKey().getCString());
			pResult->setValue(pKV->getValue().getCString());
			pos=j;
			goto clean;
		}
		j++;
	}
clean:
	return pos;
}

// CCNode属性
bool SIX_UIMgr::ParseAttribute(CCNode *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("Name"))
	{
		pControl->setObjName(pKV->getValue().getCString());
	}
	// 边框重绘，调试时使用
	// 父节点调用dirty时，只影响自身和当前已有的子节点
	// 后面添加的子节点，只有在显示(绘制)时才会跟父节点同步
	// Cool.Cat
	else if (!pKV->getKey().compareIgnoreCase("dirty"))
	{
		pControl->setRectDirty(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("zOrder"))
	{
		pControl->_setZOrder(pKV->getValue().intValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Tag"))
	{
		pControl->setTag(pKV->getValue().intValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("UserData"))
	{
		const char *userData = pKV->getValue().getCString();
		pControl->setUserData((void*)userData);
	}
	else if (!pKV->getKey().compareIgnoreCase("Visible"))
	{
		pControl->setVisible(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("IgnoreAnchorPoint"))
	{
		pControl->ignoreAnchorPointForPosition(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Rotation"))
	{
		pControl->setRotation(pKV->getValue().floatValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Scale"))
	{
		pControl->setScale(pKV->getValue().floatValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("ScaleX"))
	{
		pControl->setScaleX(pKV->getValue().floatValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("ScaleY"))
	{
		pControl->setScaleY(pKV->getValue().floatValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("PositionX"))
	{
		pControl->setPositionX(pKV->getValue().floatValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("PositionY"))
	{
		pControl->setPositionY(pKV->getValue().floatValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("SkewX"))
	{
		pControl->setSkewX(pKV->getValue().floatValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("SkewY"))
	{
		pControl->setSkewY(pKV->getValue().floatValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Size") || 
			 !pKV->getKey().compareIgnoreCase("Pos")  ||
			 !pKV->getKey().compareIgnoreCase("AnchorPoint")
			)
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		switch (count)
		{
		case 2:
			{
				float x = atof(mp->at(0).c_str());
				float y = atof(mp->at(1).c_str());
				if (!pKV->getKey().compareIgnoreCase("Size"))
					pControl->setContentSize(CCSize(x,y));
				else if (!pKV->getKey().compareIgnoreCase("Pos"))
					pControl->setPosition(CCPoint(x,y));
				else if (!pKV->getKey().compareIgnoreCase("AnchorPoint"))
					pControl->setAnchorPoint(CCPoint(x,y));
				break;
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	// 挂载动作
	else if (!pKV->getKey().compareIgnoreCase("Action"))
	{
		CCAnimation *pAnimation = GetAnimationFromCache(pKV->getValue().getCString());
		if (pAnimation)
		{
			CCAnimate *pAnimate = CCAnimate::actionWithAnimation(pAnimation);
			CCRepeatForever *pAction = CCRepeatForever::actionWithAction(pAnimate);
			pControl->runAction(pAction);
		}
	}
	//// 自定义背景精灵
	//// 当成sprite添加到父节点，会导致节点后面的子节点不方便查找，去掉先~
	//// Cool.Cat@2013-04-04
	//else if (!pKV->getKey().compareIgnoreCase("bkimage"))
	//{
	//	CCSprite* pSprite = 0;

	//	MP *mp=new MP();
	//	int count = SIX_Utility::GetInstance()->Split("=",(char*)pKV->getValue().getCString(),mp);
	//	switch (count)
	//	{
	//		// bkimage="filePath"
	//	case 0:
	//		{
	//			pSprite = CCSprite::create(pKV->getValue().getCString());
	//			CCAssert(pSprite,"A.wtf");
	//			pSprite->setAnchorPoint(ccp(0,0));
	//			break;
	//		}
	//		// bkimage="filePath=X,Y"
	//	case 2:
	//		{
	//			pSprite = CCSprite::create(mp->at(0).c_str());
	//			CCAssert(pSprite,"B.wtf");

	//			MP *child = new MP();
	//			count = SIX_Utility::GetInstance()->Split(",",(char*)mp->at(1).c_str(),child);
	//			if (count==2)
	//			{
	//				float x = atof(child->at(0).c_str());
	//				float y = atof(child->at(1).c_str());
	//				pSprite->setAnchorPoint(ccp(x,y));
	//			}
	//			child->swap(MP());
	//			child->clear();
	//			CC_SAFE_DELETE(child);
	//			break;
	//		}
	//	}
	//	mp->swap(MP());
	//	mp->clear();
	//	CC_SAFE_DELETE(mp);

	//	CCAssert(pSprite,"C.wtf");
	//	pControl->addChild(pSprite);
	//}
	else
	{
		SIXLog("Attribute[%s][%s].Unknown",pKV->getKey().getCString(),pKV->getValue().getCString());
		return false;
	}

	return true;
}

// CCSpriteBatchNode属性
bool SIX_UIMgr::ParseAttribute(CCSpriteBatchNode *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	// alpha混合函数
	if (!pKV->getKey().compareIgnoreCase("BlendFunc"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==2)
		{
			ccBlendFunc blendFunc = {0};
			blendFunc.src = (GLenum)atoi(mp->at(0).c_str());
			blendFunc.dst = (GLenum)atoi(mp->at(1).c_str());
			pControl->setBlendFunc(blendFunc);
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCNode*)pControl,pKV))
			return false;
	}
	return true;
}

// CCScale9Sprite属性
bool SIX_UIMgr::ParseAttribute(CCScale9Sprite *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	// 拉伸后大小
	if (!pKV->getKey().compareIgnoreCase("PreferredSize"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		switch (count)
		{
		case 2:
			{
				float x = atof(mp->at(0).c_str());
				float y = atof(mp->at(1).c_str());
				if (!pKV->getKey().compareIgnoreCase("PreferredSize"))
					pControl->setPreferredSize(CCSize(x,y));
				break;
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	// 左内间距
	else if (!pKV->getKey().compareIgnoreCase("InsetLeft"))
	{
		pControl->setInsetLeft(pKV->getValue().floatValue());
	}
	// 上内间距
	else if (!pKV->getKey().compareIgnoreCase("InsetTop"))
	{
		pControl->setInsetTop(pKV->getValue().floatValue());
	}
	// 右内间距
	else if (!pKV->getKey().compareIgnoreCase("InsetRight"))
	{
		pControl->setInsetRight(pKV->getValue().floatValue());
	}
	// 下内间距
	else if (!pKV->getKey().compareIgnoreCase("InsetBottom"))
	{
		pControl->setInsetBottom(pKV->getValue().floatValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Opacity"))
	{
		pControl->setOpacity(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("OpacityModifyRGB"))
	{
		pControl->setOpacityModifyRGB(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Color"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());
			pControl->setColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				pControl->setOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCNode*)pControl,pKV))
			return false;
	}

	return true;
}

// CCProgressTimer属性
bool SIX_UIMgr::ParseAttribute(CCProgressTimer *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	// 当前百分比
	if (!pKV->getKey().compareIgnoreCase("Percentage"))
	{
		pControl->setPercentage(pKV->getValue().floatValue());
	}
	// 0=kCCProgressTimerTypeRadial	雷达扇形
	// 1=kCCProgressTimerTypeBar	进度条形
	else if (!pKV->getKey().compareIgnoreCase("Type"))
	{
		pControl->setType((CCProgressTimerType)pKV->getValue().intValue());
	}
	// 是否反向播放进度动画
	else if (!pKV->getKey().compareIgnoreCase("ReverseProgress"))
	{
		pControl->setReverseProgress(pKV->getValue().boolValue());
	}
	// 是否按反方向播放动画
	else if (!pKV->getKey().compareIgnoreCase("ReverseDirection"))
	{
		pControl->setReverseDirection(pKV->getValue().boolValue());
	}
	// Alpha
	else if (!pKV->getKey().compareIgnoreCase("Opacity"))
	{
		pControl->setOpacity(pKV->getValue().uintValue());
	}
	// Alpha
	else if (!pKV->getKey().compareIgnoreCase("OpacityModifyRGB"))
	{
		pControl->setOpacityModifyRGB(pKV->getValue().boolValue());
	}
	// BarChangeRate：
	// (1,0)=横向
	// (0,1)=纵向
	// Midpoint=中点位置
	// 如果是绕中心旋转的进度动画，它就是中心；
	// 如果是条形动画，它代表了UV随进度变化的起始位置：
	// (0,y)=在最左边
	// (1,y)=在最右边
	// (x,0)=在最上面
	// (x,1)=在最下面
	// (0.5,0.5)是在中心
	else if (!pKV->getKey().compareIgnoreCase("BarChangeRate")||
				!pKV->getKey().compareIgnoreCase("Midpoint") ||
				!pKV->getKey().compareIgnoreCase("AnchorPoint")
			)
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==2)
		{
			double x = atof(mp->at(0).c_str());
			double y = atof(mp->at(1).c_str());
			if (!pKV->getKey().compareIgnoreCase("BarChangeRate"))
				pControl->setBarChangeRate(ccp(x,y));
			else if (!pKV->getKey().compareIgnoreCase("Midpoint"))
				pControl->setMidpoint(ccp(x,y));
			else if (!pKV->getKey().compareIgnoreCase("AnchorPoint"))
				pControl->setAnchorPoint(ccp(x,y));
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else if (!pKV->getKey().compareIgnoreCase("Color"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());
			pControl->setColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				pControl->setOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCNode*)pControl,pKV))
			return false;
	}

	return true;
}

// CCSprite属性
bool SIX_UIMgr::ParseAttribute(CCSprite *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("AtlasIndex"))
	{
		pControl->setAtlasIndex(pKV->getValue().uintValue());
	}
	// Alpha
	else if (!pKV->getKey().compareIgnoreCase("Opacity"))
	{
		pControl->setOpacity(pKV->getValue().uintValue());
	}
	// Alpha
	else if (!pKV->getKey().compareIgnoreCase("OpacityModifyRGB"))
	{
		pControl->setOpacityModifyRGB(pKV->getValue().boolValue());
	}
	// 所有子对象重绘
	else if (!pKV->getKey().compareIgnoreCase("DirtyRecursively"))
	{
		pControl->setDirtyRecursively(pKV->getValue().boolValue());
	}
	// X轴翻转
	else if (!pKV->getKey().compareIgnoreCase("FlipX"))
	{
		pControl->setFlipX(pKV->getValue().boolValue());
	}
	// Y轴翻转
	else if (!pKV->getKey().compareIgnoreCase("FlipY"))
	{
		pControl->setFlipY(pKV->getValue().boolValue());
	}
	// 旋转角度
	else if (!pKV->getKey().compareIgnoreCase("Rotation"))
	{
		pControl->setRotation(pKV->getValue().floatValue());
	}
	// xy拉伸
	else if (!pKV->getKey().compareIgnoreCase("Scale"))
	{
		pControl->setScale(pKV->getValue().floatValue());
	}
	// x拉伸
	else if (!pKV->getKey().compareIgnoreCase("ScaleX"))
	{
		pControl->setScaleX(pKV->getValue().floatValue());
	}
	// y拉伸
	else if (!pKV->getKey().compareIgnoreCase("ScaleY"))
	{
		pControl->setScaleY(pKV->getValue().floatValue());
	}
	// x倾斜
	else if (!pKV->getKey().compareIgnoreCase("SkewX"))
	{
		pControl->setSkewX(pKV->getValue().floatValue());
	}
	// y倾斜
	else if (!pKV->getKey().compareIgnoreCase("SkewY"))
	{
		pControl->setSkewY(pKV->getValue().floatValue());
	}
	// 是否可见
	else if (!pKV->getKey().compareIgnoreCase("Visible"))
	{
		pControl->setVisible(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Pos")  ||
			 !pKV->getKey().compareIgnoreCase("AnchorPoint")
			)
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		switch (count)
		{
		case 2:
			{
				float x = atof(mp->at(0).c_str());
				float y = atof(mp->at(1).c_str());
				if (!pKV->getKey().compareIgnoreCase("Pos"))
					pControl->setPosition(CCPoint(x,y));
				else if (!pKV->getKey().compareIgnoreCase("AnchorPoint"))
					pControl->setAnchorPoint(CCPoint(x,y));
				break;
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else if (!pKV->getKey().compareIgnoreCase("Color"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());
			pControl->setColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				pControl->setOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCNode*)pControl,pKV))
			return false;
	}

	return true;
}

// CCLabelTTF属性
bool SIX_UIMgr::ParseAttribute(CCLabelTTF *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	// 文本
	if (!pKV->getKey().compareIgnoreCase("String"))
	{
		pControl->setString(pKV->getValue().getCString());
	}
	// 字体
	else if (!pKV->getKey().compareIgnoreCase("fontName"))
	{
		pControl->setFontName(SIX_Utility::GetInstance()->U2G(pKV->getValue().getCString()).c_str());
	}
	// 字体大小
	else if (!pKV->getKey().compareIgnoreCase("fontSize"))
	{
		pControl->setFontSize(pKV->getValue().floatValue());
	}
	// 横向对齐
	else if (!pKV->getKey().compareIgnoreCase("HorizontalAlignment"))
	{
		pControl->setHorizontalAlignment((CCTextAlignment)pKV->getValue().intValue());
	}
	// 纵向对齐
	else if (!pKV->getKey().compareIgnoreCase("VerticalAlignment"))
	{
		pControl->setVerticalAlignment((CCVerticalTextAlignment)pKV->getValue().intValue());
	}
	// 字体维度
	else if (!pKV->getKey().compareIgnoreCase("Dimensions"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==2)
		{
			float w = atof(mp->at(0).c_str());
			float h = atof(mp->at(1).c_str());
			pControl->setDimensions(CCSize(w,h));
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCSprite*)pControl,pKV))
			return false;
	}

	return true;
}

// CCMenuItem属性
bool SIX_UIMgr::ParseAttribute(CCMenuItem *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("Enabled"))
	{
		pControl->setEnabled(pKV->getValue().boolValue());
	}
	else
	{
		if (!ParseAttribute((CCNode*)pControl,pKV))
			return false;
	}
	return true;
}

// CCMenuItemToggle属性
bool SIX_UIMgr::ParseAttribute(CCMenuItemToggle *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("Enabled"))
	{
		pControl->setEnabled(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Opacity"))
	{
		pControl->setOpacity(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("OpacityModifyRGB"))
	{
		pControl->setOpacityModifyRGB(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Color"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());
			pControl->setColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				pControl->setOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCMenuItem*)pControl,pKV))
			return false;
	}
	return true;
}

// CCMenuItemSprite属性
bool SIX_UIMgr::ParseAttribute(CCMenuItemSprite *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("Enabled"))
	{
		pControl->setEnabled(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Opacity"))
	{
		pControl->setOpacity(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("OpacityModifyRGB"))
	{
		pControl->setOpacityModifyRGB(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Color"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());
			pControl->setColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				pControl->setOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCMenuItem*)pControl,pKV))
			return false;
	}

	return true;
}

// CCMenuItemImage属性
bool SIX_UIMgr::ParseAttribute(CCMenuItemImage *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!ParseAttribute((CCMenuItemSprite*)pControl,pKV))
		return false;

	return true;
}

// CCMenuItemLabel属性
bool SIX_UIMgr::ParseAttribute(CCMenuItemLabel *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("String"))
	{
		pControl->setString(pKV->getValue().getCString());
	}
	else if (!pKV->getKey().compareIgnoreCase("Enabled"))
	{
		pControl->setEnabled(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Opacity"))
	{
		pControl->setOpacity(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Color") ||
			 !pKV->getKey().compareIgnoreCase("DisabledColor")
			)
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());
			if (!pKV->getKey().compareIgnoreCase("Color"))
				pControl->setColor(ccc3(r,g,b));
			else if (!pKV->getKey().compareIgnoreCase("DisabledColor"))
				pControl->setDisabledColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				pControl->setOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCMenuItem*)pControl,pKV))
			return false;
	}
	return true;
}

// CCMenuItemAtlasFont属性
bool SIX_UIMgr::ParseAttribute(CCMenuItemAtlasFont *pControl,SIX_KV *pKV)
{
	return ParseAttribute((CCMenuItemLabel*)pControl,pKV);
}

// CCMenuItemFont属性
bool SIX_UIMgr::ParseAttribute(CCMenuItemFont *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("FontSize"))
	{
		pControl->setFontSize(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("FontName"))
	{
		pControl->setFontName(pKV->getValue().getCString());
	}
	else if (!pKV->getKey().compareIgnoreCase("FontSizeObj"))
	{
		pControl->setFontSizeObj(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("FontNameObj"))
	{
		pControl->setFontNameObj(pKV->getValue().getCString());
	}
	else
	{
		if (!ParseAttribute((CCMenuItemLabel*)pControl,pKV))
			return false;
	}

	return true;
}

// CCLayer属性
bool SIX_UIMgr::ParseAttribute(CCLayer *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("TouchEnabled"))
	{
		pControl->setTouchEnabled(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("KeypadEnabled"))
	{
		pControl->setKeypadEnabled(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("AccelerometerEnabled"))
	{
		pControl->setAccelerometerEnabled(pKV->getValue().boolValue());
	}
	else
	{
		if (!ParseAttribute((CCNode*)pControl,pKV))
			return false;
	}

	return true;
}

// CCMenu属性
bool SIX_UIMgr::ParseAttribute(CCMenu *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("Enabled"))
	{
		pControl->setEnabled(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("HandlerPriority"))
	{
		pControl->setHandlerPriority(pKV->getValue().intValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Opacity"))
	{
		pControl->setOpacity(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Color"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());
			pControl->setColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				pControl->setOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCLayer*)pControl,pKV))
			return false;
	}

	return true;
}

// CCLayerMultiplex属性
bool SIX_UIMgr::ParseAttribute(CCLayerMultiplex *pControl,SIX_KV *pKV)
{
	return ParseAttribute((CCLayer*)pControl,pKV);
}

// CCLayerColor属性
bool SIX_UIMgr::ParseAttribute(CCLayerColor *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("size"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==2)
		{
			double x = atof(mp->at(0).c_str());
			double y = atof(mp->at(1).c_str());
			pControl->setContentSize(CCSize(x,y));
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else if (!pKV->getKey().compareIgnoreCase("Opacity"))
	{
		pControl->setOpacity(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("OpacityModifyRGB"))
	{
		pControl->setOpacityModifyRGB(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Color"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());
			pControl->setColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				pControl->setOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCLayer*)pControl,pKV))
			return false;
	}

	return true;
}

// CCLayerGradient属性
bool SIX_UIMgr::ParseAttribute(CCLayerGradient *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("CompressedInterpolation"))
	{
		pControl->setCompressedInterpolation(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("StartOpacity"))
	{
		pControl->setStartOpacity(pKV->getValue().uintValue());
	}
	if (!pKV->getKey().compareIgnoreCase("EndOpacity"))
	{
		pControl->setEndOpacity(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("StartColor") ||
			 !pKV->getKey().compareIgnoreCase("EndColor"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());

			if (!pKV->getKey().compareIgnoreCase("StartColor"))
				pControl->setStartColor(ccc3(r,g,b));
			else if (!pKV->getKey().compareIgnoreCase("EndColor"))
				pControl->setEndColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				if (!pKV->getKey().compareIgnoreCase("StartColor"))
					pControl->setStartOpacity(a);
				else if (!pKV->getKey().compareIgnoreCase("EndColor"))
					pControl->setEndOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCLayerColor*)pControl,pKV))
			return false;	
	}

	return true;
}

// CCControl属性
bool SIX_UIMgr::ParseAttribute(CCControl *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("DefaultTouchPriority"))
	{
		pControl->setDefaultTouchPriority(pKV->getValue().intValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Enabled"))
	{
		pControl->setEnabled(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Highlighted"))
	{
		pControl->setHighlighted(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Selected"))
	{
		pControl->setSelected(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Opacity"))
	{
		pControl->setOpacity(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("OpacityModifyRGB"))
	{
		pControl->setOpacityModifyRGB(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Color"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());
			pControl->setColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				pControl->setOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	// 增加事件解析？
	else if (!pKV->getKey().compareIgnoreCase("Events"))
	{
		pControl->addTargetWithActionForControlEvents(this,cccontrol_selector(SIX_UIMgr::doEvents),pKV->getValue().intValue());
	}
	else
	{
		if (!ParseAttribute((CCLayer*)pControl,pKV))
			return false;
	}

	return true;
}

void SIX_UIMgr::addControlEvents(unsigned int controlEvents,CCControl *pContrl,CCObject *pTarget)
{
	if (!pContrl)
		return;

	if (!pTarget)
		pTarget = pContrl;

	pContrl->addTargetWithActionForControlEvents(pTarget,cccontrol_selector(SIX_UIMgr::doEvents),controlEvents);
}
// CCControlButton属性
bool SIX_UIMgr::ParseAttribute(CCControlButton *pControl,SIX_KV *pKV)
{
	if (!pControl || !pKV)
		return false;

	if (!pKV->getKey().compareIgnoreCase("Enabled"))
	{
		pControl->setEnabled(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Selected"))
	{
		pControl->setSelected(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Highlighted"))
	{
		pControl->setHighlighted(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("PreferredSize"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		switch (count)
		{
		case 2:
			{
				float x = atof(mp->at(0).c_str());
				float y = atof(mp->at(1).c_str());
				if (!pKV->getKey().compareIgnoreCase("PreferredSize"))
					pControl->setPreferredSize(CCSize(x,y));
				break;
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else if (!pKV->getKey().compareIgnoreCase("ZoomOnTouchDown"))
	{
		pControl->setZoomOnTouchDown(pKV->getValue().boolValue());
	}
	// 增加缩放因子
	else if (!pKV->getKey().compareIgnoreCase("ZoomFactor"))
	{
		pControl->setZoomFactor(pKV->getValue().floatValue());
	}
	// 增加缩放步进
	else if (!pKV->getKey().compareIgnoreCase("ZoomStep"))
	{
		pControl->setZoomStep(pKV->getValue().floatValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("AdjustBackgroundImage"))
	{
		pControl->setAdjustBackgroundImage(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Margins") ||
				!pKV->getKey().compareIgnoreCase("LabelAnchorPoint"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==2)
		{
			if (!pKV->getKey().compareIgnoreCase("Margins"))
			{
				int x = atoi(mp->at(0).c_str());
				int y = atoi(mp->at(1).c_str());
				pControl->setMargins(x,y);
			}
			else if (!pKV->getKey().compareIgnoreCase("LabelAnchorPoint"))
			{
				double x = atof(mp->at(0).c_str());
				double y = atof(mp->at(1).c_str());
				pControl->setLabelAnchorPoint(ccp(x,y));
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else if (!pKV->getKey().compareIgnoreCase("Opacity"))
	{
		pControl->setOpacity(pKV->getValue().uintValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("OpacityModifyRGB"))
	{
		pControl->setOpacityModifyRGB(pKV->getValue().boolValue());
	}
	else if (!pKV->getKey().compareIgnoreCase("Color"))
	{
		MP *mp=new MP();
		int count = SIX_Utility::GetInstance()->Split(",",(char*)pKV->getValue().getCString(),mp);
		if (count==3 || count==4)
		{
			GLubyte r = (GLubyte)atoi(mp->at(0).c_str());
			GLubyte g = (GLubyte)atoi(mp->at(1).c_str());
			GLubyte b = (GLubyte)atoi(mp->at(2).c_str());
			((CCControlButton *)pControl)->setColor(ccc3(r,g,b));

			if (count==4)
			{
				GLubyte a = (GLubyte)atoi(mp->at(3).c_str());
				pControl->setOpacity(a);
			}
		}
#ifdef WIN32
		mp->swap(MP());
#endif
		mp->clear();
		CC_SAFE_DELETE(mp);
	}
	else
	{
		if (!ParseAttribute((CCControl*)pControl,pKV))
			return false;
	}

	return true;
}

bool SIX_UIMgr::GetAttributeSplit(const char *src,const char *name,string &value)
{
	if (!src || !name)
		return false;

	bool retv = false;
	MP *mp=new MP();
	int count = SIX_Utility::GetInstance()->Split("=",(char*)src,mp);
	if (count==2)
	{
		if (!STRCMP(mp->at(0).c_str(),name))
		{
			// name='value'
			int len = mp->at(1).length();
			if (len<2)
				goto clean;

			string x(mp->at(1).c_str(),1,mp->at(1).length()-2);
			value.append(x);
			retv = true;
		}
	}
clean:
#ifdef WIN32
	mp->swap(MP());
#endif
	mp->clear();
	CC_SAFE_DELETE(mp);
	return false;
}

SIX_DSTPL<SIX_KV> *SIX_UIMgr::ParseKV(const char *name,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (!name || !pDataElement)
		return 0;

	SIX_KV *pKV = new SIX_KV();
	int pos = GetAttributeAndPos(pDataElement,name,pKV);
	if (pos<0)
	{
		CC_SAFE_DELETE(pKV);
		return 0;
	}

	// format:string='xxx'\r\ncolor=''\r\n...
	SIX_DSTPL<SIX_KV> *pDataElementKV = new SIX_DSTPL<SIX_KV>;
	MP *mp=new MP();
	int count = SIX_Utility::GetInstance()->Split("\\r\\n",(char*)pKV->getValue().getCString(),mp);
	if (count==0)
	{
		MP *Space = new MP();
		int countSpace = SIX_Utility::GetInstance()->Split(" ",(char*)pKV->getValue().getCString(),Space);
		for (int i=0;i<countSpace;i++)
		{
			CCString *pElement = CCString::createWithFormat("%s",Space->at(i).c_str());
			MP *Arr = new MP();
			int countArr = SIX_Utility::GetInstance()->Split("=",(char*)pElement->getCString(),Arr);
			if (countArr==2)
			{
				int valueLen = Arr->at(1).length();
				// key='value'
				if (valueLen>2)
				{
					string valueFixed(Arr->at(1).c_str(),1,Arr->at(1).length()-2);
					SIX_KV *pKVAttr = new SIX_KV();
					pKVAttr->setKey(Arr->at(0).c_str());
					pKVAttr->setValue(valueFixed.c_str());
					pDataElementKV->Add(pKVAttr);
				}
			}
			CC_SAFE_DELETE(pElement);
	#ifdef WIN32
			Arr->swap(MP());
	#endif
			Arr->clear();
			CC_SAFE_DELETE(Arr);
		}
#ifdef WIN32
		Space->swap(MP());
#endif
		Space->clear();
		CC_SAFE_DELETE(Space);
	}
	else
	{
		for (int i=0;i<count;i++)
		{
			MP *Arr = new MP();
			int countArr = SIX_Utility::GetInstance()->Split("=",(char*)mp->at(i).c_str(),Arr);
			if (countArr==2)
			{
				int valueLen = Arr->at(1).length();
				// key='value'
				if (valueLen>2)
				{
					string valueFixed(Arr->at(1).c_str(),1,Arr->at(1).length()-2);
					SIX_KV *pKVAttr = new SIX_KV();
					pKVAttr->setKey(Arr->at(0).c_str());
					pKVAttr->setValue(valueFixed.c_str());
					pDataElementKV->Add(pKVAttr);
				}
			}
#ifdef WIN32
			Arr->swap(MP());
#endif
			Arr->clear();
			CC_SAFE_DELETE(Arr);
		}
	}

	CC_SAFE_DELETE(pKV);

#ifdef WIN32
	mp->swap(MP());
#endif
	mp->clear();
	CC_SAFE_DELETE(mp);

	// 移除指定的属性
	pDataElement->Del(pos);

	return pDataElementKV;
}

void SIX_UIMgr::LoadAnimationCache(SIX_XmlDataSet *pDataSet)
{
	if (!pDataSet->pDataElement)
		return;

	int buflen = 0;
	char animationName[64] = {0};

	// Name
	SIX_KV *pKV = new SIX_KV();
	int pos = GetAttributeAndPos(pDataSet->pDataElement,"Name",pKV);
	if (pos>=0)
	{
		buflen = pKV->getValue().length();
		memcpy(animationName,pKV->getValue().getCString(),buflen);
		animationName[buflen] = 0;
	}
	CC_SAFE_DELETE(pKV);
	pDataSet->pDataElement->Del(pos);

	// 解析动画帧
	CCAnimation *pAnimation = (CCAnimation*)ParseObject(pDataSet,"Animation");

	// save
	m_AnimationCache.setObject(pAnimation,animationName);
	//SIXLog("LoadActionCache.pAnimation[0x%08X][%s]..OK!",pAnimation,animationName);
}

CCAnimation *SIX_UIMgr::GetAnimationFromCache(const char *animationName)
{
	return (CCAnimation*)m_AnimationCache.objectForKey(animationName);
}

void SIX_UIMgr::UnloadAnimationCache()
{
	m_AnimationCache.removeAllObjects();
}

void SIX_UIMgr::LoadFrameCache(SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (!pDataElement)
		return;

	bool autorelease = true;
	SIX_KV *pKV = new SIX_KV();
	int pos = GetAttributeAndPos(pDataElement,"autorelease",pKV);
	if (pos>=0)
	{
		autorelease = pKV->getValue().boolValue();
	}

	CC_SAFE_DELETE(pKV);
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"file",pKV);
	if (pos>=0)
	{
		//SIXLog("m_FrameCache=%s...Loaded!",pKV->getValue().getCString());
		//CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(pKV->getValue().getCString());

		// save
		//m_FrameCache.push_back(pKV->getValue().getCString());
		FRAMECACHE_INFO *pFrameCacheInfo = new FRAMECACHE_INFO();
		pFrameCacheInfo->m_autorelease = autorelease;
		pFrameCacheInfo->m_file.append(pKV->getValue().getCString());
		SIX_CacheMgr::GetInstance()->Load(this->GetParent(),pFrameCacheInfo);
	}

	CC_SAFE_DELETE(pKV);

	// 去除file属性
	pDataElement->Del(pos);
}

void SIX_UIMgr::UnloadFrameCache()
{
//	// remove from SpriteFrameCache
//	int idx = 0;
//	vector<string>::iterator m_iter = m_FrameCache.begin();
//	for (m_iter;m_iter!=m_FrameCache.end();)
//	{	
//		//SIXLog("m_FrameCache[%d]=%s...Removed!",idx,m_FrameCache.at(0).c_str());
//		CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile(m_FrameCache.at(0).c_str());
//		m_iter = m_FrameCache.erase(m_iter);
//		idx++;
//	}
//
//#ifdef WIN32
//	m_FrameCache.swap(vector<string>());
//#endif
//	m_FrameCache.clear();

	SIX_CacheMgr::GetInstance()->Unload(this->GetParent());
}

// Sprite支持以下两种方式加载
// 1、File
// 2、SpriteFrame
CCSprite *SIX_UIMgr::CreateSprite(CCNode *pControl,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (pControl || !pDataElement)
		return 0;

	// 检查Sprite是否是从File加载
	SIX_KV *pKV = new SIX_KV();
	int pos = GetAttributeAndPos(pDataElement,"File",pKV);
	if (pos>=0)
	{
		SIX_KV *pKVRect = new SIX_KV();
		int posRect = GetAttributeAndPos(pDataElement,"rect",pKVRect);
		if (posRect>=0)
		{
			float x = 0.0f;
			float y = 0.0f;
			float width = 0.0f;
			float height = 0.0f;
			MP *mp=new MP();
			int count = SIX_Utility::GetInstance()->Split(",",(char*)pKVRect->getValue().getCString(),mp);
			if (count==4)
			{
				x = atof(mp->at(0).c_str());
				y = atof(mp->at(1).c_str());
				width = atof(mp->at(2).c_str());
				height = atof(mp->at(3).c_str());
			}
#ifdef WIN32
			mp->swap(MP());
#endif
			mp->clear();
			CC_SAFE_DELETE(mp);

			CCRect rc = CCRect(x,y,width,height);
			pControl = CCSprite::create(pKV->getValue().getCString(),rc);
			// 去除File属性
			pDataElement->Del(posRect);
		}
		else
			pControl = CCSprite::create(pKV->getValue().getCString());
		CC_SAFE_DELETE(pKVRect);
		CC_SAFE_DELETE(pKV);
		// 去除File属性
		pDataElement->Del(pos);
	}
	// 从SpriteFrame加载
	else
	{
		CC_SAFE_DELETE(pKV);

		/*********************************************************
		检查是否是从SpriteFrame加载
		自身属性：Frame/Action必须指定一个
		*********************************************************/
		// 默认初始帧(Frame)
		pKV = new SIX_KV();
		pos = GetAttributeAndPos(pDataElement,"Frame",pKV);
		if (pos>=0)
		{
			// Frame必须存在于SpriteFrameCache中
			CCSpriteFrame *pSpriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pKV->getValue().getCString());
			if (!pSpriteFrame)
			{
				CC_SAFE_DELETE(pKV);
				SIXLog("CreateSprite.SpriteFrame[%s].Nil!",pKV->getValue().getCString());
				return 0;
			}

			pControl = CCSprite::createWithSpriteFrame(pSpriteFrame);
			if (!pControl)
			{
				CC_SAFE_DELETE(pKV);
				SIXLog("CreateSprite.SpriteFrame.Create.Failed!");
				return 0;
			}

			// 去除DefaultFrame属性
			pDataElement->Del(pos);
		}
		CC_SAFE_DELETE(pKV);
	}

	// 再加载属性
	if (LoadControl<CCSprite>(this,(CCSprite*)pControl,pDataElement))
		return (CCSprite*)pControl;
	return 0;
}

CCScale9Sprite *SIX_UIMgr::CreateScale9Sprite(CCNode *pControl,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (pControl || !pDataElement)
		return 0;

	// 检查Sprite是否是从File加载
	SIX_KV *pKV = new SIX_KV();
	int pos = GetAttributeAndPos(pDataElement,"File",pKV);
	if (pos>=0)
	{
		SIX_KV *pKVRect = new SIX_KV();
		int posRect = GetAttributeAndPos(pDataElement,"rect",pKVRect);
		if (posRect>=0)
		{
			float x = 0.0f;
			float y = 0.0f;
			float width = 0.0f;
			float height = 0.0f;
			MP *mp=new MP();
			int count = SIX_Utility::GetInstance()->Split(",",(char*)pKVRect->getValue().getCString(),mp);
			if (count==4)
			{
				x = atof(mp->at(0).c_str());
				y = atof(mp->at(1).c_str());
				width = atof(mp->at(2).c_str());
				height = atof(mp->at(3).c_str());
			}
#ifdef WIN32
			mp->swap(MP());
#endif
			mp->clear();
			CC_SAFE_DELETE(mp);

			CCRect rc = CCRect(x,y,width,height);
			pControl = CCScale9Sprite::create(pKV->getValue().getCString());
			// 去除File属性
			pDataElement->Del(posRect);
		}
		else
			pControl = CCSprite::create(pKV->getValue().getCString());
		CC_SAFE_DELETE(pKVRect);
		CC_SAFE_DELETE(pKV);
		// 去除File属性
		pDataElement->Del(pos);
	}
	// 从SpriteFrame加载
	else
	{
		CC_SAFE_DELETE(pKV);

		/*********************************************************
		检查是否是从SpriteFrame加载
		自身属性：Frame/Action必须指定一个
		*********************************************************/
		// 默认初始帧(Frame)
		pKV = new SIX_KV();
		pos = GetAttributeAndPos(pDataElement,"Frame",pKV);
		if (pos>=0)
		{
			// Frame必须存在于SpriteFrameCache中
			CCSpriteFrame *pSpriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pKV->getValue().getCString());
			if (!pSpriteFrame)
			{
				CC_SAFE_DELETE(pKV);
				SIXLog("CreateSprite.SpriteFrame[%s].Nil!",pKV->getValue().getCString());
				return 0;
			}

			pControl = CCScale9Sprite::createWithSpriteFrame(pSpriteFrame);
			if (!pControl)
			{
				CC_SAFE_DELETE(pKV);
				SIXLog("CreateSprite.SpriteFrame.Create.Failed!");
				return 0;
			}

			// 去除DefaultFrame属性
			pDataElement->Del(pos);
		}
		CC_SAFE_DELETE(pKV);
	}

	if (LoadControl<CCScale9Sprite>(this,(CCScale9Sprite*)pControl,pDataElement))
		return (CCScale9Sprite*)pControl;
	return 0;
}

CCAnimation *SIX_UIMgr::CreateAnimation(CCObject *pObject,SIX_XmlDataSet *pDataSet)
{
	if (pObject || !pDataSet || !pDataSet->pDataElement)
		return 0;

	// 循环次数
	int Loops = 0;
	SIX_KV *pKV = new SIX_KV();
	int pos = GetAttributeAndPos(pDataSet->pDataElement,"Loops",pKV);
	if (pos>=0)
		Loops = pKV->getValue().intValue();
	CC_SAFE_DELETE(pKV);

	// 帧间隔
	float DelayPerUnit = 0.0;
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataSet->pDataElement,"DelayPerUnit",pKV);
	if (pos>=0)
		DelayPerUnit = pKV->getValue().floatValue();
	CC_SAFE_DELETE(pKV);

	// 恢复第一帧
	bool RestoreOriginalFrame = false;
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataSet->pDataElement,"RestoreOriginalFrame",pKV);
	if (pos>=0)
		RestoreOriginalFrame = pKV->getValue().boolValue();
	CC_SAFE_DELETE(pKV);

	// 遍历所有帧数组
	int frameCount = 0;
	CCArray *pArray = CCArray::array();
	SIX_XmlDataSet *pSpriteFrameDataSet = 0;
	CCARRAY_FOREACH_T(pDataSet->pNode->GetChildren(),pSpriteFrameDataSet,SIX_XmlDataSet*)
	{
		// 解析帧数组
		CCSpriteFrame *pSpriteFrame = (CCSpriteFrame*)ParseObject(pSpriteFrameDataSet,"SpriteFrame");
		if (pSpriteFrame)
		{
			pArray->addObject(pSpriteFrame);
			// 总帧数自增
			frameCount++;
		}
	}
	// 帧数组有内容的话
	if (pArray->count())
	{
		pObject = CCAnimation::animationWithSpriteFrames(pArray,DelayPerUnit/*,Loops*/);
		if (pObject)
			((CCAnimation*)pObject)->setRestoreOriginalFrame(RestoreOriginalFrame);
	}
	CC_SAFE_DELETE(pArray);

	return (CCAnimation*)pObject;
}

CCSpriteFrame *SIX_UIMgr::CreateSpriteFrame(CCObject *pObject,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (pObject || !pDataElement)
		return 0;

	SIX_KV *pKV = new SIX_KV();
	int pos = GetAttributeAndPos(pDataElement,"name",pKV);
	if (pos<0)
	{
		CC_SAFE_DELETE(pKV);
		return 0;
	}

	SIX_KV *pKVFile = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"file",pKVFile);
	if (pos<0)
	{
		pObject = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pKV->getValue().getCString());
	}
	else
	{
		CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage(pKVFile->getValue().getCString());
		if (pTexture)
		{
			CCRect rect = CCRectZero;
			rect.size = pTexture->getContentSize();
			pObject = CCSpriteFrame::createWithTexture(pTexture,rect);
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(dynamic_cast<CCSpriteFrame*>(pObject),pKV->getValue().getCString());
		}
	}

	CC_SAFE_DELETE(pKVFile);
	CC_SAFE_DELETE(pKV);

	//// 去除file属性
	//pDataElement->Del(pos);

	return (CCSpriteFrame*)pObject;
}

// -------------------
// 四态按钮
// -------------------
CCControlButton *SIX_UIMgr::CreateButton(CCNode *pControl,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (pControl || !pDataElement)
		return 0;

	// 九宫格背景
	CCScale9Sprite *bgImageNormal = CreateControl<CCScale9Sprite>(this,"norScale9Sprite",pDataElement);
	//// 至少得有个常态九宫格背景
	//if (!bgImageNormal)
	//	return 0;

	CCLabelTTF *pTTFNormal = CreateControl<CCLabelTTF>(this,"norLabelTTF",pDataElement);
	//// 至少得有个常态ttf
	//if (!pTTFNormal)
	//{
	//	CC_SAFE_RELEASE(bgImageNormal);
	//	return 0;
	//}

	// 来一个~
	if (pTTFNormal && bgImageNormal)
		pControl = CCControlButton::create(pTTFNormal,bgImageNormal);
	else if (pTTFNormal)
		pControl = CCControlButton::create(pTTFNormal,CCScale9Sprite::create());
	else if (bgImageNormal)
		pControl = CCControlButton::create(bgImageNormal);
	else
		pControl = CCControlButton::create("","",18);
	if (!pControl)
	{
		CC_SAFE_RELEASE(pTTFNormal);
		CC_SAFE_RELEASE(bgImageNormal);
		return 0;
	}

	// 掠过TTF
	CCLabelTTF *pTTFHover = CreateControl<CCLabelTTF>(this,"hovLabelTTF",pDataElement);
	if (pTTFHover)
		((CCControlButton*)pControl)->setTitleLabelForState(pTTFHover,CCControlStateHighlighted);
	// 掠过九宫格背景
	CCScale9Sprite *bgImageHover = CreateControl<CCScale9Sprite>(this,"hovScale9Sprite",pDataElement);
	if (bgImageHover)
		((CCControlButton*)pControl)->setBackgroundSpriteForState(bgImageHover,CCControlStateHighlighted);

	// 点击TTF
	CCLabelTTF *pTTFSelected = CreateControl<CCLabelTTF>(this,"selLabelTTF",pDataElement);
	if (pTTFSelected)
		((CCControlButton*)pControl)->setTitleLabelForState(pTTFSelected,CCControlStateSelected);
	// 点击九宫格背景
	CCScale9Sprite *bgImageSelected = CreateControl<CCScale9Sprite>(this,"selScale9Sprite",pDataElement);
	if (bgImageSelected)
		((CCControlButton*)pControl)->setBackgroundSpriteForState(bgImageSelected,CCControlStateSelected);

	// 禁用
	CCLabelTTF *pTTFDisabled = CreateControl<CCLabelTTF>(this,"disLabelTTF",pDataElement);
	if (pTTFDisabled)
		((CCControlButton*)pControl)->setTitleLabelForState(pTTFDisabled,CCControlStateDisabled);
	// 禁用九宫格背景
	CCScale9Sprite *bgImageDisabled = CreateControl<CCScale9Sprite>(this,"disScale9Sprite",pDataElement);
	if (bgImageDisabled)
		((CCControlButton*)pControl)->setBackgroundSpriteForState(bgImageDisabled,CCControlStateDisabled);

	// 再加载属性
	if (LoadControl<CCControlButton>(this,(CCControlButton*)pControl,pDataElement))
		return (CCControlButton*)pControl;
	return 0;
}

CCProgressTimer *SIX_UIMgr::CreateProgressTimer(CCNode *pControl,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (pControl || !pDataElement)
		return 0;

	CCSprite *spr = CreateControl<CCSprite>(this,"Sprite",pDataElement);
	if (!spr)
		return 0;

	pControl = CCProgressTimer::create(spr);
	if (!pControl)
	{
		CC_SAFE_RELEASE(spr);
		return 0;
	}

	// 再加载属性
	if (LoadControl<CCProgressTimer>(this,(CCProgressTimer*)pControl,pDataElement))
		return (CCProgressTimer*)pControl;
	return 0;
}

CCSpriteBatchNode *SIX_UIMgr::CreateSpriteBatchNode(CCNode *pControl,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (pControl || !pDataElement)
		return 0;

	// 先加载图片
	SIX_KV *pKV = new SIX_KV();
	int pos = GetAttributeAndPos(pDataElement,"file",pKV);
	if (pos<0)
	{
		CC_SAFE_DELETE(pKV);
		return 0;
	}

	pControl = CCSpriteBatchNode::batchNodeWithFile(pKV->getValue().getCString());

	CC_SAFE_DELETE(pKV);
	// 去除file属性
	pDataElement->Del(pos);

	// 再加载属性
	if (LoadControl<CCSpriteBatchNode>(this,(CCSpriteBatchNode*)pControl,pDataElement))
		return (CCSpriteBatchNode*)pControl;
	return 0;
}

SIX_NumberBatchNode *SIX_UIMgr::CreateNumberBatchNode(CCNode *pControl,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (pControl || !pDataElement)
		return 0;

	// 先加载图片
	SIX_KV *pKV = new SIX_KV();
	int pos = GetAttributeAndPos(pDataElement,"file",pKV);
	if (pos<0)
	{
		CC_SAFE_DELETE(pKV);
		return 0;
	}

	pControl = SIX_NumberBatchNode::Create(pKV->getValue().getCString());

	CC_SAFE_DELETE(pKV);
	// 去除file属性
	pDataElement->Del(pos);

	// cell
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"cell",pKV);
	if (pos<0)
	{
		CC_SAFE_DELETE(pKV);
		CC_SAFE_DELETE(pControl);
		return 0;
	}
	((SIX_NumberBatchNode*)pControl)->setCellNum(pKV->getValue().uintValue());
	CC_SAFE_DELETE(pKV);
	pDataElement->Del(pos);

	// alignstyle
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"alignstyle",pKV);
	if (pos<0)
	{
		CC_SAFE_DELETE(pKV);
		CC_SAFE_DELETE(pControl);
		return 0;
	}
	if (!STRCMP(pKV->getValue().getCString(),"left"))
		((SIX_NumberBatchNode*)pControl)->setCellAlignStyle(ALIGN_LEFT);
	else if (!STRCMP(pKV->getValue().getCString(),"center"))
		((SIX_NumberBatchNode*)pControl)->setCellAlignStyle(ALIGN_CENTER);
	else if (!STRCMP(pKV->getValue().getCString(),"right"))
		((SIX_NumberBatchNode*)pControl)->setCellAlignStyle(ALIGN_RIGHT);
	CC_SAFE_DELETE(pKV);
	pDataElement->Del(pos);

	// effectType
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"effectType",pKV);
	if (pos>=0)
	{
		((SIX_NumberBatchNode*)pControl)->setEffectType((EFFECT_TYPE)(pKV->getValue().intValue()));
		pDataElement->Del(pos);
	}
	CC_SAFE_DELETE(pKV);

	// effectValue
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"effectValue",pKV);
	if (pos>=0)
	{
		((SIX_NumberBatchNode*)pControl)->setEffectValue((pKV->getValue().floatValue()));
		pDataElement->Del(pos);
	}
	CC_SAFE_DELETE(pKV);

	// padding
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"padding",pKV);
	if (pos>=0)
	{
		((SIX_NumberBatchNode*)pControl)->setPadding(pKV->getValue().floatValue());
		pDataElement->Del(pos);
	}
	CC_SAFE_DELETE(pKV);

	// bitcount
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"bitcount",pKV);
	if (pos>=0)
	{
		((SIX_NumberBatchNode*)pControl)->setBitCount(pKV->getValue().uintValue());
		pDataElement->Del(pos);
	}
	CC_SAFE_DELETE(pKV);

	// durtime
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"durtime",pKV);
	if (pos>=0)
	{
		((SIX_NumberBatchNode*)pControl)->setDurtime(pKV->getValue().floatValue());
		pDataElement->Del(pos);
	}
	CC_SAFE_DELETE(pKV);

	// digit
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"digit",pKV);
	if (pos>=0)
	{
		((SIX_NumberBatchNode*)pControl)->setDigit(pKV->getValue().uintValue());
		pDataElement->Del(pos);
	}
	CC_SAFE_DELETE(pKV);

	// number
	pKV = new SIX_KV();
	pos = GetAttributeAndPos(pDataElement,"number",pKV);
	if (pos>=0)
	{
		((SIX_NumberBatchNode*)pControl)->setNumber(atol(pKV->getValue().getCString()));
		pDataElement->Del(pos);
	}
	CC_SAFE_DELETE(pKV);

	// 再加载属性
	if (LoadControl<CCSpriteBatchNode>(this,(CCSpriteBatchNode*)pControl,pDataElement))
		return (SIX_NumberBatchNode*)pControl;
	return 0;
}

SIX_ListView *SIX_UIMgr::CreateListView(CCNode *pControl,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (pControl || !pDataElement)
		return 0;

	// 解析容器

	// 解析滚动条

	// 解析自身属性
	int pViewDirection = 0;
	SIX_KV *pKV = new SIX_KV();
	int pos = GetAttributeAndPos(pDataElement,"ViewDirection",pKV);
	if (pos>=0)
	{
		pViewDirection = atoi(pKV->getValue().getCString());
		// 移除指定的属性
		pDataElement->Del(pos);
	}
	CC_SAFE_DELETE(pKV);
	
	ccColor4B color;
	ParseSingleAttribute(this,&color,"color",pDataElement);

	CCPoint offset;
	ParseSingleAttribute(this,&offset,"offset",pDataElement);

	CCSize size;
	ParseSingleAttribute(this,&size,"size",pDataElement);

	CCSize cellSize;
	ParseSingleAttribute(this,&cellSize,"cellSize",pDataElement);

	pControl = SIX_ListView::create(color,offset,size,cellSize,pViewDirection);

	if (!pControl)
		return 0;

	// 加载属性
	if (!LoadControl<CCLayerColor>(this,(SIX_ListView*)pControl,pDataElement))
		return 0;

	return (SIX_ListView*)pControl;
}

SIX_ListViewCell *SIX_UIMgr::CreateListViewCell(CCNode *pControl,SIX_DSTPL<SIX_KV> *pDataElement)
{
	if (pControl || !pDataElement)
		return 0;

	pControl = new SIX_ListViewCell();

	if (!pControl)
		return 0;

	// 加载属性
	if (!LoadControl<CCNode>(this,(SIX_ListViewCell*)pControl,pDataElement))
		return 0;

	return (SIX_ListViewCell*)pControl;
}

CCObject *SIX_UIMgr::ParseObject(SIX_XmlDataSet *pDataSet,const char *nodeName)
{
	if (!pDataSet || !nodeName)
		return 0;

	// 跳过Root和Comment
	if (pDataSet->getKey().compareIgnoreCase("Element"))
		return 0;

	CCObject *pObject = 0;

	// 如果自身有属性且匹配
	if (pDataSet->pDataElement && !pDataSet->getValue().compareIgnoreCase(nodeName))
	{
		if (!pDataSet->getValue().compareIgnoreCase("Animation"))
		{
			pObject = CreateAnimation(pObject,pDataSet);
		}
		else if (!pDataSet->getValue().compareIgnoreCase("SpriteFrame"))
		{
			pObject = CreateSpriteFrame(pObject,pDataSet->pDataElement);
		}
		else
		{
			SIXLog("SIX_UIMgr.ParseObject.pObject[%s].NoSupported!",pDataSet->getValue().getCString());
			return 0;
		}
	}

	CCAssert(pObject!=0,"SIX_UIMgr.ParseObject.pObject.Nil");

	return pObject;
}

CCNode *SIX_UIMgr::ParseControl(SIX_XmlDataSet *pDataSet,CCNode *pRoot)
{
	if (!pDataSet)
		return 0;

	// 跳过Root和Comment
	if (pDataSet->getKey().compareIgnoreCase("Element"))
		return 0;

	CCNode *pControl = 0;
	// 解析自身属性
	bool ControlIsValid = false;

	// 如果自身有属性
	if (pDataSet->pDataElement)
	{
		// 全局场景通用帧精灵缓存
		if (!pDataSet->getValue().compareIgnoreCase("SpriteFrameCache"))
		{
			 LoadFrameCache(pDataSet->pDataElement);
			 return 0;
		}
		// 当前场景通用动作组
		else if (!pDataSet->getValue().compareIgnoreCase("Animation"))
		{
			 LoadAnimationCache(pDataSet);
			 return 0;
		}
		// 下面的控件用LoadControl创建
		else if (!pDataSet->getValue().compareIgnoreCase("Node"))
		{
			pControl = CCNode::create();
			ControlIsValid = LoadControl<CCNode>(this,(CCNode*)pControl,pDataSet->pDataElement);
		}
		else if (!pDataSet->getValue().compareIgnoreCase("Control"))
		{
			pControl = new CCControl();
			((CCControl*)pControl)->init();
			ControlIsValid = LoadControl<CCControl>(this,(CCControl*)pControl,pDataSet->pDataElement);
		}
		else if (!pDataSet->getValue().compareIgnoreCase("Layer"))
		{
			pControl = CCLayer::create();
			ControlIsValid = LoadControl<CCLayer>(this,(CCLayer*)pControl,pDataSet->pDataElement);
		}
		else if (!pDataSet->getValue().compareIgnoreCase("LayerColor"))
		{
			pControl = CCLayerColor::create(ccc4(255,255,255,255));
			ControlIsValid = LoadControl<CCLayerColor>(this,(CCLayerColor*)pControl,pDataSet->pDataElement);
		}
		else if (!pDataSet->getValue().compareIgnoreCase("LabelTTF"))
		{
			pControl = CCLabelTTF::create();
			ControlIsValid = LoadControl<CCLabelTTF>(this,(CCLabelTTF*)pControl,pDataSet->pDataElement);
		}
		// 下面的控件要用CreateXXX创建
		else if (!pDataSet->getValue().compareIgnoreCase("Sprite"))
		{
			// Sprite所有子节点在CreateSprite中处理完成
			pControl = CreateSprite(pControl,pDataSet->pDataElement);
			if (pControl)
				ControlIsValid = true;
		}
		else if (!pDataSet->getValue().compareIgnoreCase("Scale9Sprite"))
		{
			pControl = CreateScale9Sprite(pControl,pDataSet->pDataElement);
			if (pControl)
				ControlIsValid = true;
		}
		else if (!pDataSet->getValue().compareIgnoreCase("Button"))
		{
			pControl = CreateButton(pControl,pDataSet->pDataElement);
			if (pControl)
				ControlIsValid = true;
		}
		else if (!pDataSet->getValue().compareIgnoreCase("ProgressTimer"))
		{
			pControl = CreateProgressTimer(pControl,pDataSet->pDataElement);
			if (pControl)
				ControlIsValid = true;
		}
		else if (!pDataSet->getValue().compareIgnoreCase("SpriteBatchNode"))
		{
			pControl = CreateSpriteBatchNode(pControl,pDataSet->pDataElement);
			if (pControl)
				ControlIsValid = true;
		}
		else if (!pDataSet->getValue().compareIgnoreCase("NumberBatchNode"))
		{
			pControl = CreateNumberBatchNode(pControl,pDataSet->pDataElement);
			if (pControl)
				ControlIsValid = true;
		}		
		else if (!pDataSet->getValue().compareIgnoreCase("ListView"))
		{
			pControl = CreateListView(pControl,pDataSet->pDataElement);
			if (pControl)
				ControlIsValid = true;
		}
		else if (!pDataSet->getValue().compareIgnoreCase("ListViewCell"))
		{
			pControl = CreateListViewCell(pControl,pDataSet->pDataElement);
			if (pControl)
			{
				// ListViewCell的父节点必须是ListView
				SIX_ListView *pFather = dynamic_cast<SIX_ListView*>(pRoot);
				if (!pFather)
				{
					CC_SAFE_DELETE(pControl);
					SIXLog("SIX_UIMgr.ParseControl[%s].pFather.Nil",pDataSet->getValue().getCString());
					CCMessageBox(pDataSet->getValue().getCString(),"Error");
					CCAssert(pControl!=0,"SIX_UIMgr.ParseControl.pFather.Nil");
					return 0;
				}

				// 用AddCell方式添加到ListView
				unsigned int idx = pFather->GetCellCount();
				pFather->AddCell((SIX_ListViewCell*)pControl,idx);

				ControlIsValid = true;
			}
		}
		else
		{
			SIXLog("SIX_UIMgr.ParseControl.pControl[%s].NoSupported!",pDataSet->getValue().getCString());
			return 0;
		}
	}

	if (pControl==0)
	{
		SIXLog("SIX_UIMgr.ParseControl[%s].pControl.Nil",pDataSet->getValue().getCString());
		CCMessageBox(pDataSet->getValue().getCString(),"Error");
		CCAssert(pControl!=0,"SIX_UIMgr.ParseControl.pControl.Nil");
		return 0;
	}

	if (ControlIsValid)
	{
		// 先加到父节点
		if (pRoot)
		{
			// 非ListViewCell节点才使用addChild方式
			if (pDataSet->getValue().compareIgnoreCase("ListViewCell"))
				pRoot->addChild(pControl);
		}

		// 再继续解析子节点
		if (pDataSet->pNode)
		{
			SIX_XmlDataSet *pChildDataSet = 0;
			CCARRAY_FOREACH_T(pDataSet->pNode->GetChildren(),pChildDataSet,SIX_XmlDataSet*)
			{
				ParseControl(pChildDataSet,pControl);
			}
		}
	}
	else
	{
		CC_SAFE_DELETE(pControl);
		SIXLog("SIX_UIMgr.ParseControl.KV[%s][%s].Failed!",
			pDataSet->getKey().getCString(),
			pDataSet->getValue().getCString()
		);
		return 0;
	}

	return pControl;
}

int SIX_UIMgr::ParseXML(SIX_DSTPL<SIX_XmlDataSet> *pXmlDataSet)
{
	//SIXLog("SIX_UIMgr.Parse=%d",pXmlDataSet->GetChildrenCount());
	SIX_XmlDataSet *pDataSet = 0;
	unsigned int i=0;

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	CCSize winSize = CCDirector::sharedDirector()->getWinSizeInPixels();

	CCARRAY_FOREACH_T(pXmlDataSet->GetChildren(),pDataSet,SIX_XmlDataSet*)
	{
		ParseControl(pDataSet,GetParent());
	}

	return 0;
}

int SIX_UIMgr::Run()
{
	return 0;
}

int SIX_UIMgr::Release()
{
	return 0;
}

void SIX_UIMgr::doEvents(CCObject* pSender, CCControlEvent event)
{
	// 传递消息给父场景
	m_Parent->doEvents(pSender,event);
	return;

	////// 脚本传递消息方式，勿删！
	////// Cool.Cat
	////CCLuaEngine* pEngine = CCLuaEngine::defaultEngine();
	////lua_State* m_state = pEngine->getLuaState();
	////lua_pushlightuserdata(m_state, pSender);
	////::lua_pushstring(m_state,pSender->getObjName());
	////switch (event)
	////{
	////	case CCControlEventTouchDown:			pEngine->executeGlobalFunction("onCCControlEventTouchDown",2);break;
	////	case CCControlEventTouchDragInside:		pEngine->executeGlobalFunction("onCCControlEventTouchDragInside",2);break;
	////	case CCControlEventTouchDragOutside:	pEngine->executeGlobalFunction("onCCControlEventTouchDragOutside",2);break;
	////	case CCControlEventTouchDragEnter:		pEngine->executeGlobalFunction("onCCControlEventTouchDragEnter",2);break;
	////	case CCControlEventTouchDragExit:		pEngine->executeGlobalFunction("onCCControlEventTouchDragExit",2);break;
	////	case CCControlEventTouchUpInside:		pEngine->executeGlobalFunction("onCCControlEventTouchUpInside",2);break;
	////	case CCControlEventTouchUpOutside:		pEngine->executeGlobalFunction("onCCControlEventTouchUpOutside",2);break;
	////	case CCControlEventTouchCancel:			pEngine->executeGlobalFunction("onCCControlEventTouchCancel",2);break;
	////	case CCControlEventValueChanged:		pEngine->executeGlobalFunction("onCCControlEventValueChanged",2);break;
	////	default:SIXLog("doEvents.Unknown[%d]",event);break;
	////}
	//switch (event)
	//{
	//	case CCControlEventTouchDown:			break;
	//	case CCControlEventTouchDragInside:		break;
	//	case CCControlEventTouchDragOutside:	break;
	//	case CCControlEventTouchDragEnter:		break;
	//	case CCControlEventTouchDragExit:		break;
	//	case CCControlEventTouchUpInside:		onCCControlEventTouchUpInside(pSender,event);break;
	//	case CCControlEventTouchUpOutside:		break;
	//	case CCControlEventTouchCancel:			break;
	//	case CCControlEventValueChanged:		break;
	//	default:SIXLog("doEvents.Unknown[%d]",event);break;
	//}
}

////弹起(鼠标在控件区域内)
//void SIX_UIMgr::onCCControlEventTouchUpInside(CCObject* pSender, CCControlEvent event)
//{
//	string name=pSender->getObjName();
//	if(name=="Help")
//	{
//		static int nCount=0;
//		switch ( nCount%7 )
//		{
//		case 0: 	/*g_FishMgr->addOneFish(0, 112,1,20.0f,0,0,CCPoint(200,300),CCPoint(1500,0),CCPoint(0,0),ccp(0,0)); break;*/
//		case 1: 	/*g_FishMgr->addOneFish(0, 2,2,20.0f,0,0,ccp(500,300),ccp(350,300),CCPoint(0,0),ccp(0,0)); break;*/
//		case 2: 	/*g_FishMgr->addOneFish(0, 1,3,20.0f,0,0,CCPoint(200,300),CCPoint(50,0),CCPoint(0,0),ccp(0,0)); break;*/
//		case 3:     /*g_FishMgr->addOneFish(0, 101,4,20.0f,0,0,CCPoint(0,0),CCPoint(0,0),CCPoint(0,0),ccp(0,0)); break;*/
//		case 4:     /*g_FishMgr->addOneFish(0, 107,2,20.0f,0,0,ccp(0,300),ccp(150,300),CCPoint(0,0),ccp(0,0)); break;*/
//		case 5:     /*g_FishMgr->addOneFish(0, 1,1,20.0f,0,0,ccp(0,100),ccp(1500,1500),CCPoint(0,0),ccp(0,0)); break;*/
//		case 6:     g_FishMgr->addOneFish(0,109,4,20.0f,0,0,0,CCPoint(0,0),CCPoint(0,-1),CCPoint(0,0),ccp(0,0)); break;
//		default:   break;
//		}
//		nCount++;
//
//	}
//	if(name=="Set")
//	{
//		static int nCount=0;
//		switch ( nCount%10 )
//		{
//		case 0: 	/*g_FishMgr->addOneGroupFish(33); break;*/
//		case 1: 	/*g_FishMgr->addOneGroupFish(21); break;*/
//		case 2: 	/*g_FishMgr->addOneGroupFish(12); break;*/
//		case 3:     /*g_FishMgr->addOneGroupFish(31); break;*/
//		case 4:     /*g_FishMgr->addOneGroupFish(11); break;*/
//		default:   g_FishMgr->SpeedUpAllFish();break;
//		}
//		nCount++;
//	}
//	if(name=="Supplement")
//	{
//		static int nCount=0;
//		switch ( nCount%10 )
//		{
//		//case 0: 	 break;
//		//default:   g_FishMgr->addOneFish(0, 1,5,5.0f,0,0,ccp(260,743),ccp(307,187),ccp(383,-202),ccp(1110,307));break;
//		default:   
//			{
//				//for(int i=0;i<20;i++)
//				//	g_FishMgr->addOneFish(0, 1,6,15.0f,0,0,ccp(512,325),ccp(10,M_PI*0.25*i),ccp(0,0),ccp(6,0));
//				g_FishMgr->SpeedUpOneFish(0);
//			}
//			break;
//		}
//		nCount++;
//	}		
//}

//void SIX_UIMgr::setAllDirty(bool bDirty)
//{
//	m_bDirty = bDirty;
//}
//
//bool SIX_UIMgr::getAllDirty()
//{
//	return m_bDirty;
//}