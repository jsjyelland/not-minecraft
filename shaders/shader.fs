#version 330 core

struct Material {
   sampler2D diffuse;
   sampler2D specular;
   sampler2D emission;

   float shininess;
};

in vec2 TexCoords;

// Lighting Structs

struct DirLight {
   vec3 direction;
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

uniform DirLight dirLight;

struct PointLight {
   vec3 position;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float constant;
   float linear;
   float quadratic;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform Material material;

uniform vec3 viewPos;

vec4 calcFog(vec3 viewFragPos, vec4 color);
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
   vec3 norm = normalize(Normal);
   vec3 viewFragPos = viewPos - FragPos;
   vec3 viewDir = normalize(viewFragPos);

   vec4 result = vec4(calcDirLight(dirLight, norm, viewDir), texture(material.diffuse, TexCoords).a);
   

   // vec4 result = texture(material.diffuse, TexCoords);

   if (result.a < 0.1) {
      discard;
   }

   // for (int i = 0; i < NR_POINT_LIGHTS; i++) {
   //    result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
   // }

   // vec3 emission = vec3(texture(material.emission, TexCoords));

   float gamma = 2.2;
   FragColor = vec4(pow(result.xyz, vec3(1.0/gamma)) - 0.2, result.w);
   FragColor = calcFog(viewFragPos, FragColor);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
   vec3 lightDir = normalize(-light.direction);

   // Diffuse shading
   float diff = max(dot(normal, lightDir), 0.0);

   // Specular shading
   vec3 halfwayDir = normalize(lightDir + viewDir);
   float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

   vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
   vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
   vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

   return ambient + diffuse + specular;
}

vec4 calcFog(vec3 viewFragPos, vec4 color) {
   float dist = length(viewFragPos);

   if (dist > 200) {
      return color * vec4(0.7, 0.7, 0.7, 1.0);
   } else {
      return color;
   }
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
   // Ambient
   vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

   float distance = length(light.position - fragPos);
   float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

   // Diffuse
   vec3 lightDir = normalize(light.position - fragPos);
   float diff = max(dot(normal, lightDir), 0.0);
   vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

   // Specular
   vec3 halfwayDir = normalize(lightDir + viewDir);
   float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
   vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

   vec3 result = (ambient + diffuse + specular) * attenuation;

   return result;
}