#version 330 core

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosition;
in vec3 vView;
noperspective in vec2 vScreenCoord;

layout (location = 0) out vec4 fFragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D uDiffuseMap;
uniform float uDiffuseMapFactor;

uniform sampler2D uSpecularMap;
uniform float uSpecularMapFactor;

uniform samplerCube uCubeMap;
uniform float uCubeMapFactor;

uniform sampler2D uNormalMap;
uniform float uNormalMapFactor;

uniform sampler2D hdrBuffer;

struct Material {
    vec3 ambient;
    vec4 diffuse;
    vec3 specular;
    float phongExp;
};

struct DirLight {
    vec3 direction;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

uniform Material uMat;
uniform DirLight uSun;
uniform SpotLight uSpot;
uniform PointLight uPoint[5];

uniform mat4 uModel;
uniform vec3 uCameraPos;
uniform bool blinn = true;
uniform float exposure = 2.0;

vec3 fNormal;
vec3 fView;
float fShininess;

vec3 sRGB_to_linear(vec3 col) {
    return pow(col, vec3(2.2));
}

vec3 linear_to_sRGB(vec3 col) {
    return pow(col, vec3(1/2.2));
}

vec3 calc_dir_light(DirLight light, vec3 mat_ambient, vec3 mat_diffuse, vec3 mat_specular, vec3 normal, vec3 view_dir) {
    vec3 ambient = light.ambient * mat_diffuse * mat_ambient;
    vec3 diffuse = light.diffuse * mat_diffuse * max(0, dot(-light.direction, fNormal));
    vec3 reflected = reflect(light.direction, vNormal);
    vec3 halfway = normalize(light.direction + fView);
    vec3 specular = light.specular * mat_specular * pow(max(dot(normal, halfway), 0.0), fShininess);
    return ambient + diffuse + specular;
}

vec3 calc_spot_light(SpotLight light, vec3 mat_ambient, vec3 mat_diffuse, vec3 mat_specular, vec3 normal, vec3 view_dir) {
    vec3 ambient = light.ambient * mat_diffuse * mat_ambient;
    vec3 light_direction = normalize(vPosition - light.position);
    vec3 halfway = normalize(light_direction + fView);
    vec3 diffuse = light.diffuse * mat_diffuse * max(0, dot(-light_direction, fNormal));
    vec3 reflected = reflect(light_direction, fNormal);
    vec3 specular = light.specular * mat_specular * pow(max(dot(normal, halfway), 0.0), fShininess);
    float distance = distance(light.position, vPosition);
    return (ambient + diffuse + specular);
}

vec3 calc_point_light( PointLight light, vec3 mat_ambient, vec3 mat_diffuse, vec3 mat_specular, vec3 normal, vec3 view_dir) {  
    vec3 ambient = light.ambient * mat_diffuse * mat_ambient;
    vec3 light_direction = normalize(vPosition - light.position);
    vec3 halfway = normalize(light_direction + view_dir);
    vec3 diffuse = light.diffuse * mat_diffuse * max(0, dot(-light_direction, fNormal));
    vec3 reflected = reflect(light_direction, fNormal);
    vec3 specular = light.specular * mat_specular * pow(max(dot(normal, halfway), 0.0), fShininess);
    float distance = distance(light.position, vPosition);
    float attenuation = 1.0 / (1.0 + 0.1 * distance);
    return (ambient + diffuse + specular) * attenuation;
}

void main() {
    fView = normalize(vView);
    vec3 normal = normalize(vNormal);
    vec3 view_dir = normalize(vView - vPosition);
    const float gamma = 1.2;
    vec3 hdrColor = texture(hdrBuffer, vTexCoord).rgb;

    fShininess = uMat.phongExp;

    fNormal = mix(normalize(vNormal), normalize(mat3(uModel) * (texture(uNormalMap, vTexCoord).xyz * 2.0 - 1.0)), uNormalMapFactor);


    vec3 mat_ambient = uMat.ambient;
    mat_ambient = sRGB_to_linear(mat_ambient);

    // let the diffuse texture coordinates be the screen coodinate texture if water surface otherwise use given tex coords
    vec2 diffuseTexCoord = vTexCoord;


    vec4 mat_diffuse = mix(uMat.diffuse, texture(uDiffuseMap, diffuseTexCoord), uDiffuseMapFactor);
    // calculate texture direction for cubemap
    vec3 vTexDir = reflect(-fView, fNormal);
    mat_diffuse.rgb = mix(mat_diffuse, texture(uCubeMap, vTexDir), uCubeMapFactor).rgb;
    // TODO Part F: mix mat_diffuse with a reflection map using uReflectionMapFactor
    mat_diffuse.rgb = sRGB_to_linear(mat_diffuse.rgb);

    // use specular map if given otherwise the material's specular coefficient
    vec3 mat_specular = mix(uMat.specular, texture(uSpecularMap, vTexCoord).rgb, uSpecularMapFactor);
    mat_specular = sRGB_to_linear(mat_specular);

    DirLight sun = uSun;
    sun.ambient = sRGB_to_linear(sun.ambient);
    sun.diffuse = sRGB_to_linear(sun.diffuse);
    sun.specular = sRGB_to_linear(sun.specular);

    SpotLight spot = uSpot;
    spot.ambient = sRGB_to_linear(spot.ambient);
    spot.diffuse = sRGB_to_linear(spot.diffuse);
    spot.specular = sRGB_to_linear(spot.specular);
    
    vec3 shade = calc_dir_light(sun, mat_ambient, mat_diffuse.rgb, mat_specular, normal, view_dir) + calc_spot_light(spot, mat_ambient, mat_diffuse.rgb, mat_specular, normal, view_dir) * 0.01;
    
    for (int i = 0; i < 5; i++) {
        PointLight point = uPoint[i];
        point.ambient = sRGB_to_linear(point.ambient);
        point.diffuse = sRGB_to_linear(point.diffuse);
        point.specular = sRGB_to_linear(point.specular);
        shade += calc_point_light(point, mat_ambient, mat_diffuse.rgb, mat_specular, normal, view_dir) * 0.01;
    }
    shade = linear_to_sRGB(shade);
    shade -= exp(-hdrColor * exposure) * 0.05;
    shade = pow(shade, vec3(1.0 / gamma));
    fFragColor = vec4(shade, mat_diffuse.a);
    float brightness = dot(shade, vec3(0.7, 0.7, 0.1));
    if (brightness > 1.0) {
        BrightColor = vec4(shade, 1.0);
    } else {
        BrightColor = vec4(0, 0, 0, 1);
    }
}
