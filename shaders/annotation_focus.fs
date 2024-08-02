#version 330 core

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// struct Rectangle {
//     vec2 topLeft;
//     vec2 size;
// };

// Input uniform values
uniform sampler2D texture0;
uniform vec4 bbox;
uniform bool IsActive;

// Output fragment color
out vec4 finalColor;

float ShadowIntensity = 0.4;
void main()
{
        vec4 texelColor = texture(texture0, fragTexCoord);
        if (IsActive)
        {
            vec2 bottomRight = vec2(bbox.x + bbox.z, bbox.y + bbox.w);
            if (fragTexCoord.x >= bbox.x && fragTexCoord.x <= bottomRight.x &&
                fragTexCoord.y >= bbox.y && fragTexCoord.y <= bottomRight.y)
            {
                finalColor = texelColor;
            }
            else
            {
                finalColor = texelColor * vec4(ShadowIntensity, ShadowIntensity,ShadowIntensity,1.0);
            }
        }
        else
        {
             finalColor = texelColor;
        }
}
