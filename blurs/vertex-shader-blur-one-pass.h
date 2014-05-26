#ifndef VERTEX_SHADER_BLUR_ONE_PASS_H
#define VERTEX_SHADER_BLUR_ONE_PASS_H

/////////////////////////////////  MIT LICENSE  ////////////////////////////////

//  Copyright (C) 2014 TroggleMonkey
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
//  sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.


/////////////////////////////  SETTINGS MANAGEMENT  ////////////////////////////

//  PASS SETTINGS:
//  Pass settings should be set by the .cg file that #includes this one.


//////////////////////////////////  INCLUDES  //////////////////////////////////

#include "../include/gamma-management.h"
#include "../include/blur-functions.h"


/////////////////////////////////  STRUCTURES  /////////////////////////////////

struct input
{
    float2 video_size;
    float2 texture_size;
    float2 output_size;
    float frame_count;
    float frame_direction;
    float frame_rotation;
};

struct out_vertex
{
    float4 position         : POSITION;
    float2 tex_uv           : TEXCOORD0;
    float2 blur_dxdy        : TEXCOORD1;
};


////////////////////////////////  VERTEX SHADER  ///////////////////////////////

out_vertex main_vertex
(
	float4 position : POSITION,
	float4 color    : COLOR,
	float2 tex_uv   : TEXCOORD0,
	uniform float4x4 modelViewProj,
	uniform input IN
)
{
	out_vertex OUT;
	OUT.position = mul(modelViewProj, position);
	OUT.tex_uv = tex_uv;

	//  Get the uv sample distance between output pixels.  Statically weighted
    //  blurs are not generic resizers though, and correct blurs require:
    //  1.) IN.output_size == IN.video_size * 2^m, where m is an integer <= 0.
    //  2.) mipmap_inputN = "true" for this pass in .cgp preset if m < 0
    //  3.) filter_linearN = "true" for all one-pass blurs
    //  Generic resizers would upsize using the distance between input texels
    //  (not output pixels), but we avoid this and leave the blur kernel small:
    //  Otherwise, combining statically calculated weights with bilinear sample
    //  exploitation would result in terrible artifacts.  If you want to upsize
    //  and blur, use blurNnearest() or a Gaussian resize with dynamic weights.
    const float2 dxdy_scale = IN.video_size/IN.output_size;
	OUT.blur_dxdy = dxdy_scale/IN.texture_size;

	return OUT;
}


#endif  //  VERTEX_SHADER_BLUR_ONE_PASS_H

