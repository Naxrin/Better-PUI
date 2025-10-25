#include <Geode/Geode.hpp>

using namespace geode::prelude;

const auto gm = GameManager::sharedState();

// single value signal
// using unique index system
class Signal : public Event {
public:
    int tag;
    float value;
    Signal(int tag, float value) {
        this->tag = tag;
        this->value = value;
    }
};

// position tuple signal
// posted when a node is dragged
class PosSignal : public Event {
public:
    int tag;
    CCPoint pos;
    PosSignal(int tag, CCPoint pos) {
        this->tag = tag;
        this->pos = pos;
    }
};

// 

// a menu bundle to set position in two inputs
class PosInputBundle : public CCMenu, public TextInputDelegate {
protected:
    // labels
    CCLabelBMFont* m_labelX, * m_labelY;
    // the inputers
    TextInput* m_inputX, * m_inputY;
    // init
    bool init() override;
    // text input callback
    void textChanged(CCTextInputNode* input) override;
public:
    // direct set its value
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
    // range
    float min, max;
    // precision
    int accu;
    // if the value should never overflow
    bool force;
    CCLabelBMFont* m_label;
    TextInput* m_input;
    Slider* m_slider;
    // init
    bool init(const char* title, float min, float max, int accu);
    // inputer
    void textChanged(CCTextInputNode* input) override;
    // slider
    void onSlider(CCObject* sender);
public:
    // register devtools
    static void registerDevTools();
    // set the inner value of inputer and slider
    void setValue(float val);
    // create it
    // @param title text label
    // @param min mininum value
    // @param max maxinum value
    // @param accu accuracy, zero for int
    static InputSliderBundle* create(const char* title, float min, float max, int accu) {
        auto node = new InputSliderBundle();
        if (node && node->init(title, min, max, accu)) {
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
    void reGridX(bool visible);
    void reGridY(bool visible);
    // fade the grid line
    virtual void helpTransition(bool in);
    // set visibility of inner grid line
    void setGridVisibility(bool visible);
};

class PracticePreviewFrame : public PreviewFrame {
protected:
    // target node;
    CCSprite* m_target;
    // target sprites
    CCSprite* m_sprL, * m_sprR;

    // init
    bool init() override;
    // detect the node should be dragged or not, elsewise quit preview
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    // drag node and post position signal
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    // snap or not
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override;
public:
    // also fade the two sprites
    void helpTransition(bool in) override;
    // place the node somewhere with action
    void placeNode(const CCPoint& pos);
    // set the node's opacity with action
    void alphaNode(GLubyte opacity, float d = 0);
    // create it
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
    // generated as preview
    bool preview;
    // current ID (zero for single mode)
    int current;
    // activated
    bool activate;
    // target node;
    GJUINode* p1mNode, * p2mNode,* p1jNode,* p2jNode;
    // init the five gjuinodes
    bool init(bool preview);
    // for dual mode, detect the current node is dragged or not, then the other nodes, elsewise quit preview
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    // drag the node and post position signal
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    // snap or not
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override;
public:
    // register devtools
    static void registerDevTools();
    // get current modified tag (zero for single mode)
    int getCurrent();
    // switch between single mode and dual mode
    void switchDual();
    // get target node as GJUINode class
    GJUINode* getTargetNode(int tag);
    // fade the visible nodes
    void helpTransition(bool in) override;
    // update the state of a node
    void updateState(int tag, const UIButtonConfig &config, float d = 0);
    void updateState(GJUINode* node, const UIButtonConfig &config, float d = 0);
    // set position of target node with action
    void placeNode(int tag, const CCPoint& pos, float d = 0);
    void placeNode(GJUINode* node, const CCPoint& pos, float d = 0);
    // set opacity of target node with action
    void alphaNode(int tag, GLubyte opacity, float d = 0);
    void alphaNode(GJUINode* node, GLubyte opacity, float d = 0);
    // set scale of target node with action
    void scaleNode(int tag, float scale, float d = 0);
    void scaleNode(GJUINode* node, float scale, float d = 0);
    // set node radius
    void radiusNode(int tag, float r, float d = 0);
    void radiusNode(GJUINode* node, float r, float d = 0);
    // create it
    static PlatformPreviewFrame* create(bool preview) {
        auto node = new PlatformPreviewFrame();
        if (node && node->init(preview)) {
            node->autorelease();
            return node;
        };
        CC_SAFE_DELETE(node);
        return nullptr;
    }
};

class SlotFrame : public CCMenu {
protected:
    // dual
    bool dual;
    // in preview
    bool showing;
    // really has data or is empty
    bool real = true;
    // raw string of this config
    gd::string* raw;
    // screen size
    const CCSize size = CCDirector::sharedDirector()->getWinSize();
    // bg
    CCLayerColor* bg;
    // button config
    CCLabelBMFont* titleLabel;
    CCLabelBMFont* descLabel;
    CCMenuItemSpriteExtra* prevBtn;
    CCMenuItemSpriteExtra* saveBtn;
    CCMenuItemSpriteExtra* loadBtn;
    // init
    bool init(int nametag);
    // parse 
    void parseSingleBtn(UIButtonConfig &config, std::string raw);
    // dump
    std::string dumpSingleBtn(UIButtonConfig const &config);
    // parse from string
    void parse();
    // change a desc label
    // I do this cuz cocos logic in setString with \n is terrible
    void refreshDescLabel();
public:
    // config
    UIButtonConfig p1m, p2m, p1j, p2j;
    // jumpL
    bool jumpL;
    // register devtools
    static void registerDevTools();
    // set dual status
    void setDualStatus(bool dual);
    // preview this config
    void onPreview(CCObject*);
    // resume
    void resume();
    // dump current to slot
    void onSave(CCObject*);
    // apply to curreent
    void onApply(CCObject*);
    // create
    static SlotFrame* create(int nametag) {
        auto node = new SlotFrame();
        if (node && node->init(nametag)) {
            node->autorelease();
            return node;
        };
        CC_SAFE_DELETE(node);
        return nullptr;
    }
};