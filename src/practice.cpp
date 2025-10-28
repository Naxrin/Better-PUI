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
		PosInputBundle* posMenu, *pcpposMenu;
		// several slider menus
		InputSliderBundle* opacityMenu, * pcpScaleMenu;

		// button sprites
		CCMenuItemSpriteExtra* snapBtn;

		// map
		PracticePreviewFrame* map;

		// radio
		EventListener<EventFilter<Signal>> radio;
        // planar radio
        EventListener<EventFilter<PosSignal>> radioPos;
		
		// in preview mode
		bool in_prev;

		// pcp mode
		Mod* pcpmod = Loader::get()->getLoadedMod("kevadroz.practicecheckpointpermanence");
		bool pcp;
	};

	/*
	static void onModify(auto& self) {
		if (!self.setHookPriority("UIPOptionsLayer::init", Priority::Late))
			geode::log::warn("Failed to set hook priority.");		
	}*/

	bool init() override {
		if (!SetupTriggerPopup::init(nullptr, nullptr, 420.f, 280.f, 1))
			return false;

		this->m_fields->radio = EventListener<EventFilter<Signal>>(
            [this](Signal* event) -> ListenerResult { return this->handleSignal(event); });

		this->m_fields->radioPos = EventListener<EventFilter<PosSignal>>(
            [this](PosSignal* event) -> ListenerResult {
				if (m_fields->pcp != bool(event->tag)) {
					m_fields->pcp = event->tag;
					if (!m_fields->in_prev)
						this->switchPCP();
				}
				// pcppos
				if (event->tag)
					this->m_fields->pcpposMenu->setValue(event->pos);
				// my own
				else {
					this->m_fields->posMenu->setValue(event->pos);
					gm->m_practicePos = event->pos;
					if (auto pl = PlayLayer::get())
						pl->m_uiLayer->getChildByID("checkpoint-menu")->setPosition(event->pos);					
				}

                return ListenerResult::Stop;
            });

		// game options layer below
		m_fields->opl = CCScene::get()->getChildByType<GameOptionsLayer>(0);

		// bg color
		this->setColor(Mod::get()->getSettingValue<ccColor3B>("bg-color"));
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
		m_fields->map->setID("map");
		this->m_mainLayer->addChild(m_fields->map);

		// relocate the practice node
		this->m_practiceNode = m_fields->map->getChildByID("target");


		// pos menu
		m_fields->posMenu = PosInputBundle::create();
		m_fields->posMenu->setPositionY(m_fields->size.height / 4 - 130.f);
		m_fields->posMenu->setScale(0);
		m_fields->posMenu->setID("pos-menu");
        m_fields->posMenu->setValue(gm->m_practicePos);
		this->m_mainLayer->addChild(m_fields->posMenu);

		// opacity menu
		m_fields->opacityMenu = InputSliderBundle::create("Opacity", 0, 1, 2);
		m_fields->opacityMenu->setPositionY(m_fields->size.height / 4 - 50.f);
		m_fields->opacityMenu->setScale(0);
		m_fields->opacityMenu->setID("opacity-menu");
		m_fields->opacityMenu->setTag(0);
        m_fields->opacityMenu->setValue(gm->m_practiceOpacity);
		this->m_mainLayer->addChild(m_fields->opacityMenu);

		m_fields->pcpmod = Loader::get()->getLoadedMod("kevadroz.practicecheckpointpermanence");

		// pos menu (create it anyway)
		m_fields->pcpposMenu = PosInputBundle::create();
		m_fields->pcpposMenu->setPositionX(m_fields->size.width * 1.5);
		m_fields->pcpposMenu->setPositionY(m_fields->size.height / 4 - 130.f);
		m_fields->pcpposMenu->setScale(1);
		m_fields->pcpposMenu->setID("pcp-pos-menu");
		if (m_fields->pcpmod) {
			m_fields->pcpposMenu->setValue(
				ccp(m_fields->pcpmod->getSavedValue<double>("switcherMenuPositionX"),
				m_fields->pcpmod->getSavedValue<double>("switcherMenuPositionY")));
		}
		this->m_mainLayer->addChild(m_fields->pcpposMenu);

		// buttons below
        std::map<int, std::pair<const char*, SEL_MenuHandler>> btnIndexes = {
			{-80, {"optionsBtn.png"_spr, menu_selector(PracticeOptionsLayer::onOptions)}},
            {-40, {"fullscreenBtn.png"_spr, menu_selector(PracticeOptionsLayer::onPreview)}},
            {0, {"snapBtn.png"_spr, menu_selector(PracticeOptionsLayer::onSnap)}},
            {40, {"resetBtn.png"_spr, menu_selector(PracticeOptionsLayer::onResetNew)}},
            {80, {"applyBtn.png"_spr, menu_selector(PracticeOptionsLayer::onClose)}}
        };

		auto color = Mod::get()->getSettingValue<ccColor3B>("ui-color");

        for (auto [k, v] : btnIndexes) {
            auto spr = CCSprite::create(v.first);
            spr->setScale(0.6);
            auto btn = CCMenuItemSpriteExtra::create(spr, this, v.second);
			btn->setPositionX(k);
            this->m_buttonMenu->addChild(btn);

			if (v.first == "snapBtn.png"_spr) {
				this->m_fields->snapBtn = btn;
				btn->setTag(10);
				btn->setColor(Mod::get()->getSavedValue<bool>("snap") ? ccc3(128, 255, 128) : ccc3(255, 128, 128));
			} else
			btn->setColor(color);				
        }

		this->m_buttonMenu->setPositionY(-5.f);
		this->m_buttonMenu->setContentSize(ccp(0.f, 0.f));
		this->m_buttonMenu->setScale(0.5);
        this->m_buttonMenu->setID("button-menu");

		this->Transition(true, true);
		m_fields->map->helpTransition(true);

		log::debug("naxrin finish hooking this");

		#ifdef GEODE_IS_WINDOWS
		if (Mod::get()->getSettingValue<bool>("dont-crash"))
			return true;
		#endif

		listenForSettingChangesV3("bg-color", [this] (ccColor3B val) { this->runAction(CCTintTo::create(0.2, val.r, val.g, val.b)); });
		listenForSettingChangesV3("bg-opacity", [this] (int val) { this->runAction(CCFadeTo::create(0.2, val * 255 / 100.f)); });
		listenForSettingChangesV3("ui-color", [this] (ccColor3B val) {
			for (auto child : CCArrayExt<CCMenuItemSpriteExtra*>(this->m_buttonMenu->getChildren()))
				if (child->getTag() != 10)
					child->setColor(val);

			//static_cast<CCLabelBMFont*>(this->m_mainLayer->getChildByID("switcher-scale")
				//->getChildByID("kevadroz.practicecheckpointpermanence/switcher_scale_label"))->setColor(val);
		});

		return true;
	}


	ListenerResult handleSignal(Signal* event) {
		// escape from fullscreen preview
		if (event->tag == -100) {
			if (!event->value && this->m_fields->map->getScale() == 1) {
				this->Transition(true, false);
				m_fields->opl->setVisible(true);				
			}
		}
		// x pos
		else if (event->tag == 114) {
			if (m_fields->pcp) {
				if (m_fields->pcpmod) {
					m_fields->pcpmod->setSavedValue("switcherMenuPositionX", event->value);
					m_fields->map->getChildByID("kevadroz.practicecheckpointpermanence/switcher_menu")->setPositionX(event->value);					
				}
			} else {
				//log::debug("pos x is set to {}", event->value);
				gm->m_practicePos.x = event->value;
				this->m_fields->map->placeNode(ccp(event->value, this->m_practiceNode->getPositionY()));
				if (auto pl = PlayLayer::get())
					pl->m_uiLayer->getChildByID("checkpoint-menu")->setPositionX(event->value);				
			}
		}
		// y pos
		else if (event->tag == 514) {
			if (m_fields->pcp) {
				if (m_fields->pcpmod) {
					m_fields->pcpmod->setSavedValue("switcherMenuPositionY", event->value);
					m_fields->map->getChildByID("kevadroz.practicecheckpointpermanence/switcher_menu")->setPositionY(event->value);
				}
			} else {
				//log::debug("pos y is set to {}", event->value);
				gm->m_practicePos.y = event->value;	
				this->m_fields->map->placeNode(ccp(this->m_practiceNode->getPositionX(), event->value));
				if (auto pl = PlayLayer::get())
					pl->m_uiLayer->getChildByID("checkpoint-menu")->setPositionY(event->value);
			}
		}		
		// opacity
		else if (event->tag == 1) {
            this->valueDidChange(-29, event->value);
			if (auto pl = PlayLayer::get()) {
				static_cast<CCSprite*>(pl->m_uiLayer->getChildByID("checkpoint-menu")->getChildByID("add-checkpoint-button")
					->getChildByID("add-checkpoint-hint"))->setPositionY(event->value);
				static_cast<CCSprite*>(pl->m_uiLayer->getChildByID("checkpoint-menu")->getChildByID("remove-checkpoint-button")
					->getChildByID("remove-checkpoint-hint"))->setPositionY(event->value);
			}

		}
		return ListenerResult::Stop;
	}

	void Transition(bool in, bool whole) {
		this->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.3, in * Mod::get()->getSettingValue<int64_t>("bg-opacity") * 255 / 100)));
		if (whole) {
			this->m_fields->map->stopAllActions();
			this->m_fields->map->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, 0.25 * (in + 1))));
		} else {
			this->m_fields->map->stopAllActions();
			this->m_fields->map->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, 1 - 0.5 * in)));
			this->m_fields->map->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.2, ccp(0.f, 30.f * in))));
		}


		this->m_fields->titleLabel->stopAllActions();
		this->m_fields->titleLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, 0.35 * (in + 1))));
		this->m_fields->titleLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3, ccp(m_fields->size.width / 2, m_fields->size.height * 3 / 4 + 100.f - in * 50.f))));
		
		switchPCP();
		this->m_fields->posMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, in)));
		this->m_fields->pcpposMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, in)));
		this->m_fields->opacityMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, in)));
		if (auto menu = this->m_mainLayer->getChildByID("switcher-scale"))
			menu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, in)));

		this->m_buttonMenu->stopAllActions();
		this->m_buttonMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.3, 0.5 * (in + 1))));
		this->m_buttonMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3, ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 100.f + in * 45.f))));
	}

	bool ccTouchBegan(CCTouch* touch, CCEvent* event) override { return true; }

	void ccTouchMoved(CCTouch* touch, CCEvent* event) override { }

	void ccTouchEnded(CCTouch* touch, CCEvent* event) override { }
	
	// this empty override makes pcp inputers wholy not clickable, really sucks
	/*
	void textChanged(CCTextInputNode* input) override {
		//UIPOptionsLayer::textChanged(input);
		log::debug("text changed");
	}

	void textInputOpened(CCTextInputNode* input) override {
		log::debug("text input opened");
	}

	void textInputClosed(CCTextInputNode* input) override {
		log::debug("text input closed");
	}*/

    void onOptions(CCObject*) {
		//m_fields->pcp = !m_fields->pcp;
		//this->switchPCP();
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
		// tint color
		this->m_fields->snapBtn->runAction(CCTintTo::create(0.2, 128 + 127 * on, 255 - 127 * on, 128));
	}

	void onResetNew(CCObject* obj) {
		auto defpos = ccp(m_fields->size.width / 2, 40.f);
		gm->m_practicePos = defpos;
		gm->m_practiceOpacity = 1;
		this->m_fields->map->placeNode(defpos);
		this->m_fields->map->alphaNode(255, 0.2);
		this->m_fields->posMenu->setValue(defpos);
		this->m_fields->opacityMenu->setValue(1);

		if (m_fields->pcpmod) {
			m_fields->map->getChildByID("kevadroz.practicecheckpointpermanence/switcher_menu")
				->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(50.f, m_fields->size.height - 50.f))));
			m_fields->map->getChildByID("kevadroz.practicecheckpointpermanence/switcher_menu")
				->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.75f)));

			m_fields->pcpposMenu->setValue(ccp(50.f, m_fields->size.height - 50.f));
			m_mainLayer->getChildByID("switcher-scale")->getChildByType<TextInput*>(0)->setString("0.75");
			m_mainLayer->getChildByID("switcher-scale")->getChildByType<Slider*>(0)->setValue(2.f / 11.f);
			m_fields->pcpmod->setSavedValue("switcherMenuPositionX", 50.f);
			m_fields->pcpmod->setSavedValue("switcherMenuPositionY", m_fields->size.height - 50.f);
			m_fields->pcpmod->setSavedValue("switcherMenuScale", 0.75f);
		}
	}

	void switchPCP() {
		this->m_fields->posMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3,
			ccp(m_fields->size.width * (0.5 - m_fields->pcp), m_fields->size.height / 4 + 10.f - m_fields->in_prev * 110.f))));
		this->m_fields->pcpposMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3,
			ccp(m_fields->size.width * (1.5 - m_fields->pcp), m_fields->size.height / 4 + 10.f - m_fields->in_prev * 110.f))));

		this->m_fields->opacityMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3,
			ccp(m_fields->size.width * (0.5 - m_fields->pcp), m_fields->size.height / 4 - 20.f - m_fields->in_prev * 80.f))));
		if (auto menu = this->m_mainLayer->getChildByID("switcher-scale"))
			menu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3,
			ccp(m_fields->size.width * (1.5 - m_fields->pcp), m_fields->size.height / 4 - 20.f - m_fields->in_prev * 80.f))));
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