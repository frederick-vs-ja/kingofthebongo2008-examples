//=================================================================================================
//
//	MJP's DX11 Sample Framework
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================

#include "PCH.h"
#include "Settings.h"
#include "Exceptions.h"
#include "Utility.h"
#include "App.h"
#include "TwHelper.h"

namespace SampleFramework11
{

// == Setting =====================================================================================

Setting::Setting() : tweakBar(NULL), type(SettingType::Invalid), data(NULL), changed(false)
{
}

void Setting::Initialize(TwBar* tweakBar_, SettingType type_, void* data_, const char* name_,
                         const char* group_, const char* label_, const char* helpText_,
                         ETwType twType_)
{
    tweakBar = tweakBar_;
    type = type_;
    data = data_;
    name = name_;
    group = group_;
    label = label_;
    helpText = helpText_;
    changed = false;

    static const ETwType twTypes[] =
    {
        TW_TYPE_FLOAT,
        TW_TYPE_INT32,
        TW_TYPE_BOOL32,
        TW_TYPE_UINT32,     // not used
        TW_TYPE_DIR3F,
        TW_TYPE_QUAT4F,
        TW_TYPE_COLOR3F,
    };

    StaticAssert_(_countof(twTypes) == uint64(SettingType::NumTypes));

    const ETwType twType = twType_ == TW_TYPE_UNDEF ? twTypes[uint64(type)] : twType_;
    TwCall(TwAddVarRW(tweakBar, name.c_str(), twType, data, nullptr));
    TwHelper::SetLabel(tweakBar, name.c_str(), label.c_str());
    TwHelper::SetHelpText(tweakBar, name.c_str(), helpText.c_str());
    TwHelper::SetGroup(tweakBar, name.c_str(), group.c_str());
}

void Setting::SetReadOnly(bool readOnly)
{
    Assert_(tweakBar != NULL);
    TwHelper::SetReadOnly(tweakBar, name.c_str(), readOnly);
}

void Setting::SetEditable(bool editable)
{
    SetReadOnly(!editable);
}

FloatSetting& Setting::AsFloat()
{
    Assert_(type == SettingType::Float);
    return *(static_cast<FloatSetting*>(this));
}

IntSetting& Setting::AsInt()
{
    Assert_(type == SettingType::Int);
    return *(static_cast<IntSetting*>(this));
}

BoolSetting& Setting::AsBool()
{
    Assert_(type == SettingType::Bool);
    return *(static_cast<BoolSetting*>(this));
}

EnumSetting& Setting::AsEnum()
{
    Assert_(type == SettingType::Enum);
    return *(static_cast<EnumSetting*>(this));
}

DirectionSetting& Setting::AsDirection()
{
    Assert_(type == SettingType::Direction);
    return *(static_cast<DirectionSetting*>(this));
}

OrientationSetting& Setting::AsOrientation()
{
    Assert_(type == SettingType::Orientation);
    return *(static_cast<OrientationSetting*>(this));
}

ColorSetting& Setting::AsColor()
{
    Assert_(type == SettingType::Color);
    return *(static_cast<ColorSetting*>(this));
}

bool Setting::Changed() const
{
    return changed;
}

const std::string& Setting::Name() const
{
    return name;
}

// == FloatSetting ================================================================================

FloatSetting::FloatSetting() : val(0.0f), oldVal(0.0f), minVal(0.0f), maxVal(0.0f), step(0.0)
{
}

void FloatSetting::Initialize(TwBar* tweakBar_, const char* name_, const char* group_,
                              const char* label_, const char* helpText_, float initialVal,
                              float minVal_, float maxVal_, float step_)
{
    val = initialVal;
    oldVal = initialVal;
    minVal = minVal_;
    maxVal = maxVal_;
    step = step_;
    Assert_(minVal < maxVal);
    Assert_(minVal <= val && val <= maxVal);
    Setting::Initialize(tweakBar_, SettingType::Float, &val, name_, group_, label_, helpText_);
    TwHelper::SetMinMax(tweakBar, name.c_str(), minVal, maxVal);
    TwHelper::SetStep(tweakBar, name.c_str(), step);
}

void FloatSetting::Update()
{
    changed = oldVal != val;
    oldVal = val;
}

float FloatSetting::Value() const
{
    return val;
}

void FloatSetting::SetValue(float newVal)
{
    val = Clamp(newVal, minVal, maxVal);
}

FloatSetting::operator float()
{
    return val;
}

// == IntSetting ==================================================================================

IntSetting::IntSetting() :  val(0), oldVal(0), minVal(0), maxVal(0)
{
}

void IntSetting::Initialize(TwBar* tweakBar_, const char* name_, const char* group_,
                            const char* label_, const char* helpText_, int32 initialVal,
                            int32 minVal_, int32 maxVal_)
{
    val = initialVal;
    oldVal = initialVal;
    minVal = minVal_;
    maxVal = maxVal_;
    Assert_(minVal < maxVal);
    Assert_(minVal <= val && val <= maxVal);
    Setting::Initialize(tweakBar_, SettingType::Int, &val, name_, group_, label_, helpText_);
    TwHelper::SetMinMax(tweakBar, name.c_str(), minVal, maxVal);
}

void IntSetting::Update()
{
    changed = oldVal != val;
    oldVal = val;
}

int32 IntSetting::Value() const
{
    return val;
}

void IntSetting::SetValue(int32 newVal)
{
    val = Clamp(newVal, minVal, maxVal);
}

IntSetting::operator int32()
{
    return val;
}

// == BoolSetting =================================================================================

BoolSetting::BoolSetting() : val(false), oldVal(0)
{
}

void BoolSetting::Initialize(TwBar* tweakBar_, const char* name_,
                             const char* group_, const char* label_,
                             const char* helpText_, bool32 initialVal)
{
    val = initialVal ? true : false;
    oldVal = val;
    Setting::Initialize(tweakBar_, SettingType::Bool, &val, name_, group_, label_, helpText_);
}

void BoolSetting::Update()
{
    changed = oldVal != val;
    oldVal = val;
}

bool32 BoolSetting::Value() const
{
    return val;
}

void BoolSetting::SetValue(bool32 newVal)
{
    val = newVal ? true : false;
}

BoolSetting::operator bool32()
{
    return val;
}

// == EnumSetting =================================================================================

EnumSetting::EnumSetting() : val(0), oldVal(0), numValues(0)
{
}

void EnumSetting::Initialize(TwBar* tweakBar_, const char* name_,
                             const char* group_, const char* label_,
                             const char* helpText_, uint32 initialVal,
                             uint32 numValues_, const char* const* valueLabels)
{
    val = std::min(initialVal, numValues - 1);
    numValues = numValues_;

    // Register an enum type
    std::vector<TwEnumVal> enumValues(numValues);
    for(uint32 i = 0; i < numValues; ++i)
    {
        enumValues[i].Value = i;
        enumValues[i].Label = valueLabels[i];
    }
    TwType twType = TwDefineEnum(name_, enumValues.data(), numValues);
    TwCall(twType);

    Setting::Initialize(tweakBar_, SettingType::Enum, &val, name_, group_, label_, helpText_, twType);
}

void EnumSetting::Update()
{
    changed = oldVal != val;
    oldVal = val;
}

bool32 EnumSetting::Value() const
{
    return val;
}

void EnumSetting::SetValue(uint32 newVal)
{
    val = std::min(newVal, numValues - 1);
}

EnumSetting::operator uint32()
{
    return val;
}

// == DirectionSetting ============================================================================

DirectionSetting::DirectionSetting()
{
}

void DirectionSetting::Initialize(TwBar* tweakBar_, const char* name_,
                                  const char* group_, const char* label_,
                                  const char* helpText_, Float3 initialVal)
{
    val = Float3::Normalize(initialVal);
    oldVal = val;
    Setting::Initialize(tweakBar_, SettingType::Direction, &val, name_, group_, label_, helpText_);
}

void DirectionSetting::Update()
{
    changed = oldVal != val;
    oldVal = val;
}

Float3 DirectionSetting::Value() const
{
    return val;
}

void DirectionSetting::SetValue(Float3 newVal)
{
    val = Float3::Normalize(newVal);
}

DirectionSetting::operator Float3()
{
    return val;
}

// == OrientationSetting ==========================================================================

OrientationSetting::OrientationSetting()
{
}

void OrientationSetting::Initialize(TwBar* tweakBar_,
                                    const char* name_,
                                    const char* group_,
                                    const char* label_,
                                    const char* helpText_,
                                    Quaternion initialVal)
{
    val = Quaternion::Normalize(initialVal);
    oldVal = val;

    Setting::Initialize(tweakBar_, SettingType::Orientation, &val, name_, group_, label_, helpText_);

    TwHelper::SetAxisMapping(tweakBar, name_, TwHelper::Axis::PositiveX, TwHelper::Axis::PositiveY,
                             TwHelper::Axis::NegativeZ);
}

void OrientationSetting::Update()
{
    changed = oldVal != val;
    oldVal = val;
}

Quaternion OrientationSetting::Value() const
{
    return val;
}

void OrientationSetting::SetValue(Quaternion newVal)
{
    val = Quaternion::Normalize(newVal);
}

OrientationSetting::operator Quaternion()
{
    return val;
}

// == ColorSetting ================================================================================

ColorSetting::ColorSetting() : hdr(false)
{
}

void ColorSetting::Initialize(TwBar* tweakBar_, const char* name_,
                              const char* group_, const char* label_,
                              const char* helpText_, Float3 initialVal,
                              bool hdr_, float minIntensity_, float maxIntensity_,
                              float step_)
{
    hdr = hdr_;
    float initialIntensity = 1.0f;
    if(hdr)
    {
        val = Float3::Clamp(initialVal, 0.0f, FLT_MAX);
        initialIntensity = std::max(std::max(val.x, val.y), val.z);
        val /= initialIntensity;
        initialIntensity = Clamp(initialIntensity, minIntensity_, maxIntensity_);
    }
    else
        val = Float3::Clamp(initialVal, 0.0f, 1.0f);

    oldVal = val;
    Setting::Initialize(tweakBar_, SettingType::Color, &val, name_, group_, label_, helpText_);

    if(hdr)
    {
        std::string intensityName = name_;
        intensityName += "Intensity";
        std::string intensityLabel = label_;
        intensityLabel += " Intensity";
        std::string intensityHelp = "The intensity of the ";
        intensityHelp += name_;
        intensityHelp += " setting";
        intensity.Initialize(tweakBar_, intensityName.c_str(), group_,
                             intensityLabel.c_str(), intensityHelp.c_str(),
                             initialIntensity, minIntensity_, maxIntensity_, step_);
    }
}

void ColorSetting::Update()
{
    changed = oldVal != val;
    oldVal = val;
}

Float3 ColorSetting::Value() const
{
    return hdr ? val * intensity.Value() : val;
}

void ColorSetting::SetValue(Float3 newVal)
{
    if(hdr)
    {
        newVal = Float3::Clamp(newVal, 0.0f, FLT_MAX);
        float newIntensity = std::max(std::max(val.x, val.y), val.z);
        newVal /= newIntensity;
        intensity.SetValue(newIntensity);
    }

    val = Float3::Clamp(newVal, 0.0f, 1.0f);
}

ColorSetting::operator Float3()
{
    return hdr ? val * intensity.Value() : val;
}

// == SettingsContainer ===========================================================================

SettingsContainer::SettingsContainer() : tweakBar(nullptr)
{
}

SettingsContainer::~SettingsContainer()
{
    settings.clear();
    for(uint64 i = 0; i < allocatedSettings.size(); ++i)
        delete allocatedSettings[i];
}

void SettingsContainer::Initialize(TwBar* tweakBar_)
{
    Assert_(tweakBar_ != nullptr);
    tweakBar = tweakBar_;
}

void SettingsContainer::Update()
{
    for(auto iter = settings.begin(); iter != settings.end(); iter++)
        iter->second->Update();
}

void SettingsContainer::SetGroupOpened(const char* groupName, bool opened)
{
    TwHelper::SetOpened(tweakBar, groupName, opened);
}

Setting& SettingsContainer::operator[](const std::string& name)
{
    Assert_(settings.find(name) != settings.end());
    return *settings[name];
}

void SettingsContainer::AddFloatSetting(const char* name, const char* label, const char* group,
                                        float initialVal, float minVal, float maxVal, float step,
                                        const char* helpText)
{
    Assert_(settings.find(name) == settings.end());
    Assert_(tweakBar != nullptr);
    FloatSetting* setting = new FloatSetting();
    setting->Initialize(tweakBar, name, group, label, helpText, initialVal, minVal, maxVal, step);
    settings[name] = setting;
    allocatedSettings.push_back(setting);
}

void SettingsContainer::AddIntSetting(const char* name, const char* label, const char* group,
                                      int32 initialVal, int32 minVal, int32 maxVal,
                                      const char* helpText)
{
    Assert_(settings.find(name) == settings.end());
    Assert_(tweakBar != nullptr);
    IntSetting* setting = new IntSetting();
    setting->Initialize(tweakBar, name, group, label, helpText, initialVal, minVal, maxVal);
    settings[name] = setting;
    allocatedSettings.push_back(setting);
}

void SettingsContainer::AddBoolSetting(const char* name, const char* label, const char* group,
                                       bool32 initialVal, const char* helpText)
{
    Assert_(settings.find(name) == settings.end());
    Assert_(tweakBar != nullptr);
    BoolSetting* setting = new BoolSetting();
    setting->Initialize(tweakBar, name, group, label, helpText, initialVal);
    settings[name] = setting;
    allocatedSettings.push_back(setting);
}

void SettingsContainer::AddEnumSetting(const char* name, const char* label, const char* group,
                                       uint32 initialVal, uint32 numValues,
                                       const char* const* valueLabels, const char* helpText)
{
    Assert_(settings.find(name) == settings.end());
    Assert_(tweakBar != nullptr);
    EnumSetting* setting = new EnumSetting();
    setting->Initialize(tweakBar, name, group, label, helpText, initialVal, numValues, valueLabels);
    settings[name] = setting;
    allocatedSettings.push_back(setting);
}

void SettingsContainer::AddDirectionSetting(const char* name, const char* label, const char* group,
                                            Float3 initialVal, const char* helpText)
{
    Assert_(settings.find(name) == settings.end());
    Assert_(tweakBar != nullptr);
    DirectionSetting* setting = new DirectionSetting();
    setting->Initialize(tweakBar, name, group, label, helpText, initialVal);
    settings[name] = setting;
    allocatedSettings.push_back(setting);
}

void SettingsContainer::AddOrientationSetting(const char* name, const char* label, const char* group,
                                              Quaternion initialVal, const char* helpText)
{
    Assert_(settings.find(name) == settings.end());
    Assert_(tweakBar != nullptr);
    OrientationSetting* setting = new OrientationSetting();
    setting->Initialize(tweakBar, name, group, label, helpText, initialVal);
    settings[name] = setting;
    allocatedSettings.push_back(setting);
}

void SettingsContainer::AddColorSetting(const char* name, const char* label, const char* group,
                                        Float3 initialVal, bool hdr, float minIntensity,
                                        float maxIntensity, float step, const char* helpText)
{
    Assert_(settings.find(name) == settings.end());
    Assert_(tweakBar != nullptr);
    ColorSetting* setting = new ColorSetting();
    setting->Initialize(tweakBar, name, group, label, helpText, initialVal,
                        hdr, minIntensity, maxIntensity, step);
    settings[name] = setting;
    allocatedSettings.push_back(setting);
}

void SettingsContainer::AddSetting(Setting* setting)
{
    Assert_(setting != NULL);
    settings[setting->Name()] = setting;
}

// Global definition
SettingsContainer Settings;

}