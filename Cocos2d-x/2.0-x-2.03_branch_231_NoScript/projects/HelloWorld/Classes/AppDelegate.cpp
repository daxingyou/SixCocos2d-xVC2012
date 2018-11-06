﻿//
//  XXDDZAppDelegate.cpp
//  XXDDZ
//
//  Created by XXY on 13-9-18.
//  Copyright __MyCompanyName__ 2013骞? All rights reserved.
//

#include "AppDelegate.h"

#include "cocos2d.h"
//#include "LoginView.h"
//#include "GameMusicController.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{

}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();
    CCEGLView *CCEGL = CCEGLView::sharedOpenGLView();
    pDirector->setOpenGLView(CCEGL);
    CCEGL->setDesignResolutionSize(960, 640, kResolutionExactFit);

    // turn on display FPS
    pDirector->setDisplayStats(false);
   
    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    //// create a scene. it's an autorelease object
    //CCScene *pScene = LoginView::scene();

    //// run
    //pDirector->runWithScene(pScene);


	//m_pGameEngine = new GameEngine();

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{

    CCDirector::sharedDirector()->pause();
    //SimpleAudioEngine::sharedEngine()->pauseAllEffects();
    //SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
    // if you use SimpleAudioEngine, it must be paused
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
 
     CCDirector::sharedDirector()->resume();
     //SimpleAudioEngine::sharedEngine()->resumeAllEffects();
     //SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
   // GameMusicController::sharedGameMusic()->pauseBackgroundMusic();
   // GameMusicController::sharedGameMusic()->stopAllEffects();
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
