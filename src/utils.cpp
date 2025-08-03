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

bool InputSliderBundle::init(const char* title, int tag, float min, float max, int accu) {
    if (!CCMenu::init())
        return false;

    this->min = min;    
    this->max = max;
    this->accu = accu;
    this->setContentSize(ccp(0.f, 0.f));
    this->setTag(tag);

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
    val = val > this->max ? this->max : (val < this->min ? this->min : val);
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
        b->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.3, 255 * in)));

    if (Mod::get()->getSavedValue<bool>("snap"))
        this->setGridVisibility(in);
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
    this->m_target->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.2, dest)));
    PosSignal(0, dest).post();
}

bool PlatformPreviewFrame::init() {
    if (!PreviewFrame::init())
        return false;

    // target nodes
    this->sNode = GJUINode::create(gm->m_dpad1);
    sNode->setTag(0);
    this->addChild(sNode);

    this->p1mNode = GJUINode::create(gm->m_dpad2);
    p1mNode->setTag(1);
    this->addChild(p1mNode);

    this->p2mNode = GJUINode::create(gm->m_dpad3);
    p2mNode->setTag(2);
    this->addChild(p2mNode);

    this->p1jNode = GJUINode::create(gm->m_dpad4);
    p1jNode->setTag(3);
    this->addChild(p1jNode);

    this->p2jNode = GJUINode::create(gm->m_dpad5);
    p2jNode->setTag(4);
    this->addChild(p2jNode);

    return true;
}