#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct DirLight {
    vec3 direction;

    vec3 ambientDay;
    vec3 diffuseDay;
    vec3 specularDay;

    vec3 ambientNight;
    vec3 diffuseNight;
    vec3 specularNight;
};

uniform sampler2D texture1;
uniform float shininess;
uniform DirLight dirLight;
uniform vec3 viewPosition;
uniform bool noc;

vec3 CalcDirLightDay(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
//     combine results
    vec3 ambient = light.ambientDay * vec3(texture(texture1, TexCoords));
    vec3 diffuse = light.diffuseDay * diff * vec3(texture(texture1, TexCoords));
    vec3 specular = light.specularDay * spec * vec3(texture(texture1, TexCoords));
    return (ambient + diffuse + specular);
}
vec3 CalcDirLightNight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
//     combine results
    vec3 ambient = light.ambientNight * vec3(texture(texture1, TexCoords));
    vec3 diffuse = light.diffuseNight * diff * vec3(texture(texture1, TexCoords));
    vec3 specular = light.specularNight * spec * vec3(texture(texture1, TexCoords));
    return (ambient + diffuse + specular);
}


void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 result;

    if(noc){
    result = CalcDirLightNight(dirLight, normal, viewDir);
    }
    else{
    result = CalcDirLightDay(dirLight, normal, viewDir);
    }

    FragColor = vec4(result, 1.0);
}