#include "head.hpp"
#include <regex>

// convert float to string with given precision
std::string convert(float val, int accu) {
    // accuracy
    std::stringstream ss;
    ss << std::fixed << std::setprecision(accu) << val;
    return ss.str();
}

// deepseek tells me to write this
std::string regulate(const std::string& input) {
    if (input == "" || input == "-")
        return "0";
    // regex match numbers and no more than one dot
    std::regex pattern(R"(^-?\d*\.?\d*)");
    std::smatch matches;
    
    if (std::regex_match(input, matches, pattern))
        return matches[0].str();
    return "0";
}

bool PosInputBundle::init() {
    if (!CCMenu::init())
        return false;

    this->setContentSize(ccp(0.f, 0.f));

    auto labelX = CCLabelBMFont::create("X = ", "chatFont.fnt");
    labelX->setPosition(ccp(-150.f, 0.f));
    this->addChild(labelX);

    this->m_inputX = TextInput::create(60, "X Pos");
    m_inputX->setFilter("-0123456789.");
    m_inputX->setPosition(ccp(-90.f, 0.f));
    m_inputX->getInputNode()->setTag(114);
    m_inputX->setDelegate(this);
    this->addChild(m_inputX);

    auto labelY = CCLabelBMFont::create("Y = ", "chatFont.fnt");
    labelY->setPosition(ccp(60.f, 0.f));
    this->addChild(labelY);

    this->m_inputY = TextInput::create(60, "Y Pos");
    m_inputY->setFilter("-0123456789.");
    m_inputY->setPosition(ccp(120.f, 0.f));
    m_inputY->getInputNode()->setTag(514);
    m_inputY->setDelegate(this);
    this->addChild(m_inputY);

    return true;
}

void PosInputBundle::textChanged(CCTextInputNode* input) {
    std::string str = input->getString();
    //log::debug("pos input changed, tag = {}, value = {}", input->getTag(), round(100 * stof(regulate(str))) / 100);
    Signal(input->getTag(), round(100 * stof(regulate(str))) / 100).post();
}

void PosInputBundle::setValue(const CCPoint &pos) {
    this->m_inputX->setString(convert(pos.x, 2));
    this->m_inputY->setString(convert(pos.y, 2));
}

bool InputSliderBundle::init(const char* title, float min, float max, int accu) {
    if (!CCMenu::init())
        return false;

    this->min = min;    
    this->max = max;
    this->accu = accu;
    // only opacity value does not accept overflow value
    this->force = std::string(title) == "Opacity";
    this->setContentSize(ccp(0.f, 0.f));

    auto label = CCLabelBMFont::create(fmt::format("{} = ", title).c_str(), "chatFont.fnt");
    label->setPosition(ccp(-100.f, 0.f));
    this->addChild(label);

    this->m_input = TextInput::create(60, "0 ~ 1");
    m_input->setFilter("0123456789.");
    m_input->setPosition(ccp(-30.f, 0.f));
    m_input->setDelegate(this);
    this->addChild(m_input);

    this->m_slider = Slider::create(this, menu_selector(InputSliderBundle::onSlider));
    m_slider->setPosition(ccp(80.f, 0.f));
    m_slider->setContentSize(ccp(0.f, 0.f));
    m_slider->setScale(0.6f);
    this->addChild(m_slider);

    return true;
}

void InputSliderBundle::textChanged(CCTextInputNode* input) {
    std::string str = input->getString();
    float val = stof(convert(stof(regulate(str)), this->accu));
    val = (val > this->max) && this->force ? this->max : (val < this->min ? this->min : val);
    this->m_slider->setValue((val - this->min) / (this->max - this->min));
    Signal(this->getTag(), val).post();
}

void InputSliderBundle::onSlider(CCObject* sender) {
    float val = this->min + (this->max - this->min) * this->m_slider->getValue();
    std::string str = convert(val, accu);
    this->m_input->setString(str.c_str());
    Signal(this->getTag(), stof(str)).post();
}

void InputSliderBundle::setValue(float val) {
    std::string str = convert(val, accu);
    this->m_input->setString(str.c_str());
    this->m_slider->setValue((stof(str) - this->min) / (this->max - this->min));
}

bool PreviewFrame::init() {
    if (!CCLayer::init())
        return false;

    auto size = CCDirector::sharedDirector()->getWinSize();
    this->setContentSize(size);
    // center
    auto center = size / 2; 
    this->setAnchorPoint(ccp(0.5, 0.5));

    // border
    this->m_border = CCSprite::create();
    m_border->setPosition(center);
    //m_border->setC(center);
    m_border->setID("border");
    //m_border->setOpacity(0);
    //m_border->setCascadeOpacityEnabled(true);
    this->addChild(m_border);

    // grid
    this->m_grid = CCSprite::create();
    m_grid->setPosition(center);
    m_grid->setID("grid");
    //m_grid->setOpacity(0);
    //m_grid->setCascadeOpacityEnabled(true);
    this->addChild(m_grid);

    float tripleX[3] = {-center.width, 0, center.width};
    for (float x : tripleX) {
        auto v = CCLayerColor::create(ccc4(255, 255, 255, 0));
        v->setPosition(ccp(x, 0));
        v->setContentSize(ccp(1.2, size.height));
        v->ignoreAnchorPointForPosition(false);
        //v->setCascadeOpacityEnabled(true);
        if (x)
            m_border->addChild(v);
        else {
            m_grid->addChild(v);
            this->vert.push_back(v);            
        }
    }

    float tripleY[3] = {-center.height, 0, center.height};
    for (float y : tripleY) {
        auto h = CCLayerColor::create(ccc4(255, 255, 255, 0));
        h->setPosition(ccp(0, y));
        h->setContentSize(ccp(size.width, 1.2));
        h->ignoreAnchorPointForPosition(false);
        //h->setCascadeOpacityEnabled(true);
        if (y)
            m_border->addChild(h);
        else {
            m_grid->addChild(h);
            this->hori.push_back(h);
        }
    }

    float x = 0;
    while (center.width - x > 30.f) {
        x += 30.f;
        auto l = CCLayerColor::create(ccc4(255, 255, 255, 0));
        l->setPosition(ccp(-x, 0));
        l->setContentSize(ccp(0.6, size.height));
        l->ignoreAnchorPointForPosition(false);
        m_grid->addChild(l);
        this->vert.push_back(l);

        auto r = CCLayerColor::create(ccc4(255, 255, 255, 0));
        r->setPosition(ccp(+x, 0));
        r->setContentSize(ccp(0.6, size.height));
        r->ignoreAnchorPointForPosition(false);
        m_grid->addChild(r);
        this->vert.push_back(r);
    }

    float y = 0;
    while (center.height - y > 30.f) {
        y += 30.f;

        auto t = CCLayerColor::create(ccc4(255, 255, 255, 0));
        t->setPosition(ccp(0, -y));
        t->setContentSize(ccp(size.width, 0.6));
        t->ignoreAnchorPointForPosition(false);
        m_grid->addChild(t);
        this->hori.push_back(t);

        auto b = CCLayerColor::create(ccc4(255, 255, 255, 0));
        b->setPosition(ccp(0, +y));
        b->setContentSize(ccp(size.width, 0.6));
        b->ignoreAnchorPointForPosition(false);
        m_grid->addChild(b);
        this->hori.push_back(b);
    }

    this->setTouchEnabled(true);
    this->setTouchMode(ccTouchesMode::kCCTouchesOneByOne);
    //this->registerWithTouchDispatcher();

    return true;
}

void PreviewFrame::helpTransition(bool in) {
    for (auto b : CCArrayExt<CCLayerColor*>(m_border->getChildren()))
        b->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, 255 * in)));

    if (Mod::get()->getSavedValue<bool>("snap"))
        this->setGridVisibility(in);
}

void PreviewFrame::setGridVisibility(bool visible) {
    for (auto v : vert)
        v->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, visible * (240 - 96 * bool(v->getPositionX())))));   
    for (auto h : hori)
        h->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, visible * (240 - 96 * bool(h->getPositionY())))));   
}

bool PracticePreviewFrame::init() {
    if (!PreviewFrame::init())
        return false;

    // target node
    // the practice button (sprite here)
    this->m_sprL = CCSprite::createWithSpriteFrameName("GJ_checkpointBtn_001.png");
    m_sprL->setPositionX(-28.25f);
    m_sprL->setOpacity(gm->m_practiceOpacity);

    this->m_sprR = CCSprite::createWithSpriteFrameName("GJ_removeCheckBtn_001.png");
    m_sprR->setPositionX(41.50f);
    m_sprR->setOpacity(gm->m_practiceOpacity);

    this->m_target = CCSprite::create();
    m_target->setPosition(gm->m_practicePos);
    m_target->setContentSize(ccp(0.f, 0.f));
    m_target->setID("target");
    m_target->addChild(this->m_sprL);
    m_target->addChild(this->m_sprR);
    this->addChild(m_target);

    return true;
}

bool PracticePreviewFrame::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    auto p = this->m_target->getPosition();
    CCRect rect = {p.x - 70.f, p.y - 28.f, 140.f, 56.f};
    bool ret = rect.containsPoint(this->convertTouchToNodeSpace(touch));
    Signal(-100, ret).post();
    return ret;
}

void PracticePreviewFrame::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    //auto delta = touch->m_point - touch->m_startPoint;
    auto delta = touch->getDelta();
    //log::debug("x = {}, y = {}", delta.x, delta.y);
    auto dest = this->m_target->getPosition() + delta / this->getScale();
    this->m_target->setPosition(dest);
    //this->m_target->setPosition(this->convertTouchToNodeSpace(touch));
    PosSignal(0, dest).post();
}

void PracticePreviewFrame::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    if (!Mod::get()->getSavedValue<bool>("snap"))
        return;
    auto t = this->convertTouchToNodeSpace(touch);
    auto c = this->getContentSize() / 2;
    auto dest = ccp(c.width + 30 * round((t.x - c.width) / 30), c.height + 30 * round((t.y - c.height) / 30));
    this->m_target->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, dest)));
    PosSignal(0, dest).post();
}

void PracticePreviewFrame::helpTransition(bool in) {
    this->alphaNode(in * 255 * gm->m_practiceOpacity, 0.3);
    PreviewFrame::helpTransition(in);
}

void PracticePreviewFrame::placeNode(const CCPoint& pos) {
    this->m_target->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, pos)));
}

void PracticePreviewFrame::alphaNode(GLubyte opacity, float d) {
    this->m_sprL->runAction(CCEaseExponentialOut::create(CCFadeTo::create(d, opacity)));
    this->m_sprR->runAction(CCEaseExponentialOut::create(CCFadeTo::create(d, opacity)));
}

bool PlatformPreviewFrame::init(bool preview) {
    if (!PreviewFrame::init())
        return false;

    this->current = Mod::get()->getSavedValue<bool>("dual");
    this->preview= preview;
    // target nodes
    this->p1mNode = GJUINode::create(current ? gm->m_dpad2 : gm->m_dpad1);
    p1mNode->setTag(current);
    this->alphaNode(p1mNode, 0);
    this->addChild(p1mNode);

    this->p2mNode = GJUINode::create(gm->m_dpad3);
    p2mNode->setTag(2);
    this->alphaNode(p2mNode, 0);
    this->addChild(p2mNode);

    this->p1jNode = GJUINode::create(gm->m_dpad4);
    p1jNode->setTag(3);
    this->alphaNode(p1jNode, 0);
    this->addChild(p1jNode);

    this->p2jNode = GJUINode::create(gm->m_dpad5);
    p2jNode->setTag(4);
    this->alphaNode(p2jNode, 0);
    this->addChild(p2jNode);

    return true;
}

bool PlatformPreviewFrame::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (!this->activate)
        return false;
    else if (this->preview) {
        auto p = this->convertTouchToNodeSpace(touch);
        auto s = this->getContentSize() / 2;
        if (p.x < s.width - 110 || p.x > s.width + 110 || p.y > s.height - 232)
            Signal(-100, -514).post();
        return false;
    }

    // not preview
    auto t = this->convertTouchToNodeSpace(touch);
    // single mode
    if (!this->current) {
        auto r = p1mNode->m_rect;
        //log::debug("rect = {} {}", r.origin.x, r.origin.y);
        r.origin = p1mNode->getPosition() - r.size / 2;
        bool ret = r.containsPoint(t);
        Signal(-100, ret ? 0 : -114).post();
        return ret;
    }
    // dual mode
    auto r = this->getTargetNode(current)->m_rect;
    r.origin = this->getTargetNode(current)->getPosition() - r.size / 2;
    if (r.containsPoint(t)) {
        Signal(-100, current).post();
        return true;
    } else for (auto tag = 1; tag < 5; tag ++) {
        if (tag == this->current)
            continue;

        auto r = this->getTargetNode(tag)->m_rect;
        r.origin = this->getTargetNode(tag)->getPosition() - r.size / 2;
        if (r.containsPoint(t)) {
            this->current = tag;
            Signal(-100, tag).post();
            return true;
        }
    }
    Signal(-100, -114).post();
    return false;
}

void PlatformPreviewFrame::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    auto delta = touch->getDelta();
    auto dest = this->getChildByTag(current)->getPosition() + delta / this->getScale();
    this->getChildByTag(current)->setPosition(dest);
    PosSignal(false, dest).post();
}

void PlatformPreviewFrame::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    auto delta = touch->getDelta();
    auto dest = this->getChildByTag(current)->getPosition() + delta / this->getScale();
    if (!Mod::get()->getSavedValue<bool>("snap")) {
        this->getChildByTag(current)->setPosition(dest);
        PosSignal(true, dest).post();
        return;
    }
    auto c = this->getContentSize() / 2;
    dest = ccp(c.width + 30 * round((dest.x - c.width) / 30), c.height + 30 * round((dest.y - c.height) / 30));
    this->getChildByTag(current)->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, dest)));
    PosSignal(true, dest).post();
}

int PlatformPreviewFrame::getCurrent() {
    return this->current;
}

void PlatformPreviewFrame::switchDual() {
    this->current = !this->current;
    this->p1mNode->setTag(this->current);

    this->updateState(this->current, this->current ? gm->m_dpad2 : gm->m_dpad1, 0.2 * (!this->preview));

    this->alphaNode(this->p2mNode, this->current * gm->m_dpad3.m_opacity, 0.2 * (!this->preview));
    this->alphaNode(this->p1jNode, this->current * gm->m_dpad4.m_opacity, 0.2 * (!this->preview));
    this->alphaNode(this->p2jNode, this->current * gm->m_dpad5.m_opacity, 0.2 * (!this->preview));

    this->p2mNode->m_drawLines = current;
    this->p1jNode->m_drawLines = current;
    this->p2jNode->m_drawLines = current;
}

GJUINode* PlatformPreviewFrame::getTargetNode(int tag) {
    return static_cast<GJUINode*>(this->getChildByTag(tag));
}

void PlatformPreviewFrame::helpTransition(bool in) {
    this->activate = in;
    if (this->current) {
        this->alphaNode(1, in * gm->m_dpad2.m_opacity, 0.3);
        this->alphaNode(2, in * gm->m_dpad3.m_opacity, 0.3);
        this->alphaNode(3, in * gm->m_dpad4.m_opacity, 0.3);
        this->alphaNode(4, in * gm->m_dpad5.m_opacity, 0.3);
    }
    else
        this->alphaNode(0, in * gm->m_dpad1.m_opacity, 0.3);

    this->p1mNode->m_drawLines = in;
    this->p2mNode->m_drawLines = in && this->current;
    this->p1jNode->m_drawLines = in && this->current;
    this->p2jNode->m_drawLines = in && this->current;

    PreviewFrame::helpTransition(in);
}

// update state
void PlatformPreviewFrame::updateState(int tag, const UIButtonConfig &config, float d) {
    //log::debug("upstate tag = {}", tag);
    this->updateState(static_cast<GJUINode*>(this->getChildByTag(tag)), config, d);
}

void PlatformPreviewFrame::updateState(GJUINode* node, const UIButtonConfig &config, float d) {

    node->m_deadzone = config.m_deadzone;
    node->m_modeB = config.m_modeB;
    node->m_snap = config.m_snap;
    node->m_split = config.m_split;
    node->m_rect.size = ccp(config.m_width, config.m_height);
    
    node->m_firstSprite->setVisible(!config.m_modeB);
    if (!node->m_oneButton)
        node->m_secondSprite->setVisible(!config.m_modeB);

    this->placeNode(node, config.m_position, d);
    this->scaleNode(node, config.m_scale, d);
    this->alphaNode(node, config.m_opacity, d);
    this->radiusNode(node, config.m_radius, d);

    node->updateButtonFrames();

}

// set position of target node
void PlatformPreviewFrame::placeNode(int tag, const CCPoint& pos, float d) {
    this->placeNode(static_cast<GJUINode*>(this->getChildByTag(tag)), pos, d);
}

void PlatformPreviewFrame::placeNode(GJUINode* node, const CCPoint& pos, float d) {
    node->runAction(CCEaseExponentialOut::create(CCMoveTo::create(d, pos)));
}


void PlatformPreviewFrame::alphaNode(int tag, GLubyte opacity, float d) {
    //log::debug("alpha node, tag = {}", tag);
    this->alphaNode(static_cast<GJUINode*>(this->getChildByTag(tag)), opacity, d);
}

void PlatformPreviewFrame::alphaNode(GJUINode* node, GLubyte opacity, float d) {
    node->m_firstSprite->runAction(CCEaseExponentialOut::create(CCFadeTo::create(d, opacity)));
    if (!node->m_oneButton)
        node->m_secondSprite->runAction(CCEaseExponentialOut::create(CCFadeTo::create(d, opacity)));
}

void PlatformPreviewFrame::scaleNode(int tag, float scale, float d) {
    this->scaleNode(static_cast<GJUINode*>(this->getChildByTag(tag)), scale, d);
}

void PlatformPreviewFrame::scaleNode(GJUINode* node, float scale, float d) {
    node->m_firstSprite->runAction(CCEaseExponentialOut::create(CCScaleTo::create(d, scale)));
    if (!node->m_oneButton)
        node->m_secondSprite->runAction(CCEaseExponentialOut::create(CCScaleTo::create(d, scale)));
}

void PlatformPreviewFrame::radiusNode(int tag, float r, float d) {
    this->radiusNode(static_cast<GJUINode*>(this->getChildByTag(tag)), r, d);
}

void PlatformPreviewFrame::radiusNode(GJUINode* node, float r, float d) {
    if (node->m_oneButton)
        return;
    node->m_radius = r;
    if (node->m_modeB) {
        node->updateButtonFrames();
        return;
    }
    float base = node->m_firstSprite->getContentWidth() / 2;
    node->m_firstSprite->runAction(CCEaseExponentialOut::create(CCMoveTo::create(d, ccp(-base - r, 0))));
    node->m_secondSprite->runAction(CCEaseExponentialOut::create(CCMoveTo::create(d, ccp(base + r, 0))));
}

bool SlotFrame::init(int nametag) {
    if (!CCMenu::init())
        return false;

    this->setContentSize(ccp(360.f, 70.f));
    this->setAnchorPoint(ccp(0.5, 0.5));
    this->ignoreAnchorPointForPosition(false);
    this->setScale(0);
    this->setTag(nametag);

    this->bg = CCLayerColor::create(ccc4(255, 255, 255, 50));
    this->bg->setContentSize(ccp(360.f, 70.f));
    this->bg->setAnchorPoint(ccp(0.5, 0.5));
    this->bg->setID("background");
    this->addChildAtPosition(this->bg, Anchor::BottomLeft);

    this->titleLabel = CCLabelBMFont::create("title", "bigFont.fnt");
    this->titleLabel->setPosition(ccp(10.f, 55.f));
    this->titleLabel->setAnchorPoint(ccp(0.f, 0.5));
    this->titleLabel->setScale(0.5);
    this->titleLabel->setID("title");
    this->addChild(this->titleLabel);

    this->descLabel = CCLabelBMFont::create("desc", "chatFont.fnt", 1000.f);
    this->descLabel->setPosition(ccp(10.f, 40.f));
    this->descLabel->setAnchorPoint(ccp(0.f, 1));
    this->descLabel->setScale(0.55);
    this->descLabel->setID("desc");
    this->addChild(this->descLabel);

    auto prevSpr = CCSprite::create("fullscreenBtn.png"_spr);
    prevSpr->setScale(0.4);
    this->prevBtn = CCMenuItemSpriteExtra::create(prevSpr, this, menu_selector(SlotFrame::onPreview));
    this->prevBtn->setPosition(ccp(290.f, 10.f));
    this->addChild(this->prevBtn);

    auto saveSpr = CCSprite::create("slotBtn.png"_spr);
    saveSpr->setScale(0.4);
    this->saveBtn = CCMenuItemSpriteExtra::create(saveSpr, this, menu_selector(SlotFrame::onSave));
    this->saveBtn->setPosition(ccp(315.f, 10.f));
    this->addChild(this->saveBtn);

    auto loadSpr = CCSprite::create("applyBtn.png"_spr);
    loadSpr->setScale(0.4);
    this->loadBtn = CCMenuItemSpriteExtra::create(loadSpr, this, menu_selector(SlotFrame::onApply));
    this->loadBtn->setPosition(ccp(340.f, 10.f));
    this->addChild(this->loadBtn);

    return true;
}

void SlotFrame::parseSingleBtn(UIButtonConfig &config, std::string raw) {
    if (raw == "") {
        this->real = false;
        return;
    }
    // regex
    std::regex pt(config.m_oneButton ? R"w(^(\d+),(\d+),(\d+.?\d{0,2})\d*,(\d+.?\d{0,2})\d*,(\d+.?\d{0,2})\d*,(\d+.?\d{0,2})\d*$)w"
        : R"w(^(\d+),(\d+),(\d+.?\d{0,2})\d*,(\d+.?\d{0,2})\d*,(\d+.?\d{0,2})\d*,(\d+.?\d{0,2})\d*,([01]),(\d+.?\d{0,1})\d*,(\d+.?\d{0,1})\d*,([01]),([01])$)w");
    std::smatch match;

    this->real = std::regex_match(raw, match, pt);
    //log::debug("parse single raw str = {} ret = {}", raw, this->real);
    if (!this->real)
        return;
    // data fill to be fixed
    config.m_width = stoi(match.str(1));
    config.m_height = stoi(match.str(2));
    config.m_scale = stof(match.str(3));
    config.m_opacity = 255 * (stof(match.str(4)) > 1 ? 1 : stof(match.str(4)));
    config.m_position = ccp(stof(match.str(5)), stof(match.str(6)));
    if (!config.m_oneButton) {
        config.m_modeB = stoi(match.str(7));
        config.m_deadzone = stof(match.str(8));
        config.m_radius = stof(match.str(9));
        config.m_snap = stoi(match.str(10));
        config.m_split = stoi(match.str(11));
    }
}

std::string SlotFrame::dumpSingleBtn(UIButtonConfig const &config) {
    return fmt::format("{},{},{},{},{},{}",
        config.m_width, config.m_height, config.m_scale, config.m_opacity / 255, config.m_position.x, config.m_position.y)
        + (config.m_oneButton ? "" : fmt::format(",{},{},{},{},{}",
        int(config.m_modeB), config.m_deadzone, config.m_radius, int(config.m_snap), int(config.m_snap)));
}

void SlotFrame::parse() {
    if (*this->raw == "") {
        this->real = false;
        return;
    }
    std::regex pt(this->dual ? R"w(([-\d.,]+);([-\d.,]+);([-\d.,]+);([-\d.,]+))w" : R"w(([-\d.,]+);([01]))w");
    std::smatch match;
    // match
    std::string str = fmt::format("{}", *this->raw);
    this->real = std::regex_match(str, match, pt);
    //log::debug("parse full raw str = {} ret = {}", *raw, this->real);
    if (!this->real)
        return;
    // parse
    this->parseSingleBtn(this->p1m, match.str(1));
    if (this->dual) {
        this->parseSingleBtn(this->p1m, match.str(1));
        this->parseSingleBtn(this->p2m, match.str(2));
        this->parseSingleBtn(this->p1j, match.str(3));
    } else
        this->jumpL = stoi(match.str(2));
}

void SlotFrame::refreshDescLabel() {
    auto pos = this->descLabel->getPosition();
    auto scale = this->descLabel->getScale();
    this->descLabel->removeFromParentAndCleanup(true);

    auto str = this->dual ? fmt::format(
        "P1M pos={}, {} w={} h={} s={} opacity={} modeb={} dz={} radius={} snap={} split={}\n"
        "P2M pos={}, {} w={} h={} s={} opacity={} modeb={} dz={} radius={} snap={} split={}\n"
        "P1J pos={}, {} w={} h={} s={} opacity={}\n"
        "P1J pos={}, {} w={} h={} s={} opacity={}",
        convert(p1m.m_position.x, 2), convert(p1m.m_position.y, 2), p1m.m_width, p1m.m_height, p1m.m_scale, p1m.m_opacity,
        p1m.m_modeB, p1m.m_deadzone, p1m.m_radius, p1m.m_snap, p1m.m_split,
        convert(p2m.m_position.x, 2), convert(p2m.m_position.y, 2), p2m.m_width, p2m.m_height, p2m.m_scale, p2m.m_opacity,
        p2m.m_modeB, p2m.m_deadzone, p2m.m_radius, p2m.m_snap, p2m.m_split,
        convert(p1j.m_position.x, 2), convert(p1j.m_position.y, 2), p1j.m_width, p1j.m_height, p1j.m_scale, p1j.m_opacity,
        convert(p2j.m_position.x, 2), convert(p2j.m_position.y, 2), p2j.m_width, p2j.m_height, p2j.m_scale, p2j.m_opacity
    ) : fmt::format("pos={}, {} w={} h={} s={} opacity={} modeb={} dz={} radius={} snap={} split={}\njumpL={}",
        convert(p1m.m_position.x, 2), convert(p1m.m_position.y, 2), p1m.m_width, p1m.m_height, p1m.m_scale, p1m.m_opacity,
        p1m.m_modeB, p1m.m_deadzone, p1m.m_radius, p1m.m_snap, p1m.m_split, this->jumpL
    );
    this->descLabel = CCLabelBMFont::create(str.c_str(), "chatFont.fnt");
    this->descLabel->setPosition(pos);
    this->descLabel->setAnchorPoint(ccp(0.f, 1));
    this->descLabel->setScale(scale);
    this->descLabel->setID("desc");
    this->addChild(this->descLabel);
}

void SlotFrame::setDualStatus(bool dual) {
    this->dual = dual;

    this->p1m = UIButtonConfig{
        .m_width = 280, .m_height = 120, .m_deadzone = 0.f, .m_scale = 1.f, .m_opacity = 255, .m_radius = 10.f, .m_modeB = false,
        .m_snap = false, .m_position = ccp(95.f, 36.f), .m_oneButton = false, .m_player2 = false, .m_split = false
    };
    if (this->dual) {
        this->p2m = UIButtonConfig{
            .m_width = 280, .m_height = 120, .m_deadzone = 0.f, .m_scale = 1.f, .m_opacity = 255, .m_radius = 10.f, .m_modeB = false,
            .m_snap = false, .m_position = ccp(size.width - 95.f, 36.f), .m_oneButton = false, .m_player2 = true, .m_split = false
        };
        this->p1j = UIButtonConfig{
            .m_width = 200, .m_height = 200, .m_scale = 1.f, .m_opacity = 255, .m_position = ccp(95.f, 196.f), .m_oneButton = true, .m_player2 = false
        };
        this->p2j = UIButtonConfig{
            .m_width = 200, .m_height = 200, .m_scale = 1.f, .m_opacity = 255, .m_position = ccp(size.width - 95.f, 196.f), .m_oneButton = true, .m_player2 = true
        };
    } else
        this->jumpL = false;

    // refresh raw string pointer
    gd::string* s[3] = {
        this->dual ? &gm->m_dpadLayoutDual1 : &gm->m_dpadLayout1,
        this->dual ? &gm->m_dpadLayoutDual2 : &gm->m_dpadLayout2,
        this->dual ? &gm->m_dpadLayoutDual3 : &gm->m_dpadLayout3
    };
    this->raw = s[this->getTag() - 1];
    this->parse();

    this->prevBtn->setEnabled(real);
    this->loadBtn->setEnabled(real);

    this->titleLabel->setString(fmt::format("Dpad Layout {} #{}", this->dual ? "Dual" : "", this->getTag()).c_str());

    this->descLabel->setString("");
    if (!this->real) {
        this->descLabel->setString("<NA>");
        return;
    }
    this->refreshDescLabel();
}

void SlotFrame::onPreview(CCObject*) {
    if (this->showing) {
        Signal(1919, false).post();
        return;
    }
    this->showing = true;
    // post signal firstly
    Signal(1919, this->getTag()).post();

    // background
    this->bg->runAction(CCEaseExponentialOut::create(CCMoveTo::create(
        0.2, ccp(0.f, -this->getPositionY()))));
    this->bg->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, size.width / 360.f, size.height / 70.f)));
    this->bg->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, 0)));
    
    // title label
    this->titleLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(
        0.2, ccp(180.f - 0.3 * this->titleLabel->getContentWidth(), 170.f - this->getPositionY()))));
    this->titleLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.6)));

    // description text
    this->descLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(
        0.2, ccp(180.f - 0.35 * this->descLabel->getContentWidth(), -20.f - this->getPositionY()))));
    this->descLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.7)));

    this->prevBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(140.f, -100.f - this->getPositionY()))));
    this->prevBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.6)));

    this->saveBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(180.f, -100.f - this->getPositionY()))));
    this->saveBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.6)));

    this->loadBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(220.f, -100.f - this->getPositionY()))));
    this->loadBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.6)));
}

void SlotFrame::resume() {
    if (!this->showing)
        return;
    this->showing = false;

    Signal(-100, 0).post();
    //log::debug("resume");
    // title label
    this->titleLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(10.f, 55.f))));
    this->titleLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.5)));

    this->bg->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(0.f, 0.f))));
    this->bg->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 1)));
    this->bg->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, 50)));

    // description text
    this->descLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(10.f, 40.f))));
    this->descLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.5)));

    this->prevBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(290.f, 10.f))));
    this->prevBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.4)));

    this->saveBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(315.f, 10.f))));
    this->saveBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.4)));

    this->loadBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(340.f, 10.f))));
    this->loadBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.4)));
}

void SlotFrame::onSave(CCObject*) {
    this->real = true;
    this->prevBtn->setEnabled(true);
    this->loadBtn->setEnabled(true);

    if (this->dual) {
        this->p1m = gm->m_dpad2;
        this->p2m = gm->m_dpad3;
        this->p1j = gm->m_dpad4;
        this->p2j = gm->m_dpad5;
        *this->raw = fmt::format("{};{};{};{}",
            dumpSingleBtn(this->p1m), dumpSingleBtn(this->p2m),
            dumpSingleBtn(this->p1j), dumpSingleBtn(this->p2j)
        );
    } else {
        this->p1m = gm->m_dpad1;
        this->jumpL = gm->getGameVariable("0113");
        *this->raw = fmt::format("{};{}", dumpSingleBtn(this->p1m), int(this->jumpL));    
    }

    this->refreshDescLabel();
    if (this->showing)
        Signal(94, true).post();
    // notify
    FLAlertLayer::create("Saved!", fmt::format("Current config has been dumped to Slot {}.", this->getTag()), "Nice")->show();
}

void SlotFrame::onApply(CCObject*) {
    if (this->dual) {
        gm->m_dpad2 = this->p1m;
        gm->m_dpad3 = this->p2m;
        gm->m_dpad4 = this->p1j;
        gm->m_dpad5 = this->p2j;        
    } else {
        gm->m_dpad1 = this->p1m;
        gm->setGameVariable("0113", this->jumpL);
    }
    // tell main page to refresh
    Signal(810, this->getTag()).post();
}
