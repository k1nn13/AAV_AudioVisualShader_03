#version 150

uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 varyingtexcoord;

uniform sampler2D tex0;
uniform float time;
uniform float mouseX;
uniform float mouseRange;
uniform vec2 mousePos;
uniform int u_magnifyingFX;
uniform int u_texturedFX;
uniform float u_texturedScale;
uniform int u_waveFX;
uniform float u_waveDisplacement;

void main()
{

   //send the vertices to the fragment shader
    varyingtexcoord = texcoord;
    
    
    //---ANIMATE-USING-SINWAVE--//
    //modified from open frameworks example simple vertex displacement
    float displacementHeight = 100.0;
    float displacementY = sin(time + (position.x / 100.0)) * u_waveDisplacement;
    float displacementX = cos(time + (position.x / 100.0)) * u_waveDisplacement;

    //================================//

    // copy position so we can work with it.
    vec4 pos = position;
    
    //--TEXTURED-DISPLACEMENT-EFFECT--//
    if (u_texturedFX == 1) {
        // we need to scale up the values we get from the texture
        float scale = 50;

        // here we get the red channel value from the texture
        // to use it as z displacement
        vec4 displacementZ = texture(tex0, varyingtexcoord);
        pos.z -= displacementZ.z * u_texturedScale;
    }
    //================================//
    
    
    
    //--MAGNIFYING-GLASS-EFFECT--//
    //modified from open frameworks example simple shader interaction
    if (u_magnifyingFX == 1) {
        // direction vector from mouse position to vertex position.
        vec2 dir = pos.xy - mousePos;

        // distance between the mouse position and vertex position.
        float dist =  distance(pos.xy, mousePos);

        // check vertex is within mouse range.
        if(dist > 0.0 && dist < mouseRange) {

            // normalise distance between 0 and 1.
            float distNorm = dist / mouseRange;

            // flip it so the closer we are the greater the repulsion.
            distNorm = 1.0 - distNorm;

            // make the direction vector magnitude fade out the further it gets from mouse position.
            dir *= distNorm;

            // add the direction vector to the vertex position.
            pos.x += dir.x;
            pos.y += dir.y;
        }
    }
    //================================//
    
    
    vec4 modifiedPosition = modelViewProjectionMatrix * pos;
    
    
    //apply sinwave to vertex position to create wave effect
    if (u_waveFX == 1) {
        modifiedPosition.y += displacementY;
        modifiedPosition.x -= displacementX;
    }
    
    //------------OUTPUT-----------//
    gl_Position = modifiedPosition;
    //==============================//
    
}
