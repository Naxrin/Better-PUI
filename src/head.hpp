#include <Geode/Geode.hpp>

using namespace geode::prelude;

const auto gm = GameManager::sharedState();

/*
static PauseLayer* pl;
static GameOptionsLayer* opl;
*/

// event
// for positions we have unique index system
// for others robtop has this own
class Signal : public Event {
public:
    int tag;
    float value;
    Signal(int tag, float value) {
        this->tag = tag;
        this->value = value;
    }
};

class PosSignal : public Event {
public:
    int tag;
    CCPoint pos;
    PosSignal(int tag, CCPoint pos) {
        this->tag = tag;
        this->pos = pos;
    }
};

class SizeSignal : public Event {
public:
    int tag;
    CCSize size;
    SizeSignal(int tag, CCSize size) {
        this->tag = tag;
        this->size = size;
    }
};

// a new added bundle that allow to set position in two inputs
class PosInputBundle : public CCMenu, public TextInputDelegate {
protected:
    TextInput* m_inputX, * m_inputY;
    // init
    bool init() override;
    // text input feedback
    void textChanged(CCTextInputNode* input) override;
public:
    // set its value
    void setValue(const CCPoint &pos);
    // create it
    static PosInputBundle* create() {
        auto node = new PosInputBundle();
        if (node && node->init()) {
            node->autorelease();
            return node;
        };
        CC_SAFE_DELETE(node);
        return nullptr;
    }
};

// a bundle that contains a positive value
class InputSliderBundle : public CCMenu, public TextInputDelegate {
protected:
    float min, max;
    int accu;
    TextInput* m_input;
    Slider* m_slider;
    // init
    bool init(const char* title, int tag, float min, float max, int accu, bool force);
    // inputer
    void textChanged(CCTextInputNode* input) override;
    // slider
    void onSlider(CCObject* sender);
public:
    // set the inner value of inputer and slider
    void setValue(float val);
    // create it
    // @param title text label
    // @param tag tag of the target value
    // @param min mininum value
    // @param max maxinum value
    // @param accu accuracy, zero for int
    // @param force do not approve a text input value out of range
    static InputSliderBundle* create(const char* title, int tag, float min, float max, int accu, bool force) {
        auto node = new InputSliderBundle();
        if (node && node->init(title, tag, min, max, accu, force)) {
            node->autorelease();
            return node;
        };
        CC_SAFE_DELETE(node);
        return nullptr;
    }
};

class PreviewFrame : public CCLayer {
protected:
    // grid
    CCSprite* m_border, * m_grid;
    // vector to fade
    std::vector<CCLayerColor*> hori, vert;
    // init
    virtual bool init() override;
public:
    // change the density of grid
    // preserved for future update
    void reGrid(int d);
    // help fade in / out
    virtual void helpTransition(bool in);
    // set visibility of inner grid
    void setGridVisibility(bool visible) {
        for (auto v : vert)
            v->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.3, visible * (240 - 96 * bool(v->getPositionX())))));   
        for (auto h : hori)
            h->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.3, visible * (240 - 96 * bool(h->getPositionY())))));   
    }
};

class PracticePreviewFrame : public PreviewFrame {
protected:
    // target node;
    CCSprite* m_target;
    CCSprite* m_sprL, * m_sprR;

    bool init() override;
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override;
public:
    // set position of target node
    CCNode* getTargetNode() {
        return this->m_target;
    }

    void helpTransition(bool in) override {
        this->alphaNode(in * 255 * gm->m_practiceOpacity, 0.3);        
        PreviewFrame::helpTransition(in);
    }

    void placeNode(const CCPoint& pos) {
        this->m_target->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.2, pos)));
    }

    void alphaNode(GLubyte opacity, float set = 0) {
        if (set) {
            this->m_sprL->runAction(CCEaseExponentialOut::create(CCFadeTo::create(set, opacity)));
            this->m_sprR->runAction(CCEaseExponentialOut::create(CCFadeTo::create(set, opacity)));
        } else {
            this->m_sprL->setOpacity(opacity);
            this->m_sprR->setOpacity(opacity);            
        }
    }
    static PracticePreviewFrame* create() {
        auto node = new PracticePreviewFrame();
        if (node && node->init()) {
            node->autorelease();
            return node;
        };
        CC_SAFE_DELETE(node);
        return nullptr;
    }
};

class PlatformPreviewFrame : public PreviewFrame {
protected:
    // dual mode
    bool dual; 
    // current
    int current;
    // target node;
    GJUINode* sNode, * p1mNode, * p2mNode,* p1jNode,* p2jNode;
    //GJUINode* index[4] = {p1mNode, p2mNode, p1jNode, p2jNode};

    bool init() override;
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override;
public:
    // set position of target node
    GJUINode* getTargetNode(int tag) {
        return static_cast<GJUINode*>(this->getChildByTag(tag));
    }

    void helpTransition(bool in) override {
        this->alphaNode(0, in * 255 * gm->m_dpad1.m_opacity, 0.3);
        this->alphaNode(1, in * 255 * gm->m_dpad2.m_opacity, 0.3);
        this->alphaNode(2, in * 255 * gm->m_dpad3.m_opacity, 0.3);
        this->alphaNode(3, in * 255 * gm->m_dpad4.m_opacity, 0.3);
        this->alphaNode(4, in * 255 * gm->m_dpad5.m_opacity, 0.3);
        PreviewFrame::helpTransition(in);
    }

    void placeNode(int tag, const CCPoint& pos) {
        this->getChildByTag(tag)->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.2, pos)));
    }

    void alphaNode(int tag, GLubyte opacity, float set = 0) {
        auto target = static_cast<GJUINode*>(this->getChildByTag(tag));
        if (set) {
            target->m_firstSprite->runAction(CCEaseExponentialOut::create(CCFadeTo::create(set, opacity)));
            if (!target->m_oneButton)
                target->m_secondSprite->runAction(CCEaseExponentialOut::create(CCFadeTo::create(set, opacity)));
        } else {
            target->m_firstSprite->setOpacity(opacity);
            if (!target->m_oneButton)
                target->m_secondSprite->setOpacity(opacity);           
        }
    }
    static PlatformPreviewFrame* create() {
        auto node = new PlatformPreviewFrame();
        if (node && node->init()) {
            node->autorelease();
            return node;
        };
        CC_SAFE_DELETE(node);
        return nullptr;
    }
};