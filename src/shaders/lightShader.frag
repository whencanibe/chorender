#version 330 core

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform DirLight dirLight;

uniform float uShininess;
uniform vec3 uKs;
uniform vec3 uViewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform bool ubIsSpotLight;

// Directional-light shadow map
uniform sampler2D shadowMap;
uniform bool uShadowsEnabled;
uniform float uShadowBias;  // depth offset in light-space [0,1] units; scaled by surface slope below
uniform int uPcfRadius;     // 0 = hard edge, 1 = 3x3 PCF, 2 = 5x5 PCF

float ShadowFactor(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);

    vec3 result = vec3(0.0);

    result += CalcDirLight(dirLight, norm, viewDir);

    for (int i = 0 ; i < NR_POINT_LIGHTS; i++)
    {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    
    if (ubIsSpotLight)
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    // Linear HDR out. Tone mapping and gamma happen in the post pass (screen.frag).
    FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir,normal),0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    spec *= step(0.0001, diff);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    // vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    vec3 specular = light.specular * spec * uKs;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir,normal),0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
    spec *= step(0.0001, diff);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    // vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    vec3 specular = light.specular * spec * uKs;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir),0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir),0.0), uShininess);
    spec *= step(0.0001, diff);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * uKs;

    // Shadow only blocks direct light; ambient stands in for light bouncing in from elsewhere.
    float shadow = uShadowsEnabled ? ShadowFactor(FragPosLightSpace, normal, lightDir) : 0.0;
    return ambient + (1.0 - shadow) * (diffuse + specular);
}

// Returns 0 = fully lit, 1 = fully in shadow.
float ShadowFactor(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // Clip space -> NDC -> [0,1] so xy index the shadow map and z compares against stored depth.
    // (w is 1.0 for an orthographic light, but dividing keeps this correct for perspective too.)
    vec3 proj = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj = proj * 0.5 + 0.5;

    // Beyond the light's far plane: nothing in the map can occlude it.
    if (proj.z > 1.0)
        return 0.0;

    // Slope-scaled bias: surfaces at a grazing angle to the light need a larger offset
    // to avoid shadow acne, surfaces facing the light need less to avoid peter-panning.
    float bias = max(uShadowBias * (1.0 - dot(normal, lightDir)), uShadowBias * 0.1);
    float currentDepth = proj.z - bias;

    // Percentage-closer filtering: average the binary test over neighbouring texels.
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float shadow = 0.0;
    int samples = 0;
    for (int x = -uPcfRadius; x <= uPcfRadius; ++x)
    {
        for (int y = -uPcfRadius; y <= uPcfRadius; ++y)
        {
            float closestDepth = texture(shadowMap, proj.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth > closestDepth) ? 1.0 : 0.0;
            samples++;
        }
    }
    return shadow / float(samples);
}