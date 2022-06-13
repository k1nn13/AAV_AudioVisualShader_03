#version 150

out vec4 fragColor;

in vec2 texCoordVarying;

uniform float u_time;
uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_amount;
uniform float mode;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D imageMask;

uniform int u_spiralFX;
uniform vec3 u_spiralParam;
uniform vec3 u_spiralParam2;

uniform int u_waterFX;
uniform int u_waterAmount;
uniform vec2 u_waterSpeed;

uniform int u_noiseFX;
uniform float u_noiseDepth;

uniform int u_crossHatchFX;
uniform vec2 u_coord;

uniform int u_morphFX;
uniform vec2 u_morphAmount;


//-----------------------//
// randomization function taken from morph grid, lewis lepton
float random2d(vec2 coord){
    return fract(sin(dot(coord.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

//-----------------------//
void main() {

vec2 uv = (texCoordVarying.xy/u_resolution.xy);
    

    //---TEXTURES--//
    //modified from openframeworks example multi texture
    vec4 rTxt = texture(tex0, texCoordVarying); //img1
    vec4 gTxt = texture(tex2, texCoordVarying); //img2
    vec4 bTxt = texture(tex1, texCoordVarying); //img3
    vec4 mask = texture(imageMask, texCoordVarying); //webcam image to use

    //---MIX-TEXTURES---//
    //combine images/rgb with webcam
    vec4 color = vec4(0, 0, 0, 0);
	color = mask;
    //color = mix(color, rTxt, mask.r);
   // color = mix(color, gTxt, mask.g);
   // color = mix(color, bTxt, mask.b);
    //=============================//
    
    
    //--ADD-EFFECT-TO-TEXTURES--//
    
    //---SPIRAL-EFFECT---//
    //rainbow swirl - modified from lewis lepton, shader tutorial series:
    //https://www.youtube.com/watch?v=wkWYXjrOVlA&list=PL4neAtv21WOmIrTrkNO3xCyrxg4LKkrF7&index=19
    
    if (u_spiralFX == 1) {
        //helps to turn the swirl
        float angle = atan(-texCoordVarying.y + 0.5, texCoordVarying.x - 0.5) * 0.5;
        float len = length(texCoordVarying - vec2(0.5, 0.5));
        
        //add color rgb values to texure color, alternate colors using cos and sin
        color.r += sin(len * u_spiralParam.x + angle * u_spiralParam2.x + u_time);
        color.g += cos(len * u_spiralParam.y + angle * u_spiralParam2.y - u_time);
        color.b += sin(len * u_spiralParam.z + angle * u_spiralParam2.z + u_time);
    }
    //===============================================//

    
    
    //---NOISE-EFFECT---//
    //morph grid - modified from lewis lepton, shader tutorial series:
    //https://www.youtube.com/watch?v=EO2ax570wKo&list=PL4neAtv21WOmIrTrkNO3xCyrxg4LKkrF7&index=24
    
    if (u_noiseFX == 1) {
        vec2 coord = (gl_FragCoord.xy * u_noiseDepth);//zoom in on noise, u_noiseDepth
        coord -= u_time + vec2(sin(coord.y), cos(coord.x));//animate coord

        //u_time animate pixels
        float rand01 = fract(random2d(floor(texCoordVarying)) + u_time / 30.0);
        float rand02 = fract(random2d(floor(texCoordVarying)) + u_time / 10.0);

        rand01 *= 0.4 - length(fract(coord));

        color.r -= rand01 * 4;
        color.g -= rand02 * 4;
        //color.b -= rand02 * rand01 * 10.0;
    }
    //================================//
    
    
    //---WATER-COLOR---//
    //water color - modified from lewis lepton, shader tutorial series:
    //https://www.youtube.com/watch?v=ye_JlwUIyto&list=PL4neAtv21WOmIrTrkNO3xCyrxg4LKkrF7&index=16
    
    if (u_waterFX == 1) {
        vec2 waterCoord = 20.0 * ((gl_FragCoord.xy - u_resolution / 2.7) / min(u_resolution.y, u_resolution.x));
        float waterLen;

        for (int i = 0; i < u_waterAmount; i++) {
            waterLen = length(vec2(waterCoord.x, waterCoord.y));//returns length of a vector
            waterCoord.x = waterCoord.x - cos(waterCoord.y + sin(waterLen)) + cos(u_time / u_waterSpeed.x);
            waterCoord.y = waterCoord.y + sin(waterCoord.x + cos(waterLen)) + sin(u_time / u_waterSpeed.y);
        }

        color.r += cos(waterLen * color.r);
        color.g += cos(waterLen * color.g);
        color.b += cos(waterLen * color.b);
    }
    //====================//
    
    
    //---CROSS-HATCH---//
    //modified from lewis lepton, shader tutorial series:
    //https://www.youtube.com/watch?v=vi2Ae2K1GQY&list=PL4neAtv21WOmIrTrkNO3xCyrxg4LKkrF7&index=29
    if (u_crossHatchFX == 1) {
        color.r += sin(texCoordVarying.x * u_coord.x);
        color.b += cos(texCoordVarying.y * u_coord.y);
    }
    //====================//
    
    //---MORPH-GRID---//
    //modified from lewis lepton, shader tutorial series:
    //https://www.youtube.com/watch?v=vHNnpZdrO-0&list=PL4neAtv21WOmIrTrkNO3xCyrxg4LKkrF7&index=23
    
    if (u_morphFX == 1) {
        vec2 morphCoord = gl_FragCoord.xy * 1.0 - u_resolution;
        color -= abs(cos(morphCoord.x / u_morphAmount.x) + sin(morphCoord.y / u_morphAmount.y) - cos(u_time));
    }








    
    //---------------//
    fragColor = color;
    //===============//
}
