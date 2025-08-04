#include <Geode/ui/GeodeUI.hpp>
#include "head.hpp"

// practice
#include <Geode/modify/UIPOptionsLayer.hpp>
class $modify(PracticeOptionsLayer, UIPOptionsLayer) {
	struct Fields {
		const CCSize size = CCDirector::sharedDirector()->getWinSize();

		// ui to hide
		GameOptionsLayer* opl;

		// title
        CCLabelBMFont* titleLabel;
		// input menu
		PosInputBundle* posMenu;
		// several slider menus
		InputSliderBundle* opacityMenu;

		// map
		PracticePreviewFrame* map;

		// radio
		EventListener<EventFilter<Signal>> radio;
        // planar radio
        EventListener<EventFilter<PosSignal>> radioPos;
	};

	bool init() override {
		if (!SetupTriggerPopup::init(nullptr, nullptr, 420.f, 280.f, 1))
			return false;

		this->m_fields->radio = EventListener<EventFilter<Signal>>(
            [this](Signal* event) -> ListenerResult { return this->handleSignal(event); });

		this->m_fields->radioPos = EventListener<EventFilter<PosSignal>>(
            [this](PosSignal* event) -> ListenerResult {
                this->m_fields->posMenu->setValue(event->pos);
                gm->m_practicePos = event->pos;
                return ListenerResult::Stop;
            });

		// game options layer below
		m_fields->opl = CCScene::get()->getChildByType<GameOptionsLayer>(0);

		// bg color
		this->setColor(Mod::get()->getSettingValue<ccColor3B>("bgcolor"));
		this->setOpacity(0);
		

		// hide the frame
		this->m_mainLayer->getChildByTag(1)->setVisible(false);

        m_fields->titleLabel = CCLabelBMFont::create("Practice UI Modifier", "goldFont.fnt");
        m_fields->titleLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        m_fields->titleLabel->setPosition(ccp(m_fields->size.width / 2, m_fields->size.height * 3 / 4 + 80.f));
        m_fields->titleLabel->setScale(0.35);
        this->m_mainLayer->addChild(m_fields->titleLabel);

		// the center map who can be scaled in game
		m_fields->map = PracticePreviewFrame::create();
        m_fields->map->setPosition(ccp(0.f, 30.f));
        m_fields->map->setScale(0.25);
		m_fields->map->placeNode(gm->m_practicePos);
        m_fields->map->alphaNode(gm->m_practiceOpacity * 255);
		this->m_mainLayer->addChild(m_fields->map);

		// relocate the practice node
		this->m_practiceNode = m_fields->map->getTargetNode();

		// pos menu
		m_fields->posMenu = PosInputBundle::create();
		m_fields->posMenu->setPositionY(m_fields->size.height / 4 - 20.f);
		m_fields->posMenu->setScale(0.5);
        m_fields->posMenu->setValue(gm->m_practicePos);
		this->m_mainLayer->addChild(m_fields->posMenu);

		// opacity menu
		m_fields->opacityMenu = InputSliderBundle::create("Opacity", -29, 0, 1, 2, true);
		m_fields->opacityMenu->setPositionY(m_fields->size.height / 4 - 50.f);
		m_fields->opacityMenu->setScale(0.5);
        m_fields->opacityMenu->setValue(gm->m_practiceOpacity);
		this->m_mainLayer->addChild(m_fields->opacityMenu);

		// buttons below
        std::map<const char*, SEL_MenuHandler> btnIndexes = {
            {"optionsBtn.png"_spr, menu_selector(PracticeOptionsLayer::onOptions)},
            {"fullscreenBtn.png"_spr, menu_selector(PracticeOptionsLayer::onPreview)},
            {"snapBtn.png"_spr, menu_selector(PracticeOptionsLayer::onSnap)},
            {"resetBtn.png"_spr, menu_selector(PracticeOptionsLayer::onReset)},
            {"applyBtn.png"_spr, menu_selector(PracticeOptionsLayer::onClose)}
        };

        for (auto [k, v] : btnIndexes) {
            auto spr = CCSprite::create(k);
            spr->setScale(0.6);
            auto btn = CCMenuItemSpriteExtra::create(spr, this, v);
            this->m_buttonMenu->addChild(btn);
        }

		m_buttonMenu->setPositionY(-5.f);
		m_buttonMenu->setScale(0.5);
        m_buttonMenu->setID("button-menu");

        this->m_buttonMenu->setLayout(
            RowLayout::create()
                ->setGap(15.f)
                ->setAxisAlignment(AxisAlignment::Center)
        );

		this->Transition(true, true);
		m_fields->map->helpTransition(true);

		return true;
	}

	ListenerResult handleSignal(Signal* event) {
		//log::debug("signal handled!");
		// opacity
		if (event->tag == -29) {
            this->valueDidChange(-29, event->value);
			//gm->m_practiceOpacity = event->value;
			//this->m_fields->map->alphaNode(event->value * 255);
			//log::debug("opacity is set to {}", event->value);
		}
		// x pos
		else if (event->tag == 114) {
            //log::debug("pos x is set to {}", event->value);
			GameManager::sharedState()->m_practicePos.x = event->value;
			this->m_fields->map->placeNode(ccp(event->value, this->m_practiceNode->getPositionY()));
		}
		// y pos
		else if (event->tag == 514) {
            //log::debug("pos y is set to {}", event->value);
			GameManager::sharedState()->m_practicePos.y = event->value;	
			this->m_fields->map->placeNode(ccp(this->m_practiceNode->getPositionX(), event->value));
		}

		// escape from fullscreen preview
		else if (event->tag == -100) {
			if (!event->value && this->m_fields->map->getScale() == 1) {
				this->Transition(true, false);
				m_fields->opl->setVisible(true);				
			}
		}

		return ListenerResult::Stop;		
	}

	void Transition(bool in, bool whole) {
		this->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.3, in * Mod::get()->getSettingValue<int64_t>("bgopacity"))));

		if (whole) {
			this->m_fields->map->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, 0.25 * (in + 1))));
		} else {
			this->m_fields->map->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, 1 - 0.5 * in)));
			this->m_fields->map->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.2, ccp(0.f, 30.f * in))));
		}

		m_fields->titleLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, 0.35 * (in + 1))));
		m_fields->titleLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3, ccp(m_fields->size.width / 2, m_fields->size.height * 3 / 4 + 100.f - in * 50.f))));

		m_fields->posMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, 0.5 * (in + 1))));
		m_fields->posMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3, ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 100.f + in * 110.f))));

		m_fields->opacityMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, 0.5 * (in + 1))));
		m_fields->opacityMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3, ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 100.f + in * 80.f))));

		m_buttonMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, 0.5 * (in + 1))));
		m_buttonMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3, ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 100.f + in * 45.f))));
	}

	bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        return SetupTriggerPopup::ccTouchBegan(touch, event);
    }

	void ccTouchMoved(CCTouch* touch, CCEvent* event) override { }

	void ccTouchEnded(CCTouch* touch, CCEvent* event) override { }
	
	void textChanged(CCTextInputNode* input) override { }

    void onOptions(CCObject*) {
		geode::openSettingsPopup(Mod::get(), false);
	}

	void onPreview(CCObject*) {
		this->Transition(false, false);
		m_fields->opl->setVisible(false);
	}

    void onSnap(CCObject*) {
        bool on = Mod::get()->getSavedValue<bool>("snap");
        Mod::get()->setSavedValue("snap", !on);
		this->m_fields->map->setGridVisibility(!on);
	}

	void onReset(CCObject* obj) {
		auto defpos = ccp(m_fields->size.width / 2, 40.f);
		gm->m_practicePos = defpos;
		gm->m_practiceOpacity = 1;
		this->m_fields->map->placeNode(defpos);
		this->m_fields->map->alphaNode(255, 0.2);
		this->m_fields->posMenu->setValue(defpos);
		this->m_fields->opacityMenu->setValue(1);
	}

	void onClose(CCObject* obj) override {
		if (m_fields->map->getScale() > 0.5) {
			this->Transition(true, false);
			m_fields->opl->setVisible(true);
		}
		else
			this->runAction(CCSequence::create(
				CallFuncExt::create([this] () { this->Transition(false, true); m_fields->map->helpTransition(false); }),
				CCDelayTime::create(0.3),
				CallFuncExt::create([this, obj] () { this->SetupTriggerPopup::onClose(obj); }),
				nullptr
			));
	}

	void keyBackClicked() override {
		if (m_fields->map->getScale() > 0.5) {
			this->Transition(true, false);
			m_fields->opl->setVisible(true);
		}
		else
			this->runAction(CCSequence::create(
				CallFuncExt::create([this] () { this->Transition(false, true); m_fields->map->helpTransition(false); }),
				CCDelayTime::create(0.3),
				CallFuncExt::create([this] () { this->SetupTriggerPopup::keyBackClicked(); }),
				nullptr
			));
	}
};