#include <Geode/ui/GeodeUI.hpp>
#include "head.hpp"

#include <Geode/modify/UIOptionsLayer.hpp>
class $modify(UIOptionsLayer) {
	struct Fields {
		const CCSize size = CCDirector::sharedDirector()->getWinSize();

		// ui to hide
		GameOptionsLayer* opl;

		// title
        CCLabelBMFont* titleLabel;
		// planar menus
		PosInputBundle* posMenu;
		// input slider menus
		InputSliderBundle* widthMenu, * heightMenu, * scaleMenu, * opacityMenu, * dzMenu, * radiusMenu;
		// check boxes
        CCMenuItemToggler* modeBtn, * snapBtn, * splitBtn, * jumplBtn, * dualBtn;
		// map
		PlatformPreviewFrame* map;

		// radio
		EventListener<EventFilter<Signal>> radio;
        // pos radio
        EventListener<EventFilter<PosSignal>> radioPos;
	};

	bool init(bool p) {
		if (!SetupTriggerPopup::init(nullptr, nullptr, 420.f, 280.f, 1))
			return false;

		this->m_fields->radio = EventListener<EventFilter<Signal>>(
            [this](Signal* event) -> ListenerResult { return this->handleSignal(event); });

		this->setColor(Mod::get()->getSettingValue<ccColor3B>("bgcolor"));
		this->setOpacity(Mod::get()->getSettingValue<int64_t>("bgopacity"));

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

        m_fields->titleLabel = CCLabelBMFont::create("Platform UI Modifier", "goldFont.fnt");
        m_fields->titleLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        m_fields->titleLabel->setPosition(ccp(m_fields->size.width / 2, m_fields->size.height * 3 / 4 + 80.f));
        m_fields->titleLabel->setScale(0.35);
        this->m_mainLayer->addChild(m_fields->titleLabel);

		// the center map who can be scaled in game
		m_fields->map = PlatformPreviewFrame::create();
        m_fields->map->setPosition(ccp(50.f, 30.f));
        m_fields->map->setScale(0.25);
		//m_fields->map->placeNode(gm->m_practicePos);
        //m_fields->map->alphaNode(gm->m_practiceOpacity * 255);
		this->m_mainLayer->addChild(m_fields->map);

        // pos menu
		m_fields->posMenu = PosInputBundle::create();
		m_fields->posMenu->setPosition(ccp(50.f, m_fields->size.height / 4 - 20.f));
		m_fields->posMenu->setScale(0.5);
        m_fields->posMenu->setValue(gm->m_practicePos);
		this->m_mainLayer->addChild(m_fields->posMenu);

        // width menu
		m_fields->widthMenu = InputSliderBundle::create("width", -1, 0, 400.f, 0, true);
		m_fields->posMenu->setPosition(ccp(50.f, m_fields->size.height / 4 - 20.f));
		m_fields->posMenu->setScale(0.5);
        m_fields->posMenu->setValue(gm->m_practicePos);
		this->m_mainLayer->addChild(m_fields->posMenu);


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

	//
	void valueDidChange(int i, float f) override {
		UIOptionsLayer::valueDidChange(i, f);
		log::debug("int = {}, float = {}", i, f);
	}


	bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
		return SetupTriggerPopup::ccTouchBegan(touch, event);
	}

	void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
		
	}
	void ccTouchEnded(CCTouch* touch, CCEvent* event) override {

	}
	
    void onOptions(CCObject*) {
		geode::openSettingsPopup(Mod::get(), false);
	}

	void onPreview(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}

	void onSnap(CCObject*) { }

	void onReset(CCObject*) { }

	void onSaveLoad(CCObject*) { }

	void onClose(CCObject* obj) override {
		UIOptionsLayer::onClose(obj);
	}

	void keyBackClicked() override {
		UIOptionsLayer::keyBackClicked();
	}
};