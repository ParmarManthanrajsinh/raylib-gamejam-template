#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main()
{
    vec4 texColor = texture2D(texture0, fragTexCoord);
    vec4 bloom = vec4(0.0);
    
    float dx = 2.0 / 720.0;
    float dy = 2.0 / 720.0;

    for(int x = -3; x <= 3; x++) 
    {
        for(int y = -3; y <= 3; y++) 
        {
            vec4 c = texture2D(texture0, fragTexCoord + vec2(float(x) * dx, float(y) * dy));
            float brightness = dot(c.rgb, vec3(0.2126, 0.7152, 0.0722));
            if(brightness > 0.75) 
            {
                bloom += c * 0.004;
            }
        }
    }
    
    gl_FragColor = texColor + bloom * 0.4;
}
