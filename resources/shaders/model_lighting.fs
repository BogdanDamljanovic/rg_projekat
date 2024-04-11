#version 330 core
out vec4 FragColor;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 specular;
    vec3 diffuse;
    vec3 ambient;

    float constant;
    float linear;
    float quadratic;
};

struct PointLight {
    vec3 position;

    vec3 specular;
    vec3 diffuse;
    vec3 ambient;

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

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    float shininess;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform PointLight pointLight1;
uniform PointLight pointLight2;
uniform PointLight pointLight3;
uniform PointLight pointLight4;
uniform PointLight pointLight5;
uniform PointLight pointLight6;
uniform PointLight pointLight7;
uniform PointLight pointLight8;
uniform PointLight pointLight9;
uniform PointLight pointLight10;
uniform PointLight pointLight11;
uniform PointLight pointLight12;
uniform PointLight pointLight13;
uniform PointLight pointLight14;
uniform PointLight pointLight15;

uniform Material material;
uniform DirLight dirLight;

uniform SpotLight spotlight1;
uniform SpotLight spotlight2;
uniform SpotLight spotlight3;
uniform SpotLight spotlight4;
uniform SpotLight spotlight5;
uniform SpotLight spotlight6;
uniform SpotLight spotlight7;
uniform SpotLight spotlight8;
uniform SpotLight spotlight9;

uniform bool noc;
uniform vec3 viewPosition;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords).xxx);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
        vec3 lightDir = normalize(light.position - fragPos);
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
        // combine results
        vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
        vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords).xxx);

        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = (light.cutOff - light.outerCutOff);
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        diffuse  *= intensity;
        specular *= intensity;

        // attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        return (ambient + diffuse + specular);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 result;

    if(noc){
        // result = CalcSpotLight(spotlight1, normal, FragPos, viewDir);
        result = CalcSpotLight(spotlight2, normal, FragPos, viewDir);
        result += CalcSpotLight(spotlight3, normal, FragPos, viewDir);
        result += CalcSpotLight(spotlight4, normal, FragPos, viewDir);
        result += CalcSpotLight(spotlight5, normal, FragPos, viewDir);
        result += CalcSpotLight(spotlight6, normal, FragPos, viewDir);
        result += CalcSpotLight(spotlight7, normal, FragPos, viewDir);
        result += CalcSpotLight(spotlight8, normal, FragPos, viewDir);
        result += CalcSpotLight(spotlight9, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight1, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight2, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight3, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight4, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight5, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight6, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight7, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight8, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight9, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight10, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight11, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight12, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight13, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight14, normal, FragPos, viewDir);
        result += CalcPointLight(pointLight15, normal, FragPos, viewDir);
    }
    else{
        result = CalcDirLight(dirLight, normal, viewDir);
    }

    FragColor = vec4(result, 1.0);
}