#version 330 core

uniform sampler2D texture;
uniform vec2 playerDirection;
uniform float dashIntensity;
uniform float dashTimer;

void main() {
    vec2 texCoord = gl_TexCoord[0].xy;
    vec2 center = vec2(0.5, 0.5);
    
    // Get the original pixel color
    vec4 originalColor = texture2D(texture, texCoord);
    
    // Basic motion blur
    vec4 blurredColor = vec4(0.0);
    float totalWeight = 0.0;
    
    // Sample along the motion vector
    const int numSamples = 8;
    float maxBlurDistance = dashIntensity * 0.04;
    
    for (int i = 0; i < numSamples; i++) {
        float weight = 1.0 - float(i) / float(numSamples);
        float offset = float(i) / float(numSamples - 1) * maxBlurDistance;
        
        // Sample in the direction opposite to movement (creates motion blur trail)
        vec2 sampleCoord = texCoord - playerDirection * offset;
        
        blurredColor += texture2D(texture, sampleCoord) * weight;
        totalWeight += weight;
    }
    
    // Normalize the blurred color
    blurredColor /= totalWeight;
    
    // Calculate pulse effect for dynamic appearance
    float pulse = 0.5 + 0.5 * sin(dashTimer * dashEffectSpeed);
    
    // Apply green tint similar to your original code
    blurredColor.g = min(1.0, blurredColor.g + 0.15 * pulse * dashIntensity);
    blurredColor.r = min(1.0, blurredColor.r + 0.05 * pulse * dashIntensity);
    blurredColor.b = min(1.0, blurredColor.b + 0.05 * pulse * dashIntensity);
    
    // Add slash effect
    float slashAngle = atan(playerDirection.y, playerDirection.x) + 1.57 + 0.2 * sin(dashTimer * 3.0);
    vec2 slashDir = vec2(cos(slashAngle), sin(slashAngle));
    
    // Relative position from center
    vec2 relPos = texCoord - center;
    
    // Distance to slash line (adjusting for aspect ratio if needed)
    float distToLine = abs(dot(normalize(slashDir), normalize(relPos)));
    float distFromCenter = length(relPos);
    
    // Calculate slash intensity
    float slashWidth = 0.3 * dashIntensity;
    float slashCurve = 0.5 * sin(length(relPos) * 10.0 + dashTimer * 5.0);
    float slashFactor = smoothstep(0.7, 1.0, 1.0 - distToLine) * 
                       (1.0 - smoothstep(0.4, 0.5, distFromCenter));
    
    // Only add slash effect where it would be visible
    if (slashFactor > 0.01) {
        // Define slash colors (bright green/teal core fading to darker edges)
        vec3 slashCoreColor = vec3(0.8, 1.0, 0.9);
        vec3 slashEdgeColor = vec3(0.2, 0.7, 0.3);
        
        // Mix between core and edge based on distance
        vec3 slashColor = mix(slashCoreColor, slashEdgeColor, 1.0 - slashFactor);
        
        // Create pulsing effect
        float shimmer = 0.7 + 0.3 * sin(dashTimer * 12.0 + distFromCenter * 20.0);
        slashColor *= shimmer;
        
        // Add the slash color to the blurred color with pulse effect
        float slashIntensityWithPulse = slashFactor * pulse * dashIntensity * 1.5;
        blurredColor.rgb = mix(blurredColor.rgb, slashColor, slashIntensityWithPulse);
        
        // Add some extra brightness for glow effect
        blurredColor.rgb += slashColor * slashIntensityWithPulse * 0.3;
    }
    
    // Add screen edge glow for more dramatic effect
    float edgeGlow = (1.0 - smoothstep(0.4, 0.5, distFromCenter)) * 0.1 * dashIntensity * pulse;
    blurredColor.g += edgeGlow;
    
    // Final color
    gl_FragColor = blurredColor;
}