#pragma once

#include <blueprint/Node.h>
#include <terraingraph/Device.h>

namespace terrainlab
{

class Node : public bp::Node
{
public:
    Node(const std::string& title, bool props = false);
    Node(const Node& node);
    Node& operator = (const Node& node);
    virtual ~Node();

    virtual void Draw(const n2::RenderParams& rp) const override;

    auto& GetName() const { return m_name; }
    void  SetName(const std::string& name) { m_name = name; }

    bool GetDisplay() const { return m_display; }
    void SetDisplay(bool display) { m_display = display; }

    void UpdatePins(const terraingraph::Device& node);

protected:
    struct PinDesc
    {
        bool operator == (const PinDesc& desc) const {
            return type == desc.type && name == desc.name;
        }

        int         type;
        std::string name;
    };

public:
    static const char* STR_PROP_DISPLAY;

protected:
    void InitPins(const std::vector<PinDesc>& input,
        const std::vector<PinDesc>& output);
    void InitPins(const std::string& name);

private:
    void InitPinsImpl(const std::vector<PinDesc>& pins,
        bool is_input);

    static void PortBack2Front(std::vector<PinDesc>& dst,
        const std::vector<terraingraph::Device::Port>& src);

private:
    std::string m_name;

    bool m_bypass  = false;
    bool m_display = true;

    RTTR_ENABLE(bp::Node)

}; // Node

}