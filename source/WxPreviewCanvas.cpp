#include "terrainlab/WxPreviewCanvas.h"
#include "terrainlab/MessageID.h"
#include "terrainlab/SplatRenderer.h"
#include "terrainlab/SplatPbrRenderer.h"
#include "terrainlab/TemplateBrushOP.h"
#include "terrainlab/NoiseBrushOP.h"
#include "terrainlab/RegistNodes.h"
#include "terrainlab/node/FullView2D.h"
#include "terrainlab/node/FullView3D.h"
#include "terrainlab/node/Clipmap.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
#include <ee2/CamControlOP.h>
#include <ee3/CameraDriveOP.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <blueprint/MessageID.h>

#include <node0/SceneNode.h>
#include <node3/RenderSystem.h>
#include <painting2/RenderSystem.h>
#include <painting2/OrthoCamera.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <painting3/PerspCam.h>
#include <painting3/Shader.h>
#include <renderpipeline/HeightfieldGrayRenderer.h>
#include <tessellation/Painter.h>
#include <terraingraph/Device.h>
#include <terraingraph/device/TemplateBrush.h>

namespace
{

const float    NODE_RADIUS = 10;
const uint32_t LIGHT_SELECT_COLOR = 0x88000088;

}

namespace terrainlab
{

WxPreviewCanvas::WxPreviewCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
                                 const ee0::RenderContext& rc)
    : ee3::WxStageCanvas(stage, ECS_WORLD_VAR &rc, nullptr, true)
{
    //m_hf_rd = std::make_shared<rp::HeightfieldGrayRenderer>();
    m_hf_rd = std::make_shared<SplatRenderer>();
    //m_hf_rd = std::make_shared<SplatPbrRenderer>();

    auto sub_mgr = stage->GetSubjectMgr();
    sub_mgr->RegisterObserver(MSG_HEIGHTMAP_CHANGED, this);

    m_cam3d = m_camera;
    m_cam2d = std::make_shared<pt2::OrthoCamera>();
}

WxPreviewCanvas::~WxPreviewCanvas()
{
    if (m_graph_page)
    {
        auto sub_mgr = m_graph_page->GetSubjectMgr();
        sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
    }

    auto sub_mgr = m_stage->GetSubjectMgr();
    sub_mgr->UnregisterObserver(MSG_HEIGHTMAP_CHANGED, this);
}

void WxPreviewCanvas::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
    ee3::WxStageCanvas::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		OnSelectionInsert(variants);
		break;

    case MSG_HEIGHTMAP_CHANGED:
        SetupRenderer();
        break;
	}
}

void WxPreviewCanvas::SetGraphPage(const bp::WxGraphPage<terraingraph::DeviceVarType>* graph_page)
{
    m_graph_page = graph_page;

    auto sub_mgr = m_graph_page->GetSubjectMgr();
    sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);

    // regist editor's sub_mgr to brush ops
    for (size_t i = 0; i < OP_MAX_NUM; ++i)
    {
        if (!m_ops[i]) {
            continue;
        }
        auto brush_op = std::dynamic_pointer_cast<BrushDrawOP>(m_ops[i]);
        if (brush_op) {
            brush_op->SetEditorSubMgr(m_graph_page->GetSubjectMgr());
        }
    }
}

void WxPreviewCanvas::InitEditOP()
{
    auto cam = m_camera;
    auto& vp = GetViewport();
    auto sub = m_stage->GetSubjectMgr();

    m_ops[OP_CAMERA_2D]   = std::make_shared<ee2::CamControlOP>(m_cam2d, sub);
    m_ops[OP_CAMERA_3D]   = std::make_shared<ee3::CameraDriveOP>(cam, vp, sub);
    m_ops[OP_TEMP_BRUSH]  = std::make_shared<TemplateBrushOP>(cam, vp, sub, m_hf_rd);
    m_ops[OP_NOISE_BRUSH] = std::make_shared<NoiseBrushOP>(cam, vp, sub, m_hf_rd);

    m_stage->GetImpl().SetEditOP(m_ops[OP_CAMERA_3D]);
}

void WxPreviewCanvas::DrawBackground3D() const
{
//    ee3::WxStageCanvas::DrawBackgroundGrids(10.0f, 0.2f);
//    ee3::WxStageCanvas::DrawBackgroundCross();
}

void WxPreviewCanvas::DrawForeground3D() const
{
    auto& wc = std::const_pointer_cast<pt3::WindowContext>(GetWidnowContext().wc3);
    auto& shaders = m_hf_rd->GetAllShaders();
    if (!shaders.empty()) {
        assert(shaders.size() == 1);
        if (shaders[0]->get_type() == rttr::type::get<pt3::Shader>()) {
            std::static_pointer_cast<pt3::Shader>(shaders[0])->AddNotify(wc);
        }
    }
    {
        auto& shaders = m_overlay_rd.GetAllShaders();
        if (!shaders.empty()) {
            assert(shaders.size() == 1);
            if (shaders[0]->get_type() == rttr::type::get<pt3::Shader>()) {
                std::static_pointer_cast<pt3::Shader>(shaders[0])->AddNotify(wc);
            }
        }
    }
    // update brush op's renderer
    for (size_t i = 0; i < OP_MAX_NUM; ++i)
    {
        if (!m_ops[i]) {
            continue;
        }
        auto brush_op = std::dynamic_pointer_cast<BrushDrawOP>(m_ops[OP_TEMP_BRUSH]);
        if (!brush_op) {
            continue;
        }
        auto renderer = brush_op->GetRenderer();
        auto& shaders = renderer->GetAllShaders();
        if (!shaders.empty()) {
            assert(shaders.size() == 1);
            if (shaders[0]->get_type() == rttr::type::get<pt3::Shader>()) {
                std::static_pointer_cast<pt3::Shader>(shaders[0])->AddNotify(wc);
            }
        }
    }
    m_full3_rd.Setup(wc);
    m_clip_rd.Setup(wc);

    pt0::RenderContext rc;
    rc.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );
    if (m_camera->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
    {
        auto persp = std::static_pointer_cast<pt3::PerspCam>(m_camera);
        rc.AddVar(
            pt3::MaterialMgr::PositionUniforms::cam_pos.name,
            pt0::RenderVariant(persp->GetPos())
        );
    }
    assert(wc);
    rc.AddVar(
        pt3::MaterialMgr::PosTransUniforms::view.name,
        pt0::RenderVariant(wc->GetViewMat())
    );
    rc.AddVar(
        pt3::MaterialMgr::PosTransUniforms::projection.name,
        pt0::RenderVariant(wc->GetProjMat())
    );

    tess::Painter pt;

    auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();

    DrawSelected(pt, cam_mat, rc);

    pt2::RenderSystem::DrawPainter(pt);
}

void WxPreviewCanvas::DrawForeground2D() const
{
}

void WxPreviewCanvas::OnSelectionInsert(const ee0::VariantSet& variants)
{
    auto var_obj = variants.GetVariant("obj");
    GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
    GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

    m_selected = obj;

    SetupRenderer();

    auto node = GetSelectedNode();
    if (node)
    {
        m_camera = m_cam3d;

        auto type = node->get_type();
        if (type == rttr::type::get<node::TemplateBrush>())
        {
            std::shared_ptr<hf::HeightField> hf = nullptr;

            auto eval = m_graph_page->GetEval();
            if (eval) {
                auto back_node = eval->QueryBackNode(*node);
                if (back_node) {
                    hf = std::static_pointer_cast<terraingraph::device::TemplateBrush>(back_node)->GetBrush();
                }
            }

            if (hf)
            {
                auto brush_op = std::static_pointer_cast<TemplateBrushOP>(m_ops[OP_TEMP_BRUSH]);
                brush_op->Setup(std::static_pointer_cast<Node>(node), hf, obj);
                m_stage->GetImpl().SetEditOP(m_ops[OP_TEMP_BRUSH]);
            }
        }
        else if (type == rttr::type::get<node::NoiseBrush>())
        {
            auto brush_op = std::static_pointer_cast<NoiseBrushOP>(m_ops[OP_NOISE_BRUSH]);
            brush_op->Setup(std::static_pointer_cast<Node>(node), nullptr, obj);
            m_stage->GetImpl().SetEditOP(m_ops[OP_NOISE_BRUSH]);
        }
        else if (type == rttr::type::get<node::FullView2D>())
        {
            m_camera = m_cam2d;
            m_stage->GetImpl().SetEditOP(m_ops[OP_CAMERA_2D]);
        }
        else
        {
            m_stage->GetImpl().SetEditOP(m_ops[OP_CAMERA_3D]);
        }
    }
}

void WxPreviewCanvas::DrawSelected(tess::Painter& pt, const sm::mat4& cam_mat,
                                   const pt0::RenderContext& rc) const
{
    auto node = GetSelectedNode();
    if (!node) {
        return;
    }

    auto type = node->get_type();
    if (type == rttr::type::get<node::FullView2D>())
    {
        m_full2_rd.Draw();
        return;
    }
    else if (type == rttr::type::get<node::FullView3D>())
    {
        auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_cam3d);

        const_cast<FullView3dRenderer&>(m_full3_rd).Update();
        m_full3_rd.Draw(p_cam->GetPos()/*, sm::mat4(), true*/);

        return;
    }
    else if (type == rttr::type::get<node::Clipmap>())
    {
        m_clip_rd.Draw();
        return;
    }

    auto device = GetSelectedDevice();
    if (!device) {
        return;
    }

    auto hf = device->GetHeightField();
    auto bmp = device->GetBitmap();
    auto mask = device->GetMask();
    if (hf && bmp) {
        m_overlay_rd.Draw();
    } else if (hf) {
        m_hf_rd->Draw();
    } else if (bmp) {
        m_img_rd.Draw();
    } else if (mask) {
        m_img_rd.Draw();
    }
}

void WxPreviewCanvas::SetupRenderer()
{
    auto device = GetSelectedDevice();
    if (!device) {
        return;
    }

    auto hf = device->GetHeightField();
    auto bmp = device->GetBitmap();
    auto mask = device->GetMask();
    if (hf && bmp) {
        m_overlay_rd.Setup(hf, bmp);
    } else if (hf) {
        m_hf_rd->Setup(hf);
    } else if (bmp) {
        m_img_rd.Setup(bmp);
    } else if (mask) {
        m_img_rd.Setup(mask);
    }

    SetDirty();
}

bp::NodePtr WxPreviewCanvas::GetSelectedNode() const
{
    if (!m_selected || !m_selected->HasUniqueComp<bp::CompNode>()) {
        return nullptr;
    }

    auto eval = m_graph_page->GetEval();
    if (!eval) {
        return nullptr;
    }

    auto& cnode = m_selected->GetUniqueComp<bp::CompNode>();
    auto bp_node = cnode.GetNode();
    if (!bp_node) {
        return nullptr;
    }

    return cnode.GetNode();
}

terraingraph::DevicePtr WxPreviewCanvas::GetSelectedDevice() const
{
    if (!m_graph_page) {
        return nullptr;
    }

    auto eval = m_graph_page->GetEval();
    if (!eval) {
        return nullptr;
    }

    auto front_node = GetSelectedNode();
    if (front_node) {
        return std::static_pointer_cast<terraingraph::Device>(eval->QueryBackNode(*front_node));
    } else {
        return nullptr;
    }
}

}
