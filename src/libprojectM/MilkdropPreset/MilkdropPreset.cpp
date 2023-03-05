/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

#include "MilkdropPreset.hpp"

#include "MilkdropPresetExceptions.hpp"
#include "MilkdropPresetFactory.hpp"
#include "PresetFileParser.hpp"
#include "PresetFrameIO.hpp"

#ifdef MILKDROP_PRESET_DEBUG
#include <iostream>
#endif


MilkdropPreset::MilkdropPreset(std::istream& presetData)
    : m_perFrameContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_perPixelContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_waveform(m_state)
{
    Load(presetData);
}


MilkdropPreset::MilkdropPreset(const std::string& absoluteFilePath)
    : m_absoluteFilePath(absoluteFilePath)
    , m_perFrameContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_perPixelContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_waveform(m_state)
{
    Load(absoluteFilePath);
}

void MilkdropPreset::InitializePreset(PresetFileParser& parsedFile)
{
    // Create the offscreen rendering surfaces.
    m_framebuffer.CreateColorAttachment(0, 0);
    m_framebuffer.CreateColorAttachment(1, 0);

    // Load global init variables into the state
    m_state.Initialize(parsedFile);

    // Custom waveforms:
    for (int i = 0; i < CustomWaveformCount; i++)
    {
        auto wave = std::make_unique<CustomWaveform>(m_state);
        wave->Initialize(parsedFile, i);
        m_customWaveforms[i] = std::move(wave);
    }

    // Custom shapes:
    for (int i = 0; i < CustomShapeCount; i++)
    {
        auto shape = std::make_unique<CustomShape>(m_state);
        shape->Initialize(parsedFile, i);
        m_customShapes[i] = std::move(shape);
    }

    CompileCodeAndRunInitExpressions();
}

void MilkdropPreset::CompileCodeAndRunInitExpressions()
{
    // Per-frame init and code
    m_perFrameContext.RegisterBuiltinVariables();
    m_perFrameContext.LoadStateVariables(m_state);
    m_perFrameContext.EvaluateInitCode(m_state);
    m_perFrameContext.CompilePerFrameCode(m_state.perFrameCode);

    // Per-vertex code
    m_perPixelContext.CompilePerPixelCode(m_state.perPixelCode);

    for (int i = 0; i < CustomWaveformCount; i++)
    {
        auto& wave = m_customWaveforms[i];
        wave->CompileCodeAndRunInitExpressions(m_perFrameContext);
    }

    for (int i = 0; i < CustomShapeCount; i++)
    {
        auto& shape = m_customShapes[i];
        shape->CompileCodeAndRunInitExpressions(m_perFrameContext);
    }
}

void MilkdropPreset::Load(const std::string& pathname)
{
#ifdef MILKDROP_PRESET_DEBUG
    std::cerr << "[Preset] Loading preset from file \"" << pathname << "\"." << std::endl;
#endif

    SetFilename(ParseFilename(pathname));

    PresetFileParser parser;

    if (!parser.Read(pathname))
    {
        throw MilkdropPresetLoadException("Could not parse preset file \"" + pathname + "\"");
    }

    InitializePreset(parser);
}

void MilkdropPreset::Load(std::istream& stream)
{
#ifdef MILKDROP_PRESET_DEBUG
    std::cerr << "[Preset] Loading preset from stream." << std::endl;
#endif

    PresetFileParser parser;

    if (!parser.Read(stream))
    {
        throw MilkdropPresetLoadException("Could not parse preset data.");
    }

    InitializePreset(parser);
}

void MilkdropPreset::RenderFrame(const libprojectM::Audio::FrameAudioData& audioData, const RenderContext& renderContext)
{
    m_state.audioData = audioData;
    m_state.renderContext = renderContext;

    // Update framebuffer size if needed
    m_framebuffer.SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY);

    // First evaluate per-frame code
    PerFrameUpdate();

    // Motion vector field. Drawn to the previous frame texture before warping it.
    m_framebuffer.Bind(1);
    // ToDo: Move this to the draw call and pass in the per-frame context.
    m_motionVectors.r = static_cast<float>(*m_perFrameContext.mv_r);
    m_motionVectors.g = static_cast<float>(*m_perFrameContext.mv_g);
    m_motionVectors.b = static_cast<float>(*m_perFrameContext.mv_b);
    m_motionVectors.a = static_cast<float>(*m_perFrameContext.mv_a);
    m_motionVectors.length = static_cast<float>(*m_perFrameContext.mv_l);
    m_motionVectors.x_num = static_cast<float>(*m_perFrameContext.mv_x);
    m_motionVectors.y_num = static_cast<float>(*m_perFrameContext.mv_y);
    m_motionVectors.x_offset = static_cast<float>(*m_perFrameContext.mv_dx);
    m_motionVectors.y_offset = static_cast<float>(*m_perFrameContext.mv_dy);
    m_motionVectors.Draw(renderContext);

    // We now draw to the first framebuffer, but read from the second one for warping.
    m_framebuffer.BindRead(1);
    m_framebuffer.BindDraw(0);

    // Draw previous frame image warped via per-pixel mesh and warp shader
    // ToDo: ComputeGridAlphaValues
    // ToDo: Per-Pixel and warp stuff

    // Draw audio-data-related stuff
    for (auto& shape : m_customShapes)
    {
        shape->Draw(m_perFrameContext);
    }
    for (auto& wave : m_customWaveforms)
    {
        wave->Draw(m_perFrameContext);
    }
    m_waveform.Draw();

    // ToDo: Sprite drawing would go here.

    // Todo: Song title anim would go here

    // Copy pixels from framebuffer index 0 to 1
    m_framebuffer.BindRead(0);
    m_framebuffer.BindDraw(1);
    glBlitFramebuffer(0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY,
                      0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // ToDo: Apply composite shader
    m_framebuffer.Bind(0);

    // ToDo: Draw user sprites (can have evaluated code)
}


void MilkdropPreset::PerFrameUpdate()
{
}

auto MilkdropPreset::ParseFilename(const std::string& filename) -> std::string
{
    const std::size_t start = filename.find_last_of('/');

    if (start == std::string::npos || start >= (filename.length() - 1))
    {
        return "";
    }

    return filename.substr(start + 1, filename.length());
}
