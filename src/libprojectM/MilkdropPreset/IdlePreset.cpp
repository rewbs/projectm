#include "IdlePreset.hpp"

#include "MilkdropPreset.hpp"

#include <sstream>
#include <string>

namespace libprojectM {
namespace MilkdropPreset {

// Original preset name: "Geiss & Sperl - Feedback (projectM idle HDR mix).milk"
std::string IdlePresets::presetText()
{    return R"(MILKDROP_PRESET_VERSION=201
PSVERSION=3
PSVERSION_WARP=3
PSVERSION_COMP=3
[preset00]
fRating=5.000
fGammaAdj=1.000
fDecay=0.1
fVideoEchoZoom=1.500
fVideoEchoAlpha=0.80
nVideoEchoOrientation=0
nWaveMode=0
bAdditiveWaves=1 
bWaveDots=0
bWaveThick=1
bModWaveAlphaByVolume=0
bMaximizeWaveColor=0
bTexWrap=1
bDarkenCenter=0
bRedBlueStereo=0
bBrighten=0
bDarken=1
bSolarize=0
bInvert=0
fWaveAlpha=100.000
fWaveScale=5.360
fWaveSmoothing=0.8
fWaveParam=0.000
fModWaveAlphaStart=1
fModWaveAlphaEnd=1
fWarpAnimSpeed=0.500
fWarpScale=100000
fZoomExponent=.5
fShader=0.000
zoom=0.99980 
rot=0.01000
cx=0.500
cy=0.500
dx=0.00000
dy=0.01000
warp=100
sx=0.9
sy=0.9
wave_r=0.000
wave_g=0.000
wave_b=0.000 
wave_x=0.000
wave_y=0.500
ob_size=0.800
ob_r=0.000
ob_g=0.000
ob_b=0.000
ob_a=0.010
ib_size=0.500
ib_r=0.000
ib_g=0.000
ib_b=0.000
ib_a=0.010
nMotionVectorsX=0.000
nMotionVectorsY=0.000
mv_dx=0.000
mv_dy=0.000
mv_l=1.000
mv_r=1.000
mv_g=0.974
mv_b=0.710
mv_a=1.000
b1n=0.000
b2n=0.000
b3n=0.000
b1x=1.000
b2x=1.000
b3x=1.000
b1ed=0.0

per_frame_init_1=base_att_att = 0;
per_frame_1=base_att_att = base_att_att * 0.85 + bass_att * 0.10;
per_frame_2=q1=base_att_att;

per_frame_3=mid_att_att = mid_att_att * 0.75 + mid_att * 0.10;
per_frame_4=q2=mid_att_att;


per_frame_5=treb_att_att = treb_att_att * 0.75 + treb_att * 0.10;
per_frame_6=q3=treb_att_att;

per_frame_7=q4=tot_att_att = treb_att_att + mid_att_att + base_att_att;

per_frame_8=switch=if (above(tot_att_att,1) , 1, 0);

shapecode_1_enabled=1
shapecode_1_sides=3
shapecode_1_additive=0
shapecode_1_thickOutline=1
shapecode_1_textured=0
shapecode_1_num_inst=1
shapecode_1_x=0.800
shapecode_1_y=0.800
shapecode_1_rad=0.3
shapecode_1_ang=0.00000
shapecode_1_tex_ang=0.00000
shapecode_1_tex_zoom=1.00000
shapecode_1_r=1.000
shapecode_1_g=1.000
shapecode_1_b=1.000
shapecode_1_a=0.500
shapecode_1_r2=1.000
shapecode_1_g2=1.000
shapecode_1_b2=1.000
shapecode_1_a2=1.000
shapecode_1_border_r=1.000
shapecode_1_border_g=1.000
shapecode_1_border_b=1.000 
shapecode_1_border_a=0.200
shape_1_per_frame1=rad=0.1+(q3/2);
shape_1_per_frame2=y=sin(time)/5+0.5;
shape_1_per_frame3=x=cos(time)/5+0.5;



shapecode_2_enabled=1
shapecode_2_sides=5
shapecode_2_additive=0
shapecode_2_thickOutline=1
shapecode_2_textured=0
shapecode_2_num_inst=1
shapecode_2_x=0.500
shapecode_2_y=0.500
shapecode_2_rad=0.3
shapecode_2_ang=0.00000
shapecode_2_tex_ang=0.00000
shapecode_2_tex_zoom=1.00000
shapecode_2_r=1.000
shapecode_2_g=1.000
shapecode_2_b=1.000
shapecode_2_a=0.500
shapecode_2_r2=1.000
shapecode_2_g2=1.000
shapecode_2_b2=1.000
shapecode_2_a2=1.000
shapecode_2_border_r=1.000
shapecode_2_border_g=1.000
shapecode_2_border_b=1.000
shapecode_2_border_a=0.200
shape_2_per_frame1=rad=0.1+(q2/2);
shape_2_per_frame2=ang=time;


shapecode_3_enabled=1
shapecode_3_sides=100
shapecode_3_additive=0
shapecode_3_thickOutline=1
shapecode_3_textured=0
shapecode_3_num_inst=1
shapecode_3_x=0.200
shapecode_3_y=0.200
shapecode_3_rad=0.3
shapecode_3_ang=0.00000
shapecode_3_tex_ang=0.00000
shapecode_3_tex_zoom=1.00000
shapecode_3_r=1.000
shapecode_3_g=1.000
shapecode_3_b=1.000
shapecode_3_a=0.500
shapecode_3_r2=1.000
shapecode_3_g2=1.000
shapecode_3_b2=1.000
shapecode_3_a2=1.000
shapecode_3_border_r=1.000
shapecode_3_border_g=1.000
shapecode_3_border_b=1.000
shapecode_3_border_a=0.200
shape_3_per_frame1=rad=0.1+(q1/2);
shape_3_per_frame2=y=sin(-time)/2.5+0.5;
shape_3_per_frame3=x=cos(-time)/2.5+0.5;
)";
}

std::unique_ptr<Preset>
IdlePresets::allocate()
{
    std::istringstream in(presetText());
    return std::unique_ptr<Preset>(new MilkdropPreset(in));
}

} // namespace MilkdropPreset
} // namespace libprojectM
